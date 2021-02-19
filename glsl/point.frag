#version 460

uniform vec2 resolution;

uniform mat4 invView;
uniform mat4 invProj;
uniform mat4 proj;
uniform mat4 view;

#define Vector3 vec3
in vec3 boxPos;
out vec4 fragColor;

const vec3 voxelSize = vec3(0.5);

float maxComponent(vec3 a)
{
    return max(a.x, max(a.y, a.z));
}

struct Ray
{
    vec3 origin;
    vec3 direction;
};


float deg2rad(const float deg)
{
    const float M_PI = 3.14159265359;
    return deg * 2.0*M_PI / 180.0;
}

Ray calcRay()
{
    Ray ray;
    const vec3 cameraPos = invView[3].xyz;

    const float x = gl_FragCoord.x;
    const float y = gl_FragCoord.y;
    const float px = (2.0 * ((x) / resolution.x) - 1.0);
    const float py = (2.0 * ((y) / resolution.y) - 1.0);
    const vec4 pt = invView*invProj*vec4(px, py, -1, 1);
    const vec3 world_pt = pt.xyz / pt.w;

    ray.origin = cameraPos;
    ray.direction = normalize(world_pt - cameraPos);

    return ray;
}

struct Box
{
    vec3 center;
    vec3 radius;
    mat3 rotation;
    vec3 invRadius;
};

// vec3 box.radius:       independent half-length along the X, Y, and Z axes
// mat3 box.rotation:     box-to-world rotation (orthonormal 3x3 matrix) transformation
// bool rayCanStartInBox: if true, assume the origin is never in a box. GLSL optimizes this at compile time
// bool oriented:         if false, ignore box.rotation
bool ourIntersectBoxCommon(Box box,
                           Ray ray,
                           out float distance,
                           out vec3 normal,
                           const bool rayCanStartInBox,
                           const in bool oriented,
                           in vec3 _invRayDirection) 
{
    // Move to the box's reference frame. This is unavoidable and un-optimizable.
    ray.origin = box.rotation * (ray.origin - box.center);
    if (oriented) {
        ray.direction = ray.direction * box.rotation;
    }
    
    // This "rayCanStartInBox" branch is evaluated at compile time because `const` in GLSL
    // means compile-time constant. The multiplication by 1.0 will likewise be compiled out
    // when rayCanStartInBox = false.
    float winding;
    if (rayCanStartInBox) {
        // Winding direction: -1 if the ray starts inside of the box (i.e., and is leaving), +1 if it is starting outside of the box
        winding = (maxComponent(abs(ray.origin) * box.invRadius) < 1.0) ? -1.0 : 1.0;
    } else {
        winding = 1.0;
    }

    // We'll use the negated sign of the ray direction in several places, so precompute it.
    // The sign() instruction is fast...but surprisingly not so fast that storing the result
    // temporarily isn't an advantage.
    Vector3 sgn = -sign(ray.direction);

	// Ray-plane intersection. For each pair of planes, choose the one that is front-facing
    // to the ray and compute the distance to it.
    Vector3 distanceToPlane = box.radius * winding * sgn - ray.origin;
    if (oriented) {
        distanceToPlane /= ray.direction;
    } else {
        distanceToPlane *= _invRayDirection;
    }

    // Perform all three ray-box tests and cast to 0 or 1 on each axis. 
    // Use a macro to eliminate the redundant code (no efficiency boost from doing so, of course!)
    // Could be written with 
#   define TEST(U, VW)\
         /* Is there a hit on this axis in front of the origin? Use multiplication instead of && for a small speedup */\
         (distanceToPlane.U >= 0.0) && \
         /* Is that hit within the face of the box? */\
         all(lessThan(abs(ray.origin.VW + ray.direction.VW * distanceToPlane.U), box.radius.VW))

    bvec3 test = bvec3(TEST(x, yz), TEST(y, zx), TEST(z, xy));

    // CMOV chain that guarantees exactly one element of sgn is preserved and that the value has the right sign
    sgn = test.x ? vec3(sgn.x, 0.0, 0.0) : (test.y ? vec3(0.0, sgn.y, 0.0) : vec3(0.0, 0.0, test.z ? sgn.z : 0.0));    
#   undef TEST
        
    // At most one element of sgn is non-zero now. That element carries the negative sign of the 
    // ray direction as well. Notice that we were able to drop storage of the test vector from registers,
    // because it will never be used again.

    // Mask the distance by the non-zero axis
    // Dot product is faster than this CMOV chain, but doesn't work when distanceToPlane contains nans or infs. 
    //
    distance = (sgn.x != 0.0) ? distanceToPlane.x : ((sgn.y != 0.0) ? distanceToPlane.y : distanceToPlane.z);

    // Normal must face back along the ray. If you need
    // to know whether we're entering or leaving the box, 
    // then just look at the value of winding. If you need
    // texture coordinates, then use box.invDirection * hitPoint.
    
    if (oriented) {
        normal = box.rotation * sgn;
    } else {
        normal = sgn;
    }
    
    return (sgn.x != 0) || (sgn.y != 0) || (sgn.z != 0);
}

mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    const float s = sin(angle);
    const float c = cos(angle);
    const float oc = 1.0 - c;

    return mat3(oc*axis.x*axis.x + c, oc*axis.x*axis.y - axis.z*s, oc*axis.z*axis.x + axis.y*s,
                oc*axis.x*axis.y + axis.z*s,  oc*axis.y*axis.y + c, oc*axis.y*axis.z - axis.x*s,
                oc*axis.z*axis.x - axis.y*s,  oc*axis.y*axis.z + axis.x*s, oc*axis.z*axis.z + c);
}

Box calcBox()
{
    Box box;
    box.center = boxPos;
    box.radius = voxelSize;
    box.invRadius = 1.0 / voxelSize;
    box.rotation = mat3(1.0);//rotationMatrix(vec3(0., 1., 0.), deg2rad(30.0));

    return box;
}

struct Result
{
    bool hit;
    float distance;
    vec3 normal;
};

Result intersectBox(Ray ray, Box box)
{
    float distance;
    vec3 normal;
    const vec3 invRayDirection = 1.0 / ray.direction;
    const bool intersects =
            ourIntersectBoxCommon(box,
                                  ray,
                                  distance,
                                  normal,
                                  true,
                                  false,
                                  invRayDirection);

    Result res;
    res.hit = intersects;
    res.normal = normal;
    res.distance = distance;

    return res;
}

void main()
{
    Ray ray = calcRay();
    Box box = calcBox();
    Result res = intersectBox(ray, box);

    if (res.hit)
    {
        gl_FragDepth = res.distance / 1000.0;
        fragColor = vec4(abs(res.normal), 1.0);
    }
    else
    {
    //fragColor = vec4(1);
        discard;
    }
}
