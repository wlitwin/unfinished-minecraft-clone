#version 460

layout(location = 0) in float position;

uniform mat4 MVP;
uniform mat4 proj;
uniform mat4 view;
uniform sampler1D cubeLocs;
uniform isampler1D cubeMats;
uniform vec3 offset;

flat out int material;
out float depth;
out vec4 cubePos;

const vec3 voxelSize = vec3(1);
const vec3 vertexLUT[8] = {
    vec3(0,           voxelSize.y, 0),
    vec3(voxelSize.x, voxelSize.y, 0),
    vec3(0,           0,           0),
    vec3(voxelSize.x, 0,           0),
    vec3(voxelSize.x, 0,           voxelSize.z),
    vec3(voxelSize.x, voxelSize.y, voxelSize.z),
    vec3(0,           voxelSize.y, voxelSize.z),
    vec3(0,           0,           voxelSize.z),
};

/*
mat4 rotationMatrix(vec3 axis, float angle, vec3 pos)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,
                oc * axis.x * axis.y - axis.z * s, 
                oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s, 
                oc * axis.y * axis.y + c,          
                oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  
                oc * axis.y * axis.z + axis.x * s,  
                oc * axis.z * axis.z + c, 0.0,
                pos.x, pos.y, pos.z, 1.0);
}
*/

void main()
{
    material = texelFetch(cubeMats, gl_InstanceID, 0).r;
    const vec4 pos_scale = texelFetch(cubeLocs, gl_InstanceID, 0).rgba;
    const float scale = pos_scale.w*255.0;
    cubePos = vec4(vertexLUT[int(position)]*scale, scale);
    const vec3 boxPos = pos_scale.xyz * vec3(255) + cubePos.xyz + offset;
    gl_Position = MVP*vec4(boxPos, 1);
    depth = length(gl_Position.xyz);
}
