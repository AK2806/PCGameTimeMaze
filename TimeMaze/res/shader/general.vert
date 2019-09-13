#version 420 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vNormal;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vsNormal;
out vec3 vsPos;
out vec2 texCoord;

void main()
{
	gl_Position = projection * view * model * vPosition;
	vec4 v4vsPos = view * model * vPosition;
	vsPos = v4vsPos.xyz / v4vsPos.w;
	vsNormal = (transpose(inverse(view * model)) * vNormal).xyz;
	texCoord = vTexCoord;
}