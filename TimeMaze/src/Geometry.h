#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "GLLib.h"

class GeometryCreator final {
public:
	static GLLib::VertexElementArray createCube(float length = 2.0f, float width = 2.0f, float height = 2.0f, float texScale = 1.0f, GLenum frontFace = GL_CCW);
	static GLLib::VertexElementArray createSphere(int horizontalSegment = 64, int verticalSegment = 32, GLenum frontFace = GL_CCW);
	static GLLib::VertexElementArray createRing(int segment1 = 64, int segment2 = 32, float radius1 = 2.0f, float radius2 = 0.5f, GLenum frontFace = GL_CCW);
	static GLLib::VertexElementArray createCone(int segment = 64, GLenum frontFace = GL_CCW);
	static GLLib::VertexElementArray createPanel(float width = 2.0f, float height = 2.0f, float texScale = 1.0f, GLenum frontFace = GL_CCW);
	static GLLib::VertexElementArray createCylinder(int segment = 64, GLenum frontFace = GL_CCW);
	static GLLib::VertexArray createFromOBJFile(const char* filename);
};

#endif