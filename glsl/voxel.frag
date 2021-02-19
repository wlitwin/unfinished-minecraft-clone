#version 460

flat in int material;
in vec4 cubePos;
in float depth;
uniform float alpha;

uniform sampler2D texSamp;

out vec4 fragColor;

// want to avoid multiplying by matrices
// may need to pass more geometry instead...

const float tsx = 1.0 / 2.9999999;
const float tsy = 0.5;

const vec2 tOffset[] = {
    vec2(tsx, tsy),
    vec2(tsx, tsy),
    vec2(0, tsy),
    vec2(2*tsx, 0),
    vec2(2*tsx, 0),
    vec2(tsx*2, tsy),
    vec2(tsx*2, tsy),
    vec2(tsx, tsy),
    vec2(tsx, tsy),
    vec2(0, tsy),
    vec2(0, 0),
    vec2(0, 0),
};

const mat3 transforms[] = {
    mat3(-tsx, 0, 0,  // Front
         0, -tsy, 0,
         0, 0,  0),
    mat3(-tsx, 0, 0,  // Front
         0, -tsy, 0,
         0, 0,  0),
    mat3(-tsx, 0, 0,  // Bottom
         0, 0,  tsy,
         0, 0,  0),
    mat3(0, 0, -tsx,  // Right
         0, -tsy, 0,
         0, 0,  0),
    mat3(0, 0, -tsx,  // Right
         0, -tsy, 0,
         0, 0,  0),
    mat3(tsx, 0, 0,  // Top
         0, 0,  -tsy,
         0, 0,  0),
    mat3(tsx, 0, 0,  // Top
         0, 0,  -tsy,
         0, 0,  0),
    mat3(0, 0, tsx,  // Left
         0, -tsy,  0,
         0, 0,  0),
    mat3(0, 0, tsx,  // Left
         0, -tsy,  0,
         0, 0,  0),
    mat3(-tsx, 0, 0,  // Bottom
         0, 0,  tsy,
         0, 0,  0),
    mat3(tsx, 0, 0,  // Back
         0, -tsy, 0,
         0, 0,  0),
    mat3(tsx, 0, 0,  // Back
         0, -tsy, 0,
         0, 0,  0),
};

const vec3 normals[] = {
    vec3(0, 0, -1), // Front
    vec3(0, 0, -1), // Front
    vec3(0, -1, 0), // Bottom
    vec3(1, 0, 0),  // Right
    vec3(1, 0, 0),  // Right 
    vec3(0, 1, 0),  // Top
    vec3(0, 1, 0),  // Top
    vec3(-1, 0, 0), // Left
    vec3(-1, 0, 0), // Left
    vec3(0, -1, 0), // Bottom
    vec3(0, 0, 1),  // Back
    vec3(0, 0, 1),  // Back
};

vec3 gamma(vec3 color){
    return pow(color, vec3(1.0/2.0));
}

vec3 fog(vec3 color, vec3 fcolor, float depth, float density){
    const float e = 2.71828182845904523536028747135266249;
    float f = pow(e, -pow(depth*density, 2));
    return mix(fcolor, color, f);
}

float fogFactorExp2(
  const float dist,
  const float density)
{
  const float LOG2 = -1.442695;
  float d = density * dist;
  return 1.0 - clamp(exp2(d * d * LOG2), 0.0, 1.0);
}

vec2 calcTCoord()
{
    const vec2 offset = tOffset[gl_PrimitiveID];
    const vec2 tcoord = (mod(cubePos.xyz, 1.0)*transforms[gl_PrimitiveID]).xy;
    return tcoord + offset;
}

void main()
{
    const vec3 n = normals[gl_PrimitiveID];
    const vec2 tcoord = calcTCoord();
    const vec3 color = texture(texSamp, tcoord).rgb;
    const vec3 result = fog(color*alpha, vec3(0.8), depth, 0.005);
    fragColor = vec4(gamma(result), 1.0);
}
