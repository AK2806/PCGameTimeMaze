#include "GLLib.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

inline static GLsizei sizeofGLType(GLenum elementType)
{
	switch (elementType)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
		return 1;
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_HALF_FLOAT:
		return 2;
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_FLOAT:
	case GL_FIXED:
	case GL_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_10F_11F_11F_REV:
		return 4;
	case GL_DOUBLE:
		return 8;
	default:
		return 0;
	}
}

GLLib::RenderProcess::RenderProcess(const shared_ptr<Camera>& camera) :
	m_objects(), m_pipeline(nullptr), m_frameBuffer(GLFramebuffer::getScreen()), m_camera(camera), m_useMVP(camera.get() ? true : false)
{
}

GLLib::RenderProcess::RenderProcess(const shared_ptr<Camera>& camera, const shared_ptr<GLShaderProgram>& pipeline) :
	m_objects(), m_pipeline(pipeline), m_frameBuffer(GLFramebuffer::getScreen()), m_camera(camera), m_useMVP(camera.get() ? true : false)
{
}

GLLib::RenderProcess::~RenderProcess()
{
}

void GLLib::RenderProcess::addObject(const shared_ptr<GLVertexArray>& object)
{
	this->m_objects.push_back(object);
}

void GLLib::RenderProcess::removeObject(const shared_ptr<GLVertexArray>& object)
{
	for (auto iterator = this->m_objects.begin(); iterator != this->m_objects.end(); ++iterator) {
		if (iterator->get() == object.get()) {
			this->m_objects.erase(iterator);
			return;
		}
	}
}

void GLLib::RenderProcess::clearObjects()
{
	this->m_objects.clear();
}

void GLLib::RenderProcess::useMVP(bool enabled)
{
	this->m_useMVP = enabled;
}

void GLLib::RenderProcess::render()
{
	if (!this->m_pipeline.get() || !this->m_frameBuffer.get()) return;

	if (this->m_useMVP && !this->m_camera.get()) return;

	this->m_pipeline->use();
	this->m_frameBuffer->bind();

	if (this->m_useMVP) {
		mat4 viewMatrix = this->m_camera->getViewMatrix();
		mat4 projectionMatrix = this->m_camera->getProjectionMatrix();
		glUniformMatrix4fv(this->m_viewMatLoc, 1, false, value_ptr(viewMatrix));
		glUniformMatrix4fv(this->m_projMatLoc, 1, false, value_ptr(projectionMatrix));
	}
	
	for (shared_ptr<GLVertexArray> vao : this->m_objects) {
		if (this->m_useMVP) {
			mat4 modelMatrix = vao->getModelMatrix();
			glUniformMatrix4fv(this->m_modelMatLoc, 1, false, value_ptr(modelMatrix));
		}

		GLVertexArray::DrawMode drawMode = vao->getDrawMode();
		if (drawMode == GLVertexArray::Array)
		{
			vao->bind();
			glDrawArrays(vao->getPrimitive(), vao->getBeginPos(), vao->getDrawCount());
		}
		else if (drawMode == GLVertexArray::Element)
		{
			vao->bind();
			glDrawElements(vao->getPrimitive(), vao->getDrawCount(), vao->getElementType(), (GLvoid *)(vao->getBeginPos() * sizeofGLType(vao->getElementType())));
		}
	}
}

GLLib::GLShaderProgram::GLShaderProgram()
{
	this->m_ptr = glCreateProgram();
}

GLLib::GLShaderProgram::~GLShaderProgram()
{
	glDeleteProgram(this->m_ptr);
}

void GLLib::GLShaderProgram::attachShader(const GLShader & shader)
{
	glAttachShader(this->m_ptr, shader.m_ptr);
}

bool GLLib::GLShaderProgram::build()
{
	glLinkProgram(this->m_ptr);
	GLint linked;
	glGetProgramiv(this->m_ptr, GL_LINK_STATUS, &linked);
	return linked;
}

bool GLLib::GLShaderProgram::build(std::string & info)
{
	glLinkProgram(this->m_ptr);
	GLint linked;
	glGetProgramiv(this->m_ptr, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(this->m_ptr, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len + 1];
		glGetProgramInfoLog(this->m_ptr, len, &len, log);
		info = log;
		delete[] log;
		return false;
	}
	else
	{
		info = "";
		return true;
	}
}

