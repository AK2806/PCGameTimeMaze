#version 420 core

uniform vec3 wsLightPos[16];
uniform vec3 lightColor[16];
uniform float lightStrength[16];
uniform bool useLight[16];

uniform vec3 ambientLightColor;

uniform vec3 materialColor;
uniform float shininess;

uniform bool texColor_inverse;
uniform uint texChannel_link;

uniform bool lighting;

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
layout (binding = 20) uniform sampler3D aniSampler;
uniform bool useAnimap;

uniform mat4 view;
uniform samplerCubeShadow cubeShadowSampler[16];
uniform float shadowFar;
uniform float shadowNear;

in vec3 fTangent;
in vec3 fBitangent;
in vec3 fvsNormal;
in vec3 fvsPos;
in vec2 ftexCoord;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out float fragDepth;

const float sampleStep = 0.005f;
const int sampleSideCount = 5;

// Cook-Torrance Light Model



// Shadow Mapping

float VectorToDepth(vec3 vector)
{
    vec3 AbsVec = abs(vector);
    float LocalZComp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));
    float NormZComp = (shadowFar + shadowNear) / (shadowFar - shadowNear) - (2 * shadowFar * shadowNear) / (shadowFar - shadowNear) / LocalZComp;
    return (NormZComp + 1.0) * 0.5;
}

void main()
{
	fragDepth = gl_FragCoord.z;
	vec3 fvsNormalN = normalize(fvsNormal);
	if (useNormalmap) {
		mat3 mTBN = mat3(normalize(fTangent), normalize(fBitangent), fvsNormalN);
		fvsNormalN = normalize(mTBN * (2 * texture(normalSampler, ftexCoord).xyz - vec3(1.0)));
	}
	vec4 baseColor = vec4(materialColor, 1.0);
	if (useColormap) {
		vec4 texColor = texture(colorSampler, ftexCoord);
		vec4 transColor = vec4(0.0);
		transColor.r = texColor[texChannel_link >> 6];
		transColor.g = texColor[(texChannel_link >> 4) & 3];
		transColor.b = texColor[(texChannel_link >> 2) & 3];
		transColor.a = texColor[texChannel_link & 3];
		if (texColor_inverse) {
			transColor = vec4(1.0) - transColor;
		}
		baseColor *= transColor;
	}
	if (useEnvmap) {
		vec4 texColor = texture(envSampler, reflect(fvsPos, fvsNormalN));
		vec4 transColor = vec4(0.0);
		transColor.r = texColor[texChannel_link >> 6];
		transColor.g = texColor[(texChannel_link >> 4) & 3];
		transColor.b = texColor[(texChannel_link >> 2) & 3];
		transColor.a = texColor[texChannel_link & 3];
		if (texColor_inverse) {
			transColor = vec4(1.0) - transColor;
		}
		baseColor *= transColor;
	}
	if (useAnimap) {
		vec4 texColor = texture(aniSampler, vec3(ftexCoord, time / animationDuring));
		vec4 transColor = vec4(0.0);
		transColor.r = texColor[texChannel_link >> 6];
		transColor.g = texColor[(texChannel_link >> 4) & 3];
		transColor.b = texColor[(texChannel_link >> 2) & 3];
		transColor.a = texColor[texChannel_link & 3];
		if (texColor_inverse) {
			transColor = vec4(1.0) - transColor;
		}
		baseColor *= transColor;
	}
	if (lighting) {
		fragColor = baseColor * vec4(ambientLightColor, 1.0);
		for (int i = 0; i < wsLightPos.length(); ++i) {
			if (!useLight[i]) continue;
			vec4 v4vsLightPos = view * vec4(wsLightPos[i], 1.0);
			vec3 vsLightDirN = fvsPos - v4vsLightPos.xyz / v4vsLightPos.w;
			float damping = lightStrength[i] / length(vsLightDirN); // it's 1.0 / dis^2 in real, here is a approximate solution
			vsLightDirN = normalize(vsLightDirN);
			float diffusePercent = max(0.0, dot(-vsLightDirN, fvsNormalN));
			vec3 reflectLightDirN = reflect(vsLightDirN, fvsNormalN);
			float specularPercent = pow(max(0.0, dot(normalize(-fvsPos), reflectLightDirN)), shininess);
			if (useSpecmap) {
				specularPercent *= texture(specSampler, ftexCoord).r;
			}
			vec4 v4fwsPos = inverse(view) * vec4(fvsPos, 1.0);
			vec3 lsPos = v4fwsPos.xyz / v4fwsPos.w - wsLightPos[i];
			float shadow = texture(cubeShadowSampler[i], vec4(lsPos, VectorToDepth(lsPos)));
			fragColor += baseColor * shadow * damping * (diffusePercent + specularPercent) * vec4(lightColor[i], 1.0);
		}
	} else {
		fragColor = baseColor;
	}
}