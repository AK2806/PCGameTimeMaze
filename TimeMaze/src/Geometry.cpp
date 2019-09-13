#include "Geometry.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace GLLib;
using namespace glm;
using namespace std;
using namespace tinyobj;

VertexElementArray GeometryCreator::createCube(float length, float width, float height, float texScale, GLenum frontFace)
{
	vec4 posArr[] = {
		// top
		{ length / 2.0f, height / 2.0f, width / 2.0f, 1 },
		{ length / 2.0f, height / 2.0f, -width / 2.0f, 1 },
		{ -length / 2.0f, height / 2.0f, -width / 2.0f, 1 },
		{ -length / 2.0f, height / 2.0f, width / 2.0f, 1 },

		// bottom
		{ length / 2.0f, -height / 2.0f, width / 2.0f, 1 },
		{ length / 2.0f, -height / 2.0f, -width / 2.0f, 1 },
		{ -length / 2.0f, -height / 2.0f, -width / 2.0f, 1 },
		{ -length / 2.0f, -height / 2.0f, width / 2.0f, 1 },

		// left
		{ -length / 2.0f, height / 2.0f, -width / 2.0f, 1 },
		{ -length / 2.0f, height / 2.0f, width / 2.0f, 1 },
		{ -length / 2.0f, -height / 2.0f, width / 2.0f, 1 },
		{ -length / 2.0f, -height / 2.0f, -width / 2.0f, 1 },

		// right
		{ length / 2.0f, height / 2.0f, -width / 2.0f, 1 },
		{ length / 2.0f, height / 2.0f, width / 2.0f, 1 },
		{ length / 2.0f, -height / 2.0f, width / 2.0f, 1 },
		{ length / 2.0f, -height / 2.0f, -width / 2.0f, 1 },

		// front
		{ -length / 2.0f, -height / 2.0f, width / 2.0f, 1 },
		{ length / 2.0f, -height / 2.0f, width / 2.0f, 1 },
		{ length / 2.0f, height / 2.0f, width / 2.0f, 1 },
		{ -length / 2.0f, height / 2.0f, width / 2.0f, 1 },

		// back
		{ -length / 2.0f, -height / 2.0f, -width / 2.0f, 1 },
		{ length / 2.0f, -height / 2.0f, -width / 2.0f, 1 },
		{ length / 2.0f, height / 2.0f, -width / 2.0f, 1 },
		{ -length / 2.0f, height / 2.0f, -width / 2.0f, 1 }
	};

	vec4 normalArr[] = {
		// top
		{ 0, 1, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 1, 0, 0 },

		// bottom
		{ 0, -1, 0, 0 },
		{ 0, -1, 0, 0 },
		{ 0, -1, 0, 0 },
		{ 0, -1, 0, 0 },

		// left
		{ -1, 0, 0, 0 },
		{ -1, 0, 0, 0 },
		{ -1, 0, 0, 0 },
		{ -1, 0, 0, 0 },

		// right
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 0 },

		// front
		{ 0, 0, 1, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 1, 0 },

		// back
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 },
		{ 0, 0, -1, 0 }

	};

	vec2 texCoordArr[] = {
		// top
		{ length / texScale, 0 },
		{ length / texScale, width / texScale },
		{ 0, width / texScale },
		{ 0, 0 },

		// bottom
		{ length / texScale, 0 },
		{ length / texScale, width / texScale },
		{ 0, width / texScale },
		{ 0, 0 },

		// left
		{ 0, 0 },
		{ width / texScale, 0 },
		{ width / texScale, height / texScale },
		{ 0, height / texScale },

		// right
		{ 0, 0 },
		{ width / texScale, 0 },
		{ width / texScale, height / texScale },
		{ 0, height / texScale },

		// front
		{ 0, height / texScale },
		{ length / texScale, height / texScale },
		{ length / texScale, 0 },
		{ 0, 0 },

		// back
		{ 0, height / texScale },
		{ length / texScale, height / texScale },
		{ length / texScale, 0 },
		{ 0, 0 }

	};

	VertexArrayFactory geometryFactory;
	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, sizeof(posArr), 0));
	geometryFactory.pushAttribute(AttrDef(2, AttrDef::TWO, GL_FLOAT, sizeof(posArr) + sizeof(normalArr), 0));
	geometryFactory.allocData(sizeof(posArr) + sizeof(normalArr) + sizeof(texCoordArr));
	geometryFactory.setData(posArr, 0, sizeof(posArr));
	geometryFactory.setData(normalArr, sizeof(posArr), sizeof(normalArr));
	geometryFactory.setData(texCoordArr, sizeof(posArr) + sizeof(normalArr), sizeof(texCoordArr));

	if (frontFace == GL_CCW)
	{
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0,

			6, 5, 4,
			4, 7, 6,

			10, 9, 8,
			8, 11, 10,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			22, 21, 20,
			20, 23, 22
		};
		return geometryFactory.createVAO(indices, sizeof(indices) / sizeof(unsigned int));
	}
	else
	{
		unsigned int indices[] = {
			2, 1, 0,
			0, 3, 2,

			4, 5, 6,
			6, 7, 4,

			8, 9, 10,
			10, 11, 8,

			14, 13, 12,
			12, 15, 14,

			18, 17, 16,
			16, 19, 18,

			20, 21, 22,
			22, 23, 20
		};
		return geometryFactory.createVAO(indices, sizeof(indices) / sizeof(unsigned int));
	}
}