void GLLib::GLShaderProgram::use()
{
	glUseProgram(this->m_ptr);
}

GLint GLLib::GLShaderProgram::getUniformLocation(const std::string & name)
{
	return glGetUniformLocation(this->m_ptr, name.c_str());
}

GLLib::GLShader::GLShader(GLenum shaderType) :
	m_src("")
{
	this->m_ptr = glCreateShader(shaderType);
}

GLLib::GLShader::~GLShader()
{
	glDeleteShader(this->m_ptr);
}

void GLLib::GLShader::setSrc(const std::string & src)
{
	this->m_src = src;
}

bool GLLib::GLShader::loadFile(const std::string & filename)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	if (!fp) return false;
	fseek(fp, 0, SEEK_END);
	size_t length = ftell(fp);
	char* code = new char[length + 1];
	rewind(fp);
	fread(code, 1, length, fp);
	code[length] = '\0';
	fclose(fp);
	this->m_src = code;
	delete[] code;
	return true;
}

bool GLLib::GLShader::compile()
{
	const char* code = this->m_src.c_str();
	glShaderSource(this->m_ptr, 1, &code, nullptr);
	glCompileShader(this->m_ptr);
	GLint compiled;
	glGetShaderiv(this->m_ptr, GL_COMPILE_STATUS, &compiled);
	return compiled;
}

bool GLLib::GLShader::compile(std::string & info)
{
	const char* code = this->m_src.c_str();
	glShaderSource(this->m_ptr, 1, &code, nullptr);
	glCompileShader(this->m_ptr);

	GLint compiled;
	glGetShaderiv(this->m_ptr, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLsizei len;
		glGetShaderiv(this->m_ptr, GL_INFO_LOG_LENGTH, &len);
		char* log = new char[len + 1];
		glGetShaderInfoLog(this->m_ptr, len, &len, log);
		info += log;
		delete[] log;
		return false;
	}
	else
	{
		info = "";
		return true;
	}
}

GLLib::GLVertexArray::GLVertexArray() :
	m_drawMode(Array), m_primitive(GL_TRIANGLES), m_startIndex(0), m_vertexCount(0), m_elementType(GL_UNSIGNED_INT),
	m_position(0.0f), m_rotation(1.0f, 0.0f, 0.0f, 0.0f), m_scale(1.0f), m_origin(0.0f)
{
	glGenVertexArrays(1, &this->m_ptr);
	this->__updateMatrix();
}

GLLib::GLVertexArray::~GLVertexArray()
{
	glDeleteVertexArrays(1, &this->m_ptr);
}

void GLLib::GLVertexArray::bind()
{
	glBindVertexArray(this->m_ptr);
}

void GLLib::GLVertexArray::translate(const vec3& dis)
{
	this->m_position += dis;
	this->__updateMatrix();
}

void GLLib::GLVertexArray::translate(float x, float y, float z)
{
	this->translate(vec3(x, y, z));
}

void GLLib::GLVertexArray::rotate(float angle, const vec3& axis)
{
	this->m_rotation = normalize(angleAxis(angle, axis) * this->m_rotation);
	this->__updateMatrix();
}

void GLLib::GLVertexArray::rotate(float angle, float x, float y, float z)
{
	this->rotate(angle, vec3(x, y, z));
}

void GLLib::GLVertexArray::setPosition(const vec3& pos)
{
	this->m_position = pos;
	this->__updateMatrix();
}

void GLLib::GLVertexArray::setPosition(float x, float y, float z)
{
	this->setPosition(vec3(x, y, z));
}

void GLLib::GLVertexArray::setRotation(const quat& rotation)
{
	this->m_rotation = rotation;
	this->__updateMatrix();
}

void GLLib::GLVertexArray::setScale(const vec3& scale)
{
	this->m_scale = scale;
	this->__updateMatrix();
}

void GLLib::GLVertexArray::setScale(float x, float y, float z)
{
	this->setScale(vec3(x, y, z));
}

void GLLib::GLVertexArray::setScale(float scale)
{
	this->setScale(vec3(scale));
}

void GLLib::GLVertexArray::setOrigin(const vec3& localPos)
{
	this->m_origin = localPos;
	this->__updateMatrix();
}

void GLLib::GLVertexArray::setOrigin(float x, float y, float z)
{
	this->setOrigin(vec3(x, y, z));
}

