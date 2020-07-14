#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 0) out vec2 outtex;

layout(set=0, binding=0) uniform UBO00 {
    mat4 prj;
    vec4 pos;
} cam;


void main() 
{
    gl_Position = cam.prj * vec4(cam.pos.xyz +  pos, 1);
    outtex = tex;
}