VertexElementArray GeometryCreator::createSphere(int horizontalSegment, int verticalSegment, GLenum frontFace)
{
	const float PI = 3.1415926535898f;
	float vStep = PI / verticalSegment;
	float hStep = 2 * PI / horizontalSegment;
	float texXStep = 1.0f / horizontalSegment;
	float texYStep = 1.0f / verticalSegment;

	vector<vec4> posArr;
	vector<vec4> normalArr;
	vector<vec2> texcoordArr;
	posArr.reserve((verticalSegment + 1) * (horizontalSegment + 1));
	normalArr.reserve((verticalSegment + 1) * (horizontalSegment + 1));
	texcoordArr.reserve((verticalSegment + 1) * (horizontalSegment + 1));

	for (int i = 0; i <= verticalSegment; ++i)
	{
		for (int j = 0; j <= horizontalSegment; ++j)
		{
			float vAngle = i * vStep;
			float hAngle = j * hStep;
			vec4 pos(cosf(PI * 0.5f - vAngle) * cosf(hAngle), sinf(PI * 0.5f - vAngle), cosf(PI * 0.5f - vAngle) * -sinf(hAngle), 1.0f);
			posArr.push_back(pos);
			normalArr.push_back(pos);
			texcoordArr.push_back(vec2(j * texXStep, i * texYStep));
		}
	}

	vector<unsigned int> indices;
	indices.reserve(verticalSegment * horizontalSegment * 6);
	if (frontFace == GL_CCW)
	{
		for (int i = 0; i <= verticalSegment - 1; ++i)
		{
			for (int j = 0; j <= horizontalSegment - 1; ++j)
			{
				indices.push_back(i * (horizontalSegment + 1) + j);
				indices.push_back((i + 1) * (horizontalSegment + 1) + j);
				indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);

				indices.push_back(i * (horizontalSegment + 1) + j);
				indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
				indices.push_back(i * (horizontalSegment + 1) + j + 1);
			}
		}
	}
	else
	{
		for (int i = 0; i <= verticalSegment - 1; ++i)
		{
			for (int j = 0; j <= horizontalSegment - 1; ++j)
			{
				indices.push_back(i * (horizontalSegment + 1) + j);
				indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
				indices.push_back((i + 1) * (horizontalSegment + 1) + j);

				indices.push_back(i * (horizontalSegment + 1) + j);
				indices.push_back(i * (horizontalSegment + 1) + j + 1);
				indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
			}
		}
	}

	VertexArrayFactory geometryFactory;
	size_t posSize = posArr.size() * sizeof(vec4);
	size_t normalSize = normalArr.size() * sizeof(vec4);
	size_t texcoordSize = texcoordArr.size() * sizeof(vec2);

	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, posSize, 0));
	geometryFactory.pushAttribute(AttrDef(2, AttrDef::TWO, GL_FLOAT, posSize + normalSize, 0));
	geometryFactory.allocData(posSize + normalSize + texcoordSize);
	geometryFactory.setData(posArr.data(), 0, posSize);
	geometryFactory.setData(normalArr.data(), posSize, normalSize);
	geometryFactory.setData(texcoordArr.data(), posSize + normalSize, texcoordSize);

	return geometryFactory.createVAO(indices.data(), indices.size());
}