vec3 GLLib::GLVertexArray::getOrigin()
{
	return this->m_origin;
}

vec3 GLLib::GLVertexArray::getPosition()
{
	return this->m_position;
}

quat GLLib::GLVertexArray::getRotation()
{
	return this->m_rotation;
}

vec3 GLLib::GLVertexArray::getScale()
{
	return this->m_scale;
}

mat4 GLLib::GLVertexArray::getModelMatrix()
{
	return this->m_modelMatrix;
}

void GLLib::GLVertexArray::__updateMatrix()
{
	this->m_modelMatrix =
		glm::translate(mat4(1.0f), this->m_position)
		* glm::rotate(mat4(1.0f), angle(this->m_rotation), axis(this->m_rotation))
		* glm::scale(mat4(1.0f), this->m_scale)
		* glm::translate(mat4(1.0f), -this->m_origin);
}

GLLib::GLBuffer::GLBuffer()
{
	glGenBuffers(1, &this->m_ptr);
}

GLLib::GLBuffer::~GLBuffer()
{
	glDeleteBuffers(1, &this->m_ptr);
}

void GLLib::GLBuffer::bind(GLenum target)
{
	glBindBuffer(target, this->m_ptr);
}

GLLib::GLTexture::GLTexture()
{
	glGenTextures(1, &this->m_ptr);
}

GLLib::GLTexture::~GLTexture()
{
	glDeleteTextures(1, &this->m_ptr);
}

void GLLib::GLTexture::bind(GLuint unit, GLenum target)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, this->m_ptr);
}

GLLib::Camera::Camera() :
	m_position(0.0f, 0.0f, 1.0f), m_focusPoint(0.0f), m_upVector(0.0f, 1.0f, 0.0f)
{
	this->__updateViewMatrix();
	this->orthogonal(-1, 1, -1, 1, -1, 1);
}

GLLib::Camera::~Camera()
{
}

void GLLib::Camera::setPosition(float x, float y, float z) {
	this->setPosition(vec3(x, y, z));
}

void GLLib::Camera::setPosition(const vec3 & pos) {
	this->m_position = pos;
	this->__updateViewMatrix();
}

vec3 GLLib::Camera::getPosition() {
	return this->m_position;
}

void GLLib::Camera::lookTo(float x, float y, float z) {
	this->lookTo(vec3(x, y, z));
}

void GLLib::Camera::lookTo(const vec3 & direction) {
	this->m_focusPoint = this->m_position + direction;
	this->__updateViewMatrix();
}

void GLLib::Camera::focusOn(float x, float y, float z) {
	this->focusOn(vec3(x, y, z));
}

void GLLib::Camera::focusOn(const vec3 & pos) {
	this->m_focusPoint = pos;
	this->__updateViewMatrix();
}

vec3 GLLib::Camera::focusOn() {
	return this->m_focusPoint;
}

void GLLib::Camera::setUpVector(float x, float y, float z) {
	this->setUpVector(vec3(x, y, z));
}

void GLLib::Camera::setUpVector(const vec3 & dir) {
	this->m_upVector = dir;
	this->__updateViewMatrix();
}

vec3 GLLib::Camera::getUpVector() {
	return this->m_upVector;
}

mat4 GLLib::Camera::getViewMatrix() {
	return this->m_viewMatrix;
}

void GLLib::Camera::perspective(float fovy, float aspect, float zNear, float zFar) {
	this->m_projectionMatrix = glm::perspective(fovy, aspect, zNear, zFar);
}

void GLLib::Camera::orthogonal(float left, float right, float bottom, float top, float zNear, float zFar) {
	this->m_projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
}

mat4 GLLib::Camera::getProjectionMatrix() {
	return this->m_projectionMatrix;
}

void GLLib::Camera::__updateViewMatrix()
{
	this->m_viewMatrix = glm::lookAt(this->m_position, this->m_focusPoint, this->m_upVector);
}

shared_ptr<GLLib::GLFramebuffer> GLLib::GLFramebuffer::s_screen(nullptr);

GLLib::GLFramebuffer::GLFramebuffer()
{
	glGenFramebuffers(1, &this->m_ptr);
}

GLLib::GLFramebuffer::~GLFramebuffer()
{
	if (this->m_ptr)
		glDeleteFramebuffers(1, &this->m_ptr);
}

