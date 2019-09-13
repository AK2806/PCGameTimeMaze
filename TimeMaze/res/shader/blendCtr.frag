#version 420 core

uniform vec3 materialColor;
uniform float alpha;

uniform float time;
uniform float animationDuring;

layout (binding = 0) uniform sampler2D colorSampler;
uniform bool useColormap;
layout (binding = 1) uniform sampler2D normalSampler;
uniform bool useNormalmap;
layout (binding = 2) uniform samplerCube envSampler;
uniform bool useEnvmap;
layout (binding = 3) uniform sampler2D specSampler;
uniform bool useSpecmap;
layout (binding = 4) uniform sampler3D aniSampler;
uniform bool useAnimap;
layout (binding = 5) uniform sampler2DMS depth_sampler;

uniform mat4 view;

uniform bool texColor_inverse;
uniform bool texAlpha_inverse;
uniform uint blend_param;
uniform uint max_length;
layout (binding = 0) uniform atomic_uint index_counter;
layout (binding = 0, rgba32ui) uniform uimageBuffer list_buffer;
layout (binding = 1, r32ui) uniform uimage2D head_ptr_image;

in vec3 fTangent;
in vec3 fBitangent;
in vec3 fvsNormal;
in vec3 fvsPos;
in vec2 ftexCoord;

// OIT algorithm
void main()
{
	if (gl_FragCoord.z <= texelFetch(depth_sampler, ivec2(gl_FragCoord.xy), 0).x) {	
		uint index = atomicCounterIncrement(index_counter);
		if (index >= max_length) discard;

		vec3 fvsNormalN = normalize(fvsNormal);
		if (useNormalmap) {
			mat3 mTBN = mat3(normalize(fTangent), normalize(fBitangent), fvsNormalN);
			fvsNormalN = normalize(mTBN * (2 * texture(normalSampler, ftexCoord).xyz - vec3(1.0)));
		}

		uint channelLink = blend_param & 0xFF;
		vec4 fragColor = vec4(materialColor, alpha);
		if (useColormap) {
			vec4 texColor = texture(colorSampler, ftexCoord);
			vec4 transColor = vec4(0.0);
			transColor.r = texColor[channelLink >> 6];
			transColor.g = texColor[(channelLink >> 4) & 3];
			transColor.b = texColor[(channelLink >> 2) & 3];
			if (texColor_inverse) {
				transColor.r = 1.0 - transColor.r;
				transColor.g = 1.0 - transColor.g;
				transColor.b = 1.0 - transColor.b;
			}
			transColor.a = texColor[channelLink & 3];
			if (texAlpha_inverse) {
				transColor.a = 1.0 - transColor.a;
			}
			fragColor *= transColor;
		}
		if (useEnvmap) {
			vec4 texColor = texture(envSampler, reflect(fvsPos, fvsNormalN));
			vec4 transColor = vec4(0.0);
			transColor.r = texColor[channelLink >> 6];
			transColor.g = texColor[(channelLink >> 4) & 3];
			transColor.b = texColor[(channelLink >> 2) & 3];
			if (texColor_inverse) {
				transColor.r = 1.0 - transColor.r;
				transColor.g = 1.0 - transColor.g;
				transColor.b = 1.0 - transColor.b;
			}
			transColor.a = texColor[channelLink & 3];
			if (texAlpha_inverse) {
				transColor.a = 1.0 - transColor.a;
			}
			fragColor *= transColor;
		}
		if (useAnimap) {
			vec4 texColor = texture(aniSampler, vec3(ftexCoord, time / animationDuring));
			vec4 transColor = vec4(0.0);
			transColor.r = texColor[channelLink >> 6];
			transColor.g = texColor[(channelLink >> 4) & 3];
			transColor.b = texColor[(channelLink >> 2) & 3];
			if (texColor_inverse) {
				transColor.r = 1.0 - transColor.r;
				transColor.g = 1.0 - transColor.g;
				transColor.b = 1.0 - transColor.b;
			}
			transColor.a = texColor[channelLink & 3];
			if (texAlpha_inverse) {
				transColor.a = 1.0 - transColor.a;
			}
			fragColor *= transColor;
		}

		float lightness = max(fragColor.r, max(fragColor.g, fragColor.b));
		uint lightness16bit = packHalf2x16(vec2(lightness, 0)) & 0xFFFF;
		fragColor = vec4(fragColor.rgb / lightness, fragColor.a);

		uint second = imageAtomicExchange(head_ptr_image, ivec2(gl_FragCoord.xy), index);
		uvec4 newFragment;
		newFragment.x = second;
		newFragment.y = packUnorm4x8(fragColor);
		newFragment.z = floatBitsToUint(gl_FragCoord.z);
		newFragment.w = ((blend_param & 0xFF00) >> 8) | lightness16bit << 8;
		imageStore(list_buffer, int(index), newFragment);
	}
}