VertexElementArray GeometryCreator::createRing(int segment1, int segment2, float radius1, float radius2, GLenum frontFace)
{
	const float PI = 3.1415926535898f;
	vector<vec4> posArr;
	vector<vec4> normalArr;
	posArr.reserve((segment1 + 1) * (segment2 + 1));
	normalArr.reserve((segment1 + 1) * (segment2 + 1));

	float step1 = 2 * PI / segment1;
	float step2 = 2 * PI / segment2;

	for (int i = 0; i <= segment1; ++i)
	{
		for (int j = 0; j <= segment2; ++j)
		{
			float angle1 = i * step1;
			float angle2 = j * step2;
			vec3 dir_vec = vec3(radius1 * cosf(angle1), 0, -radius1 * sinf(angle1));
			vec3 lit_vec = normalize(dir_vec) * radius2 * cosf(angle2) + vec3(0, radius2 * sinf(angle2), 0);
			posArr.push_back(vec4(dir_vec + lit_vec, 1.0f));
			normalArr.push_back(vec4(lit_vec, 0.0f));
		}
	}

	vector<unsigned int> indices;
	indices.reserve(segment1 * segment2 * 6);
	if (frontFace == GL_CCW)
	{
		for (int i = 0; i <= segment1 - 1; ++i)
		{
			for (int j = 0; j <= segment2 - 1; ++j)
			{
				indices.push_back(i * (segment2 + 1) + j);
				indices.push_back((i + 1) * (segment2 + 1) + j);
				indices.push_back(i * (segment2 + 1) + j + 1);

				indices.push_back(i * (segment2 + 1) + j + 1);
				indices.push_back((i + 1) * (segment2 + 1) + j);
				indices.push_back((i + 1) * (segment2 + 1) + j + 1);
			}
		}
	}
	else
	{
		for (int i = 0; i <= segment1 - 1; ++i)
		{
			for (int j = 0; j <= segment2 - 1; ++j)
			{
				indices.push_back(i * (segment2 + 1) + j);
				indices.push_back(i * (segment2 + 1) + j + 1);
				indices.push_back((i + 1) * (segment2 + 1) + j);

				indices.push_back(i * (segment2 + 1) + j + 1);
				indices.push_back((i + 1) * (segment2 + 1) + j + 1);
				indices.push_back((i + 1) * (segment2 + 1) + j);
			}
		}
	}

	VertexArrayFactory geometryFactory;
	size_t posSize = posArr.size() * sizeof(vec4);
	size_t normalSize = normalArr.size() * sizeof(vec4);

	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, posSize, 0));
	geometryFactory.allocData(posSize + normalSize);
	geometryFactory.setData(posArr.data(), 0, posSize);
	geometryFactory.setData(normalArr.data(), posSize, normalSize);

	return geometryFactory.createVAO(indices.data(), indices.size());
}