void GLLib::GLFramebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_ptr);
}

void GLLib::GLFramebuffer::bind(GLenum target)
{
	glBindFramebuffer(target, this->m_ptr);
}

GLLib::GLFramebuffer::GLFramebuffer(GLuint ptr) :
	m_ptr(ptr)
{
}

GLLib::VertexArrayFactory::VertexArrayFactory() :
	m_attributes(), m_data(nullptr), m_dataLength(0)
{
}

GLLib::VertexArrayFactory::~VertexArrayFactory()
{
	if (this->m_data) {
		delete[] this->m_data;
		this->m_data = nullptr;
	}
}

void GLLib::VertexArrayFactory::pushAttribute(const AttrDef & attr)
{
	this->m_attributes.push_back(attr);
}

void GLLib::VertexArrayFactory::popAttribute()
{
	this->m_attributes.pop_back();
}

void GLLib::VertexArrayFactory::clearAttributes()
{
	this->m_attributes.clear();
}

void GLLib::VertexArrayFactory::allocData(size_t length)
{
	if (this->m_data) return;
	this->m_data = new unsigned char[length];
	this->m_dataLength = length;
}

void GLLib::VertexArrayFactory::setData(const void * data, size_t offset, size_t length)
{
	if (!this->m_data) return;
	memcpy((unsigned char*)this->m_data + offset, data, length);
}

void GLLib::VertexArrayFactory::releaseData()
{
	if (!this->m_data) return;
	delete[] this->m_data;
	this->m_data = nullptr;
	this->m_dataLength = 0;
}

GLLib::VertexArray GLLib::VertexArrayFactory::createVAO(size_t count)
{
	VertexArray ret = {};

	if (!this->m_data || this->m_dataLength == 0 || this->m_attributes.size() == 0) return ret;

	ret.vao.reset(new GLVertexArray);
	ret.abo.reset(new GLBuffer);

	GLVertexArray* vao = ret.vao.get();
	GLBuffer* abo = ret.abo.get();
	vao->bind();
	abo->bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, this->m_dataLength, this->m_data, GL_STATIC_DRAW);
	for (AttrDef attr : m_attributes) {
		glVertexAttribPointer(attr.channel, attr.dimension, attr.type, false, attr.stride, reinterpret_cast<void *>(attr.offset));
		glEnableVertexAttribArray(attr.channel);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vao->setDrawMode(GLVertexArray::Array);
	vao->setDrawCount(count);

	return ret;
}

GLLib::VertexElementArray GLLib::VertexArrayFactory::createVAO(unsigned int indices[], size_t count)
{
	VertexElementArray ret = {};

	if (!this->m_data || this->m_dataLength == 0 || this->m_attributes.size() == 0) return ret;

	ret.vao.reset(new GLVertexArray);
	ret.abo.reset(new GLBuffer);
	ret.ebo.reset(new GLBuffer);

	GLVertexArray* vao = ret.vao.get();
	GLBuffer* abo = ret.abo.get();
	GLBuffer* ebo = ret.ebo.get();
	vao->bind();
	abo->bind(GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, this->m_dataLength, this->m_data, GL_STATIC_DRAW);
	for (AttrDef attr : m_attributes) {
		glVertexAttribPointer(attr.channel, attr.dimension, attr.type, false, attr.stride, reinterpret_cast<void *>(attr.offset));
		glEnableVertexAttribArray(attr.channel);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	ebo->bind(GL_ELEMENT_ARRAY_BUFFER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	vao->setDrawMode(GLVertexArray::Element);
	vao->setDrawCount(count);

	return ret;
}

GLLib::AttrDef::AttrDef() :
	AttrDef(0, FOUR, GL_FLOAT, 0, 0)
{
}

GLLib::AttrDef::AttrDef(int channel, Dimension dimension, GLenum type, GLsizeiptr offset, GLsizei stride) :
	channel(channel), dimension(dimension), type(type), offset(offset), stride(stride)
{
}

GLLib::GLRenderbuffer::GLRenderbuffer()
{
	glGenRenderbuffers(1, &this->m_ptr);
}

GLLib::GLRenderbuffer::~GLRenderbuffer()
{
	glDeleteRenderbuffers(1, &this->m_ptr);
}

void GLLib::GLRenderbuffer::bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_ptr);
}
