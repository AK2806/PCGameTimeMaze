#ifndef GLLIB_H
#define GLLIB_H

#define GLM_FORCE_SWIZZLE

#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLLib {
	class RenderProcess;
	class GLShaderProgram;
	class GLShader;
	class GLFramebuffer;
	class GLRenderbuffer;
	class GLTexture;
	class GLVertexArray;
	class GLBuffer;
	class Camera;

	struct VertexArray;
	struct VertexElementArray;
	class VertexArrayFactory;
	struct AttrDef;

	class RenderProcess final {
	public:
		RenderProcess(const std::shared_ptr<Camera>& camera);
		RenderProcess(const std::shared_ptr<Camera>& camera, const std::shared_ptr<GLShaderProgram>& pipeline);
		~RenderProcess();

		void addObject(const std::shared_ptr<GLVertexArray>& object);
		void removeObject(const std::shared_ptr<GLVertexArray>& object);
		void clearObjects();

		void setPipeline(const std::shared_ptr<GLShaderProgram>& pipeline) { this->m_pipeline = pipeline; };
		std::shared_ptr<GLShaderProgram> getPipeline() { return this->m_pipeline; };
		void setFramebuffer(const std::shared_ptr<GLFramebuffer>& frameBuffer) { this->m_frameBuffer = frameBuffer; };
		std::shared_ptr<GLFramebuffer> getFramebuffer() { return this->m_frameBuffer; };

		void setCamera(const std::shared_ptr<Camera>& camera) { this->m_camera = camera; };
		std::shared_ptr<Camera> getCamera() { return this->m_camera; };

		void setModelMatUniformLoc(GLint location) { this->m_modelMatLoc = location; }
		void setViewMatUniformLoc(GLint location) { this->m_viewMatLoc = location; }
		void setProjMatUniformLoc(GLint location) { this->m_projMatLoc = location; }

		void useMVP(bool enabled);

		void render();

	private:
		std::vector< std::shared_ptr<GLVertexArray> > m_objects;
		std::shared_ptr<GLShaderProgram> m_pipeline;
		std::shared_ptr<GLFramebuffer> m_frameBuffer;

		GLint m_modelMatLoc;
		GLint m_viewMatLoc;
		GLint m_projMatLoc;

		std::shared_ptr<Camera> m_camera;

		bool m_useMVP;
	};

	class GLShaderProgram final
	{
	public:
		GLShaderProgram();
		~GLShaderProgram();

		void attachShader(const GLShader& shader); // attach shader
		bool build(); // build the shader program
		bool build(std::string& info); // build the shader program and output information when fail (for debugging)
		void use(); // use the built program

		GLint getUniformLocation(const std::string& name);

		operator GLuint() { return this->m_ptr; }
	private:
		GLuint m_ptr;
	};

	class GLShader final
	{
		friend class GLShaderProgram;
	public:
		GLShader(GLenum shaderType);
		~GLShader();

		void setSrc(const std::string& src); // directly set the source code
		bool loadFile(const std::string& filename); // load shader from file
		bool compile(); // compile the shader
		bool compile(std::string& info); // compile the shader and output information when fail (for debugging)

		operator GLuint() { return this->m_ptr; }
	private:
		GLuint m_ptr;
		std::string m_src;
	};

	class GLFramebuffer final {
	public:
		static std::shared_ptr<GLFramebuffer> getScreen() {
			if (!s_screen.get()) {
				s_screen.reset(new GLFramebuffer(0));
			}
			return s_screen;
		}

	public:
		GLFramebuffer();
		~GLFramebuffer();

		void bind();
		void bind(GLenum target);

		operator GLuint() { return this->m_ptr; }
	private:
		static std::shared_ptr<GLFramebuffer> s_screen;

		GLFramebuffer(GLuint ptr);

	private:
		GLuint m_ptr;
	};
	
	class GLRenderbuffer final {
	public:
		GLRenderbuffer();
		~GLRenderbuffer();

		void bind();

		operator GLuint() { return this->m_ptr; }
	private:
		GLuint m_ptr;
	};

	class GLTexture final {
	public:
		GLTexture();
		~GLTexture();

		void bind(GLuint unit, GLenum target);

		operator GLuint() { return this->m_ptr; }
	private:
		GLuint m_ptr;
	};

	class GLVertexArray final {
	public:
		enum DrawMode { Array, Element };

		GLVertexArray();
		~GLVertexArray();

		void bind();
		
		void setDrawMode(DrawMode mode) { this->m_drawMode = mode; };
		DrawMode getDrawMode() { return this->m_drawMode; };
		void setPrimitive(GLenum primitive) { this->m_primitive = primitive; };
		GLenum getPrimitive() { return this->m_primitive; };
		void setBeginPos(GLint index) { this->m_startIndex = index; };
		GLint getBeginPos() { return this->m_startIndex; };
		void setDrawCount(GLsizei count) { this->m_vertexCount = count; };
		GLsizei getDrawCount() { return this->m_vertexCount; };

		void setElementType(GLenum type) { this->m_elementType = type; } // only affects when drawing mode is the element mode
		GLenum getElementType() { return this->m_elementType; };

		void translate(const glm::vec3& dis);
		void translate(float x, float y, float z);
		void rotate(float angle, const glm::vec3& axis);
		void rotate(float angle, float x, float y, float z);

		void setPosition(const glm::vec3& pos);
		void setPosition(float x, float y, float z);
		void setRotation(const glm::quat& rotation);
		void setScale(const glm::vec3& scale);
		void setScale(float x, float y, float z);
		void setScale(float scale);

		void setOrigin(const glm::vec3& localPos);
		void setOrigin(float x, float y, float z);
		
		glm::vec3 getPosition();
		glm::quat getRotation();
		glm::vec3 getScale();
		glm::vec3 getOrigin();

		glm::mat4 getModelMatrix();

		operator GLuint() { return this->m_ptr; }
	private:
		void __updateMatrix();

		GLuint m_ptr;
		DrawMode m_drawMode;
		GLenum m_primitive;
		GLint m_startIndex;
		GLsizei m_vertexCount;
		GLenum m_elementType;

		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::vec3 m_scale;
		glm::vec3 m_origin;

		glm::mat4 m_modelMatrix;
	};

	class GLBuffer final {
	public:
		GLBuffer();
		~GLBuffer();

		void bind(GLenum target);

		operator GLuint() { return this->m_ptr; }
	private:
		GLuint m_ptr;
	};

	class Camera final {
	public:
		Camera();
		~Camera();

		void setPosition(float x, float y, float z);
		void setPosition(const glm::vec3& pos);
		glm::vec3 getPosition();

		void lookTo(float x, float y, float z);
		void lookTo(const glm::vec3& direction);
		void focusOn(float x, float y, float z);
		void focusOn(const glm::vec3& pos);
		glm::vec3 focusOn();

		void setUpVector(float x, float y, float z);
		void setUpVector(const glm::vec3& dir);
		glm::vec3 getUpVector();

		glm::mat4 getViewMatrix();

		void perspective(float fovy, float aspect, float zNear, float zFar);
		void orthogonal(float left, float right, float bottom, float top, float zNear, float zFar);

		glm::mat4 getProjectionMatrix();

	private:
		void __updateViewMatrix();

		glm::vec3 m_position;
		glm::vec3 m_focusPoint;
		glm::vec3 m_upVector;

		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;
	};

	struct VertexArray {
		std::shared_ptr<GLVertexArray> vao;
		std::shared_ptr<GLBuffer> abo;
	};

	struct VertexElementArray {
		std::shared_ptr<GLVertexArray> vao;
		std::shared_ptr<GLBuffer> abo;
		std::shared_ptr<GLBuffer> ebo;
	};

	class VertexArrayFactory final {
	public:
		VertexArrayFactory();
		~VertexArrayFactory();

		void setAttributes(const std::vector<AttrDef>& attributes) { this->m_attributes = attributes; };
		std::vector<AttrDef> getAttributes() { return this->m_attributes; };

		void pushAttribute(const AttrDef& attr);
		void popAttribute();
		void clearAttributes();

		void allocData(size_t length);
		void setData(const void* data, size_t offset, size_t length);
		void releaseData();

		void* data() { return this->m_data; };
		size_t dataLength() { return this->m_dataLength; };

		VertexArray createVAO(size_t count); // create a new VAO drawing in array mode
		VertexElementArray createVAO(unsigned int indices[], size_t count); // create a new VAO drawing in element mode

	private:
		std::vector<AttrDef> m_attributes;
		void* m_data;
		size_t m_dataLength;
	};

	struct AttrDef {
		enum Dimension { ONE = 1, TWO = 2, THREE = 3, FOUR = 4 };

		AttrDef();
		AttrDef(int channel, Dimension dimension, GLenum type, GLsizeiptr offset, GLsizei stride);

		int channel;
		Dimension dimension;
		GLenum type;
		GLsizeiptr offset;
		GLsizei stride;
	};
}

#endif