VertexElementArray GeometryCreator::createCone(int segment, GLenum frontFace)
{
	const float PI = 3.1415926535898f;
	float step = 2 * PI / segment;

	vector<vec4> posArr;
	vector<vec4> normalArr;
	posArr.reserve(4 * segment + 2);
	normalArr.reserve(4 * segment + 2);

	for (int i = 1; i <= segment; ++i)
	{
		float angle = (i - 0.5f) * step;
		posArr.push_back(vec4(0, 1, 0, 1));
		normalArr.push_back(vec4(normalize(vec3(cosf(angle), 1, -sinf(angle))), 0));
	}
	for (int i = 0; i <= segment; ++i)
	{
		float angle = i * step;
		vec4 pos(cosf(angle), 0, -sinf(angle), 1);
		posArr.push_back(pos);
		normalArr.push_back(vec4(normalize(pos.xyz + vec3(0, 1, 0)), 0));
	}
	for (int i = 1; i <= segment; ++i)
	{
		posArr.push_back(vec4(0, 0, 0, 1));
		normalArr.push_back(vec4(0, -1, 0, 0));
	}
	for (int i = 0; i <= segment; ++i)
	{
		float angle = i * step;
		posArr.push_back(vec4(cosf(angle), 0, -sinf(angle), 1));
		normalArr.push_back(vec4(0, -1, 0, 0));
	}

	vector<unsigned int> indices;
	indices.reserve(segment * 6);
	if (frontFace == GL_CCW)
	{
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(i);
			indices.push_back(segment + i);
			indices.push_back(segment + i + 1);
		}
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(2 * segment + i + 1);
			indices.push_back(3 * segment + i + 2);
			indices.push_back(3 * segment + i + 1);
		}
	}
	else
	{
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(i);
			indices.push_back(segment + i + 1);
			indices.push_back(segment + i);
		}
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(2 * segment + i + 1);
			indices.push_back(3 * segment + i + 1);
			indices.push_back(3 * segment + i + 2);
		}
	}

	VertexArrayFactory geometryFactory;
	size_t posSize = posArr.size() * sizeof(vec4);
	size_t normalSize = normalArr.size() * sizeof(vec4);

	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, posSize, 0));
	geometryFactory.allocData(posSize + normalSize);
	geometryFactory.setData(posArr.data(), 0, posSize);
	geometryFactory.setData(normalArr.data(), posSize, normalSize);

	return geometryFactory.createVAO(indices.data(), indices.size());
}

VertexElementArray GeometryCreator::createPanel(float width, float height, float texScale, GLenum frontFace)
{
	vec4 posArr[] = {
		{ -width / 2.0f, -height / 2.0f, 0, 1 },
		{ width / 2.0f, -height / 2.0f, 0, 1 },
		{ width / 2.0f, height / 2.0f, 0, 1 },
		{ -width / 2.0f, height / 2.0f, 0 ,1 }
	};
	vec4 normalArr[] = {
		{ 0, 0, 1, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 1, 0 }
	};
	vec2 texCoordArr[] = {
		{ 0, height / texScale },
		{ width / texScale, height / texScale },
		{ width / texScale, 0 },
		{ 0, 0 }
	};

	VertexArrayFactory geometryFactory;
	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, sizeof(posArr), 0));
	geometryFactory.pushAttribute(AttrDef(2, AttrDef::TWO, GL_FLOAT, sizeof(posArr) + sizeof(normalArr), 0));
	geometryFactory.allocData(sizeof(posArr) + sizeof(normalArr) + sizeof(texCoordArr));
	geometryFactory.setData(posArr, 0, sizeof(posArr));
	geometryFactory.setData(normalArr, sizeof(posArr), sizeof(normalArr));
	geometryFactory.setData(texCoordArr, sizeof(posArr) + sizeof(normalArr), sizeof(texCoordArr));

	if (frontFace == GL_CCW)
	{
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		return geometryFactory.createVAO(indices, sizeof(indices) / sizeof(unsigned int));
	}
	else
	{
		unsigned int indices[] = {
			0, 3, 2,
			2, 1, 0
		};
		return geometryFactory.createVAO(indices, sizeof(indices) / sizeof(unsigned int));
	}
}

