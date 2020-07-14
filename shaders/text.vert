#version 450

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;
layout(location = 0) out vec2 txx;

void main() 
{
    gl_Position = vec4(pos, 0, 1);
    txx = tex; 
}