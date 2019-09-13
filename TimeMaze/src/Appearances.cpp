#include "Appearances.h"

using namespace std;
using namespace glm;
using namespace GLLib;

shared_ptr<GLTexture> ShieldBallAppear::colorTex(nullptr);

shared_ptr<GLTexture> FloorAppear::normalTex(nullptr);
shared_ptr<GLTexture> FloorAppear::colorTex(nullptr);
shared_ptr<GLTexture> FloorAppear::specTex(nullptr);

shared_ptr<GLTexture> WallAppear::normalTex(nullptr);
shared_ptr<GLTexture> WallAppear::colorTex(nullptr);
shared_ptr<GLTexture> WallAppear::specTex(nullptr);

shared_ptr<GLTexture> IronDoorAppear::normalTex(nullptr);
shared_ptr<GLTexture> IronDoorAppear::colorTex(nullptr);
shared_ptr<GLTexture> IronDoorAppear::specTex(nullptr);

shared_ptr<GLTexture> IronDoorBaseAppear::normalTex(nullptr);
shared_ptr<GLTexture> IronDoorBaseAppear::colorTex(nullptr);
shared_ptr<GLTexture> IronDoorBaseAppear::specTex(nullptr);

shared_ptr<GLTexture> BarrierAppear::animationTex(nullptr);

shared_ptr<GLTexture> BarrierSideAppear::normalTex(nullptr);
shared_ptr<GLTexture> BarrierSideAppear::colorTex(nullptr);
shared_ptr<GLTexture> BarrierSideAppear::specTex(nullptr);

shared_ptr<GLTexture> PedestalAppear::normalTex(nullptr);
shared_ptr<GLTexture> PedestalAppear::colorTex(nullptr);
shared_ptr<GLTexture> PedestalAppear::specTex(nullptr);

shared_ptr<GLTexture> PedestalEffectAppear::colorTex(nullptr);

RingAppear::RingAppear(float radius)
{
	this->m_vertexArray = GeometryCreator::createRing(64, 16, radius, radius * 0.05f);
}

shared_ptr<GLVertexArray> RingAppear::geometry()
{
	return this->m_vertexArray.vao;
}

ShieldBallAppear::ShieldBallAppear()
{
	this->m_vertexArray = GeometryCreator::createSphere(16, 16);
	this->m_color = vec3(1.0f);
	this->m_alpha = 0.5f;
	if (!colorTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/shield/effect.png");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
}

shared_ptr<GLVertexArray> ShieldBallAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void ShieldBallAppear::textures(std::vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
}

bool ShieldBallAppear::doBlend()
{
	return true;
}

bool ShieldBallAppear::doCullface()
{
	return false;
}

float ShieldBallAppear::alpha()
{
	return this->m_alpha;
}

vec3 ShieldBallAppear::color()
{
	return this->m_color;
}

void ShieldBallAppear::setColor(const vec3 & color)
{
	this->m_color = color;
}

void ShieldBallAppear::setAlpha(float alpha)
{
	this->m_alpha = alpha;
}

FloorAppear::FloorAppear(float length, float width, float height)
{
	this->m_vertexArray = GeometryCreator::createCube(length, width, height, 1);

	if (!colorTex.get() || !normalTex.get() || !specTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		
		colorTex.reset(new GLTexture);
		normalTex.reset(new GLTexture);
		specTex.reset(new GLTexture);

		normalTex->bind(0, GL_TEXTURE_2D);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		ilLoadImage("res/meshes/floor/FloorNormalMap.jpg");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/floor/FloorTextureMap.jpg");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		specTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/floor/FloorSpecularMap.jpg");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
}

shared_ptr<GLVertexArray> FloorAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void FloorAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
	out.push_back(TexFlag{ normalTex, TexFlag::NORMALMAP });
	out.push_back(TexFlag{ specTex, TexFlag::SPECMAP });
}

WallAppear::WallAppear(float width, float height)
{
	this->m_vertexArray = GeometryCreator::createPanel(width, height, 2);

	if (!colorTex.get() || !normalTex.get() || !specTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);
		normalTex.reset(new GLTexture);
		specTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		normalTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/wall/WallNormalMap.jpg");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/wall/WallTextureMap.jpg");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		specTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/wall/WallSpecularMap.jpg");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}

}

shared_ptr<GLVertexArray> WallAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void WallAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
	out.push_back(TexFlag{ normalTex, TexFlag::NORMALMAP });
	out.push_back(TexFlag{ specTex, TexFlag::SPECMAP });
}

IronDoorAppear::IronDoorAppear()
{
	this->m_vertexArray = GeometryCreator::createFromOBJFile("res/meshes/iron_door/door.obj");
	if (!colorTex.get() || !normalTex.get() || !specTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);
		normalTex.reset(new GLTexture);
		specTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/iron_door/doorTexture.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		normalTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/iron_door/doorNormal.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		specTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/iron_door/doorSpecular.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
	this->m_vertexArray.vao->setOrigin(0, 3.54f, 0);
}

