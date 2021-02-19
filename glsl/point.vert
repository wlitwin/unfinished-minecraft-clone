#version 460

layout(location = 0) in vec3 position;

uniform vec2 resolution;
uniform vec3 chunkOffset;
uniform mat4 proj;
uniform mat4 view;
const vec3 voxelSize = vec3(0.5);
const vec3 cubeCoordScale = vec3(255);
uniform sampler2D cubeLocs;

out vec3 boxPos;

void calcScreenSize(in vec3 boxPos,
                    out float pointSize,
                    out vec4 outPosition)
{
    const mat4 MVP = proj*view;
    const vec4 p1 = MVP*(vec4(boxPos + voxelSize, 1));
    const vec4 p2 = MVP*(vec4(boxPos - voxelSize, 1));
    const vec4 p3 = MVP*(vec4(boxPos + vec3(-voxelSize.x,  voxelSize.y,  voxelSize.z), 1));
    const vec4 p4 = MVP*(vec4(boxPos + vec3( voxelSize.x, -voxelSize.y,  voxelSize.z), 1));
    const vec4 p5 = MVP*(vec4(boxPos + vec3( voxelSize.x,  voxelSize.y, -voxelSize.z), 1));
    const vec4 p6 = MVP*(vec4(boxPos + vec3(-voxelSize.x, -voxelSize.y,  voxelSize.z), 1));
    const vec4 p7 = MVP*(vec4(boxPos + vec3( voxelSize.x, -voxelSize.y, -voxelSize.z), 1));
    const vec4 p8 = MVP*(vec4(boxPos + vec3(-voxelSize.x,  voxelSize.y, -voxelSize.z), 1));

    const vec3 p1s = p1.xyz / p1.w;
    const vec3 p2s = p2.xyz / p2.w;
    const vec3 p3s = p3.xyz / p3.w;
    const vec3 p4s = p4.xyz / p4.w;
    const vec3 p5s = p5.xyz / p5.w;
    const vec3 p6s = p6.xyz / p6.w;
    const vec3 p7s = p7.xyz / p7.w;
    const vec3 p8s = p8.xyz / p8.w;

    const float max_x = max(max(max(p1s.x, p2s.x), max(p3s.x, p4s.x)), max(max(p5s.x, p6s.x), max(p7s.x, p8s.x)));
    const float min_x = min(min(min(p1s.x, p2s.x), min(p3s.x, p4s.x)), min(min(p5s.x, p6s.x), min(p7s.x, p8s.x)));
    const float max_y = max(max(max(p1s.y, p2s.y), max(p3s.y, p4s.y)), max(max(p5s.y, p6s.y), max(p7s.y, p8s.y)));
    const float min_y = min(min(min(p1s.y, p2s.y), min(p3s.y, p4s.y)), min(min(p5s.y, p6s.y), min(p7s.y, p8s.y)));
    const float max_z = max(max(max(p1s.z, p2s.z), max(p3s.z, p4s.z)), max(max(p5s.z, p6s.z), max(p7s.z, p8s.z)));
    const float min_z = min(min(min(p1s.z, p2s.z), min(p3s.z, p4s.z)), min(min(p5s.z, p6s.z), min(p7s.z, p8s.z)));

    const vec3 scale = vec3(max_x - min_x, max_y - min_y, max_z - min_z)*0.5;
    const vec3 center = vec3(min_x, min_y, min_z) + scale;

    pointSize = scale.x*resolution.x; 
    outPosition = vec4(position.xy*scale.xy + center.xy, 0, 1.0);
}

void main()
{
    const float count = 4096;
    const float div = 1.0 / count;
    const float x = mod(gl_InstanceID, count);
    const float y = floor(gl_InstanceID/count);
    const vec3 tx = texture(cubeLocs, vec2(x, y)*div).rgb;
    const vec3 pos = vec3(tx * cubeCoordScale + chunkOffset);
    boxPos = pos;

    calcScreenSize(pos, gl_PointSize, gl_Position);
    // TODO - we're setting gl_FragDepth in the fragment shader
    //        so we probably don't need the z-coordinate anymore.
    //        should check if there's a depth-check between the
    //        vertex and fragment shaders.
    //gl_Position = vec4(position.xy*scale.xy + center.xy, 0, 1.0);
    //gl_PointSize = scale.x*resolution.x;
    //gl_Position = vec4(position*scale + center, 1.0);
}
