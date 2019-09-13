#version 420 core

uniform bool useNormalmap;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vsNormal[];
in vec3 vsPos[];
in vec2 texCoord[];

out vec3 fTangent;
out vec3 fBitangent;
out vec3 fvsNormal;
out vec3 fvsPos;
out vec2 ftexCoord;

void main() {
	if (useNormalmap) {
		vec3 q1 = vsPos[1] - vsPos[0];
		vec3 q2 = vsPos[2] - vsPos[0];
		mat2 m = mat2(texCoord[1] - texCoord[0], texCoord[2]- texCoord[0]);
		mat2x3 mtb = mat2x3(q1, q2) * inverse(m);

		fTangent = mtb[0] - dot(vsNormal[0], mtb[0]) / dot(vsNormal[0], vsNormal[0]) * vsNormal[0];
		fBitangent = mtb[1] - dot(vsNormal[0], mtb[1]) / dot(vsNormal[0], vsNormal[0]) * vsNormal[0] - dot(fTangent, mtb[1]) / dot(fTangent, fTangent) * fTangent;
		fvsNormal = vsNormal[0];
		fvsPos = vsPos[0];
		ftexCoord = texCoord[0];
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		
		fTangent = mtb[0] - dot(vsNormal[1], mtb[0]) / dot(vsNormal[1], vsNormal[1]) * vsNormal[1];
		fBitangent = mtb[1] - dot(vsNormal[1], mtb[1]) / dot(vsNormal[1], vsNormal[1]) * vsNormal[1] - dot(fTangent, mtb[1]) / dot(fTangent, fTangent) * fTangent;
		fvsNormal = vsNormal[1];
		fvsPos = vsPos[1];
		ftexCoord = texCoord[1];
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();
		
		fTangent = mtb[0] - dot(vsNormal[2], mtb[0]) / dot(vsNormal[2], vsNormal[2]) * vsNormal[2];
		fBitangent = mtb[1] - dot(vsNormal[2], mtb[1]) / dot(vsNormal[2], vsNormal[2]) * vsNormal[2] - dot(fTangent, mtb[1]) / dot(fTangent, fTangent) * fTangent;
		fvsNormal = vsNormal[2];
		fvsPos = vsPos[2];
		ftexCoord = texCoord[2];
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();
		
		EndPrimitive();
	} else {
		fTangent = vec3(0.0);
		fBitangent = vec3(0.0);
		fvsNormal = vsNormal[0];
		fvsPos = vsPos[0];
		ftexCoord = texCoord[0];
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		fvsNormal = vsNormal[1];
		fvsPos = vsPos[1];
		ftexCoord = texCoord[1];
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();
		fvsNormal = vsNormal[2];
		fvsPos = vsPos[2];
		ftexCoord = texCoord[2];
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();
		EndPrimitive();
	}
}