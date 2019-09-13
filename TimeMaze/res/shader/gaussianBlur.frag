#version 420 core

layout (binding = 0) uniform sampler2D colorSampler;

uniform float kernel[17];
uniform float Z;

in vec2 texCoord;

out vec4 fragColor;

void main()
{
	//declare stuff
	vec3 final_color = vec3(0.0);
	
	//read out the texels
	for (int i= -8; i <= 8; ++i)
	{
		for (int j= -8; j <= 8; ++j)
		{
			final_color += kernel[8 + j] * kernel[8 + i] * texelFetch(colorSampler, ivec2(gl_FragCoord.xy) + ivec2(i, j), 0).rgb;
		}
	}
	
	fragColor = vec4(final_color / (Z * Z), 1.0);
}
