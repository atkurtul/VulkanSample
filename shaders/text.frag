#version 450

layout(location = 0) in vec2 txx;
layout(location = 0) out vec4 color;

layout(set = 3, binding = 0) uniform UBO30 { mat4 m; } _30;
layout(set = 3, binding = 1) uniform UBO31 { mat4 m; } _31;
layout(set = 3, binding = 2) uniform UBO32 { mat4 m; } _32;

void main() 
{
	color = vec4(1);
}