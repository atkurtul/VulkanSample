#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 tex;
layout(location = 5) in uvec3   boneid;
layout(location = 6) in vec3    bonewgt;


layout(location = 0) out vec2 outtex;
layout(location = 1) out mat3 outnorm;
layout(location = 4) out vec4 fragpos;
layout(location = 5) out vec3 fnorm;


layout(set = 0, binding = 0) uniform UBO00 {
    mat4 prj;
} cam;

layout(set = 1, binding = 0) uniform UBO10 {
    mat4 m[64];
} bones;

layout(push_constant) uniform push_block {
    mat4 xf;
    int ss;
} xf;

void main() 
{
   mat4 bone_mat = 
        bones.m[boneid.x] * bonewgt.x + 
        bones.m[boneid.y] * bonewgt.y + 
        bones.m[boneid.z] * bonewgt.z;
    
    vec4 pp = bone_mat * vec4(pos, 1);
    if(xf.ss == 1)
        pp = vec4(pos, 1) * bone_mat;
    fragpos = xf.xf * pp;
    gl_Position = cam.prj * fragpos;
    outtex = tex;
    vec3 T = normalize(xf.xf * vec4(tangent, 0.0)).xyz;
    vec3 B = normalize(xf.xf * vec4(bitangent, 0.0)).xyz;
    vec3 N = normalize(xf.xf * vec4(norm, 0.0)).xyz;
    outnorm = mat3(T, B, N);
    fnorm = (xf.xf * vec4(norm, 1)).xyz;
}