shared_ptr<GLVertexArray> IronDoorAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void IronDoorAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
	out.push_back(TexFlag{ normalTex, TexFlag::NORMALMAP });
	out.push_back(TexFlag{ specTex, TexFlag::SPECMAP });
}

IronDoorBaseAppear::IronDoorBaseAppear()
{
	this->m_vertexArray = GeometryCreator::createFromOBJFile("res/meshes/iron_door/base.obj");
	if (!colorTex.get() || !normalTex.get() || !specTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);
		normalTex.reset(new GLTexture);
		specTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/iron_door/baseTexture.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		normalTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/iron_door/baseNormal.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		specTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/iron_door/baseSpecular.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
}

shared_ptr<GLVertexArray> IronDoorBaseAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void IronDoorBaseAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
	out.push_back(TexFlag{ normalTex, TexFlag::NORMALMAP });
	out.push_back(TexFlag{ specTex, TexFlag::SPECMAP });
}

BarrierAppear::BarrierAppear(float length, float width, float height)
{
	this->m_vertexArray = GeometryCreator::createCube(length, width, height, 8);
	this->m_color = vec3(1.0f);
	if (!animationTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		animationTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);
		
		animationTex->bind(0, GL_TEXTURE_3D);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R8, 512, 512, 61);
		for (int i = 0; i <= 60; ++i) {
			char path[64];
			sprintf(path, "res/meshes/barrier/red3_%05d.tif", i);
			ilLoadImage(path);
			glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, i, 512, 512, 1, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		}
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
}

shared_ptr<GLVertexArray> BarrierAppear::geometry()
{
	return this->m_vertexArray.vao;
}

vec3 BarrierAppear::color()
{
	return this->m_color;
}

void BarrierAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ animationTex, TexFlag::ANIMATED_COLORMAP });
}

float BarrierAppear::alpha()
{
	return 0.5f;
}

bool BarrierAppear::doBlend()
{
	return true;
}

unsigned char BarrierAppear::textureChannelLink()
{
	return 0b01011000;
}

bool BarrierAppear::texColorInverse()
{
	return true;
}

void BarrierAppear::setColor(const vec3 & color)
{
	this->m_color = color;
}

BarrierSideAppear::BarrierSideAppear()
{
	this->m_vertexArray = GeometryCreator::createFromOBJFile("res/meshes/barrier/side/side.OBJ");
	if (!colorTex.get() || !normalTex.get() || !specTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);
		normalTex.reset(new GLTexture);
		specTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/barrier/side/texture.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		normalTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/barrier/side/normalmap.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		specTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/barrier/side/specularmap.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
}

shared_ptr<GLVertexArray> BarrierSideAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void BarrierSideAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
	out.push_back(TexFlag{ normalTex, TexFlag::NORMALMAP });
	out.push_back(TexFlag{ specTex, TexFlag::SPECMAP });
}

PedestalAppear::PedestalAppear()
{
	this->m_vertexArray = GeometryCreator::createFromOBJFile("res/meshes/pedestal/pedestal.obj");
	if (!colorTex.get() || !normalTex.get() || !specTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);
		normalTex.reset(new GLTexture);
		specTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/pedestal/pedestalTexture.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		normalTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/pedestal/pedestalNormal.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		specTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/pedestal/pedestalSpecular.tif");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
	this->m_vertexArray.vao->setOrigin(0, -0.05f, 0);
}

shared_ptr<GLVertexArray> PedestalAppear::geometry()
{
	return this->m_vertexArray.vao;
}

void PedestalAppear::textures(std::vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
	out.push_back(TexFlag{ normalTex, TexFlag::NORMALMAP });
	out.push_back(TexFlag{ specTex, TexFlag::SPECMAP });
}

PedestalEffectAppear::PedestalEffectAppear()
{
	this->m_vertexArray = GeometryCreator::createCylinder(16);
	this->m_color = vec3(1.0f);
	if (!colorTex.get()) {
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		colorTex.reset(new GLTexture);

		ILuint image_ptr = ilGenImage();
		ilBindImage(image_ptr);

		colorTex->bind(0, GL_TEXTURE_2D);

		ilLoadImage("res/meshes/pedestal/effect.png");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		ilBindImage(0);
		ilDeleteImage(image_ptr);
	}
}

shared_ptr<GLVertexArray> PedestalEffectAppear::geometry()
{
	return this->m_vertexArray.vao;
}

vec3 PedestalEffectAppear::color()
{
	return this->m_color;
}

float PedestalEffectAppear::alpha()
{
	return 0.5f;
}

void PedestalEffectAppear::textures(vector<TexFlag>& out)
{
	out.push_back(TexFlag{ colorTex, TexFlag::COLORMAP });
}

bool PedestalEffectAppear::doBlend()
{
	return true;
}

bool PedestalEffectAppear::doCullface()
{
	return false;
}

void PedestalEffectAppear::setColor(const vec3 & color)
{
	this->m_color = color;
}
