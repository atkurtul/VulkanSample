#version 450

layout(location = 0) out vec4 color;
layout(location = 0) in vec2 tex;

layout(set=1, binding=0) uniform sampler2D diffuse;

void main() 
{
	color = texture(diffuse, tex);
}