VertexElementArray GeometryCreator::createCylinder(int segment, GLenum frontFace) {
	const float PI = 3.1415926535898f;
	float angleStep = (float)(2 * PI / segment);
	float texcoordStep = 1.0f / segment;

	vector<vec4> posArr;
	vector<vec4> normalArr;
	vector<vec2> texcoordArr;
	posArr.reserve(4 * (segment + 1) + 2);
	normalArr.reserve(4 * (segment + 1) + 2);
	texcoordArr.reserve(4 * (segment + 1) + 2);

	// top
	{
		posArr.push_back(vec4(0, 1, 0, 1));
		normalArr.push_back(vec4(0, 1, 0, 0));
		texcoordArr.push_back(vec2(0, 0));
	}
	for (int i = 0; i <= segment; ++i)
	{
		float angle = i * angleStep;
		posArr.push_back(vec4(cosf(angle), 1, -sinf(angle), 1));
		normalArr.push_back(vec4(0, 1, 0, 0));
		texcoordArr.push_back(vec2(i * texcoordStep, 0));
	}
	// side
	for (int i = 0; i <= segment; ++i)
	{
		float angle = i * angleStep;
		posArr.push_back(vec4(cosf(angle), 1, -sinf(angle), 1));
		normalArr.push_back(vec4(normalize(vec3(cosf(angle), 0, -sinf(angle))), 0));
		texcoordArr.push_back(vec2(i * texcoordStep, 0));
	}
	for (int i = 0; i <= segment; ++i)
	{
		float angle = i * angleStep;
		posArr.push_back(vec4(cosf(angle), 0, -sinf(angle), 1));
		normalArr.push_back(vec4(normalize(vec3((float)cos(angle), 0, -(float)sin(angle))), 0));
		texcoordArr.push_back(vec2(i * texcoordStep, 1.0f));
	}
	// bottom
	for (int i = 0; i <= segment; ++i)
	{
		float angle = i * angleStep;
		posArr.push_back(vec4(cosf(angle), 0, -sinf(angle), 1));
		normalArr.push_back(vec4(0, -1, 0, 0));
		texcoordArr.push_back(vec2(i * texcoordStep, 1.0f));
	}
	{
		posArr.push_back(vec4(0, 0, 0, 1));
		normalArr.push_back(vec4(0, -1, 0, 0));
		texcoordArr.push_back(vec2(1.0f, 1.0f));
	}

	vector<unsigned int> indices;
	indices.reserve(segment * 6);
	if (frontFace == GL_CCW)
	{
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(0);
			indices.push_back(i + 1);
			indices.push_back(i + 2);
		}
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(segment + 2 + i);
			indices.push_back(2 * (segment + 1) + 1 + i);
			indices.push_back(segment + 2 + i + 1);
			
			indices.push_back(segment + 2 + i + 1);
			indices.push_back(2 * (segment + 1) + 1 + i);
			indices.push_back(2 * (segment + 1) + 1 + i + 1);
		}
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(4 * (segment + 1) + 1);
			indices.push_back(3 * (segment + 1) + 1 + i + 1);
			indices.push_back(3 * (segment + 1) + 1 + i);
		}
	}
	else
	{
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(0);
			indices.push_back(i + 2);
			indices.push_back(i + 1);
		}
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(segment + 2 + i);
			indices.push_back(segment + 2 + i + 1);
			indices.push_back(2 * (segment + 1) + 1 + i);

			indices.push_back(segment + 2 + i + 1);
			indices.push_back(2 * (segment + 1) + 1 + i + 1);
			indices.push_back(2 * (segment + 1) + 1 + i);
		}
		for (int i = 0; i <= segment - 1; ++i)
		{
			indices.push_back(4 * (segment + 1) + 1);
			indices.push_back(3 * (segment + 1) + 1 + i);
			indices.push_back(3 * (segment + 1) + 1 + i + 1);
		}
	}

	VertexArrayFactory geometryFactory;
	size_t posSize = posArr.size() * sizeof(vec4);
	size_t normalSize = normalArr.size() * sizeof(vec4);
	size_t texcoordSize = texcoordArr.size() * sizeof(vec2);

	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, posSize, 0));
	geometryFactory.pushAttribute(AttrDef(2, AttrDef::TWO, GL_FLOAT, posSize + normalSize, 0));
	geometryFactory.allocData(posSize + normalSize + texcoordSize);
	geometryFactory.setData(posArr.data(), 0, posSize);
	geometryFactory.setData(normalArr.data(), posSize, normalSize);
	geometryFactory.setData(texcoordArr.data(), posSize + normalSize, texcoordSize);

	return geometryFactory.createVAO(indices.data(), indices.size());
}

