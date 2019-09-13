#ifndef APPEARANCES_H
#define APPEARANCES_H

#include "Utilities.h"
#include <memory>

class RingAppear : public IAppear {
public:
	RingAppear(float radius);
	std::shared_ptr<GLLib::GLVertexArray> geometry() override;

private:
	GLLib::VertexElementArray m_vertexArray;
};

class ShieldBallAppear : public IAppear {
public:
	ShieldBallAppear();
	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;
	bool doBlend() override;
	bool doCullface() override;
	float alpha() override;
	glm::vec3 color() override;

	void setColor(const glm::vec3& color);
	void setAlpha(float alpha);

private:
	GLLib::VertexElementArray m_vertexArray;
	glm::vec3 m_color;
	float m_alpha;

	static std::shared_ptr<GLLib::GLTexture> colorTex;
};

class FloorAppear : public IAppear {
public:
	FloorAppear(float length, float width, float height);

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;

private:
	GLLib::VertexElementArray m_vertexArray;

	static std::shared_ptr<GLLib::GLTexture> normalTex;
	static std::shared_ptr<GLLib::GLTexture> colorTex;
	static std::shared_ptr<GLLib::GLTexture> specTex;
};

class WallAppear : public IAppear {
public:
	WallAppear(float width, float height);

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;

private:
	GLLib::VertexElementArray m_vertexArray;

	static std::shared_ptr<GLLib::GLTexture> normalTex;
	static std::shared_ptr<GLLib::GLTexture> colorTex;
	static std::shared_ptr<GLLib::GLTexture> specTex;
};

class IronDoorAppear : public IAppear {
public:
	IronDoorAppear();

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;

private:
	GLLib::VertexArray m_vertexArray;

	static std::shared_ptr<GLLib::GLTexture> normalTex;
	static std::shared_ptr<GLLib::GLTexture> colorTex;
	static std::shared_ptr<GLLib::GLTexture> specTex;
};

class IronDoorBaseAppear : public IAppear {
public:
	IronDoorBaseAppear();

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;

private:
	GLLib::VertexArray m_vertexArray;

	static std::shared_ptr<GLLib::GLTexture> normalTex;
	static std::shared_ptr<GLLib::GLTexture> colorTex;
	static std::shared_ptr<GLLib::GLTexture> specTex;
};

class BarrierAppear : public IAppear {
public:
	BarrierAppear(float length, float width, float height);

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	glm::vec3 color() override;
	void textures(std::vector<TexFlag>& out) override;
	float alpha() override;
	bool doBlend() override;
	unsigned char textureChannelLink() override;
	bool texColorInverse() override;

	void setColor(const glm::vec3& color);

private:
	GLLib::VertexElementArray m_vertexArray;
	glm::vec3 m_color;

	static std::shared_ptr<GLLib::GLTexture> animationTex;
};

class BarrierSideAppear : public IAppear {
public:
	BarrierSideAppear();

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;

private:
	GLLib::VertexArray m_vertexArray;

	static std::shared_ptr<GLLib::GLTexture> normalTex;
	static std::shared_ptr<GLLib::GLTexture> colorTex;
	static std::shared_ptr<GLLib::GLTexture> specTex;
};

class PedestalAppear : public IAppear {
public:
	PedestalAppear();

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	void textures(std::vector<TexFlag>& out) override;

private:
	GLLib::VertexArray m_vertexArray;

	static std::shared_ptr<GLLib::GLTexture> normalTex;
	static std::shared_ptr<GLLib::GLTexture> colorTex;
	static std::shared_ptr<GLLib::GLTexture> specTex;
};

class PedestalEffectAppear : public IAppear {
public:
	PedestalEffectAppear();

	std::shared_ptr<GLLib::GLVertexArray> geometry() override;
	glm::vec3 color() override;
	float alpha() override;
	void textures(std::vector<TexFlag>& out) override;
	bool doBlend() override;
	bool doCullface() override;

	void setColor(const glm::vec3& color);

private:
	GLLib::VertexElementArray m_vertexArray;
	glm::vec3 m_color;

	static std::shared_ptr<GLLib::GLTexture> colorTex;
};
#endif