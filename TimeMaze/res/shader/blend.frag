#version 420 core

layout (binding = 0, rgba32ui) uniform uimageBuffer list_buffer;
layout (binding = 1, r32ui) uniform uimage2D head_ptr_image;

layout (binding = 0) uniform sampler2DMS ssColorSampler;

in vec2 texCoord;

out vec4 fragColor;

vec4 alphaBlend(vec4 src, vec4 dst) {
	return mix(dst, src, src.a);
}

vec4 additiveBlend(vec4 src, vec4 dst) {
	return dst + src;
}

vec4 multiplyBlend(vec4 src, vec4 dst) {
	return dst * src;
}

void main()
{
	fragColor = vec4(0.0);
	for (int i = 0; i < 8; ++i) {
		fragColor += texelFetch(ssColorSampler, ivec2(gl_FragCoord.xy), i);
	}
	fragColor /= 8;

	// OIT algorithm
	uint end = 0xFFFFFFFF;
	uint ptr;
	while ((ptr = imageLoad(head_ptr_image, ivec2(gl_FragCoord.xy)).x) != end) {
		// bubble sort
		uvec4 cur_data;
		uvec4 next_data;
		while (true) {
			cur_data = imageLoad(list_buffer, int(ptr));
			if (cur_data.x == end) break;
			next_data = imageLoad(list_buffer, int(cur_data.x));
			float depth_cur = uintBitsToFloat(cur_data.z);
			float depth_next = uintBitsToFloat(next_data.z);
			if (depth_cur < depth_next) { // exchange content
				imageStore(list_buffer, int(ptr), uvec4(cur_data.x, next_data.yzw));
				imageStore(list_buffer, int(cur_data.x), uvec4(next_data.x, cur_data.yzw));
			}
			ptr = cur_data.x;
		}
		end = ptr;
	}
	
	// calculate final color
	uint next_ptr;
	for (uint sorted_ptr = imageLoad(head_ptr_image, ivec2(gl_FragCoord.xy)).x; sorted_ptr != 0xFFFFFFFF; sorted_ptr = next_ptr) {
		uvec4 cur_data = imageLoad(list_buffer, int(sorted_ptr));
		next_ptr = cur_data.x;
		vec4 curFragColor = unpackUnorm4x8(cur_data.y);
		float lightness = unpackHalf2x16((cur_data.w >> 8) & 0xFFFF).x;
		curFragColor = vec4(curFragColor.rgb * lightness, curFragColor.a);
		uint blend_flag = cur_data.w & 0xFF;
		switch(blend_flag) {
			case 0:
				fragColor = alphaBlend(curFragColor, fragColor);
				break;
			case 1:
				fragColor = additiveBlend(curFragColor, fragColor);
				break;
			case 2:
				fragColor = multiplyBlend(curFragColor, fragColor);
				break;
			default:
				fragColor = alphaBlend(curFragColor, fragColor);
				break;
		}
	}
}