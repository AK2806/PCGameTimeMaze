#version 420 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 vTexCoord;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(vPosition, 0.0, 1.0);
	texCoord = vTexCoord;
}