VertexArray GeometryCreator::createFromOBJFile(const char* filename)
{
	attrib_t attrib;
	vector<shape_t> shapes;
	vector<material_t> materials;
	string err;

	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);
	
	if (!err.empty()) { // `err` may contain warning message.
		cerr << err << endl;
	}

	if (!success) {
		return VertexArray();
	}

	vector<vec4> posArr;
	vector<vec4> normalArr;
	vector<vec2> texcoordArr;

	// Loop over shapes
	size_t shapeCount = shapes.size();
	for (size_t i = 0; i < shapeCount; ++i) {
		// Loop over indices
		size_t indicesCount = shapes[i].mesh.indices.size();
		for (size_t j = 0; j < indicesCount; ++j) {
			// access to vertex
			index_t idx = shapes[i].mesh.indices[j];
			float vx = attrib.vertices[3 * idx.vertex_index + 0];
			float vy = attrib.vertices[3 * idx.vertex_index + 1];
			float vz = attrib.vertices[3 * idx.vertex_index + 2];
			posArr.push_back(vec4(vx, vy, vz, 1));
			if (idx.normal_index != -1) {
				float nx = attrib.normals[3 * idx.normal_index + 0];
				float ny = attrib.normals[3 * idx.normal_index + 1];
				float nz = attrib.normals[3 * idx.normal_index + 2];
				normalArr.push_back(vec4(nx, ny, nz, 0));
			}
			if (idx.texcoord_index != -1) {
				float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				texcoordArr.push_back(vec2(tx, ty));
			}
		}
	}

	VertexArrayFactory geometryFactory;
	size_t posSize = posArr.size() * sizeof(vec4);
	size_t normalSize = normalArr.size() * sizeof(vec4);
	size_t texcoordSize = texcoordArr.size() * sizeof(vec2);

	geometryFactory.pushAttribute(AttrDef(0, AttrDef::FOUR, GL_FLOAT, 0, 0));
	size_t nextOffset = posSize;
	if (normalSize > 0) {
		geometryFactory.pushAttribute(AttrDef(1, AttrDef::FOUR, GL_FLOAT, nextOffset, 0));
		nextOffset += normalSize;
	}
	if (texcoordSize > 0) {
		geometryFactory.pushAttribute(AttrDef(2, AttrDef::TWO, GL_FLOAT, nextOffset, 0));
		nextOffset += texcoordSize;
	}
	geometryFactory.allocData(nextOffset);
	geometryFactory.setData(posArr.data(), 0, posSize);
	if (normalSize > 0) geometryFactory.setData(normalArr.data(), posSize, normalSize);
	if (texcoordSize > 0) geometryFactory.setData(texcoordArr.data(), posSize + normalSize, texcoordSize);

	return geometryFactory.createVAO(posArr.size());
}
