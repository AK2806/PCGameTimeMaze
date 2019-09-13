#version 420 core

layout (binding = 0) uniform sampler2D colorSampler;

in vec2 texCoord;

out vec4 baseFragColor;
out vec4 highlightFragColor;

void main()
{
	vec4 color = texture(colorSampler, texCoord);
	baseFragColor = color;
	float brightness = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
	highlightFragColor = brightness * color;
}