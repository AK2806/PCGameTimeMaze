#version 420 core

layout (binding = 0) uniform sampler2D baseColorSampler;
layout (binding = 1) uniform sampler2D highlightColorSampler;

uniform float bloom;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
	fragColor = texture(baseColorSampler, texCoord) + bloom * texture(highlightColorSampler, texCoord);
}