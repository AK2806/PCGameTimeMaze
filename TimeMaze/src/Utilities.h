#ifndef UTILITIES_H
#define UTILITIES_H

#include "GLLib.h"
#include "Geometry.h"

#include <vector>
#include <map>
#include <memory>
#include <stack>
#include <functional>
#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <IL/il.h>
#include <tweeny2/tweeny.h>

struct PhysicsDef2D;

class IAppear;
class GameObject;
class Light;
class Scene;
class Director;
class Animation;
class Renderer;
class Keyboard;

struct PhysicsDef2D {
	b2BodyDef bodyDef;
	b2FixtureDef fixtureDef;
	std::shared_ptr<b2Shape> shapeDef;
};

class IAppear {
public:
	struct TexFlag {
		std::shared_ptr<GLLib::GLTexture> texture;
		enum { COLORMAP, NORMALMAP, ENVMAP, SPECMAP, ANIMATED_COLORMAP } type;
	};

	virtual ~IAppear() {}
	
	virtual std::shared_ptr<GLLib::GLVertexArray> geometry() = 0;
	virtual glm::vec3 color() { return glm::vec3(1.0f, 1.0f, 1.0f); }
	virtual float shininess() { return 8.0f; }
	virtual void textures(std::vector<TexFlag>& out) {}
	virtual float alpha() { return 1.0f; }
	virtual bool doCullface() { return true; }
	virtual bool doBlend() { return false; }
	virtual bool texColorInverse() { return false; }
	virtual bool texAlphaInverse() { return false; }
	virtual unsigned char blendParameter() { return 0; }
	virtual unsigned char textureChannelLink() { return 0b00011011; }
	virtual bool lightEffect() { return true; }
	virtual float animationDuring() { return 1.0f; }
};

class GameObject {
	friend class Scene;
	friend class Director;
public:
	GameObject();
	virtual ~GameObject();

	void addChild(const std::shared_ptr<GameObject>& child);
	void removeChild(const std::shared_ptr<GameObject>& child);

	void forEach(const std::function<void(GameObject*)>& func);

	void set2DPhysics(const std::shared_ptr<PhysicsDef2D>& def);
	b2Body* get2DPhysicsBody();

	void setAppear(const std::shared_ptr<IAppear>& appear);
	std::shared_ptr<IAppear> getAppear();

	virtual void update(double delta);

	void translate(const glm::vec3& dis);
	void translate(float x, float y, float z);
	void rotate(float angle, const glm::vec3& axis);
	void rotate(float angle, float x, float y, float z);

	void setLocalPosition(const glm::vec3& pos);
	void setLocalPosition(float x, float y, float z);
	glm::vec3 getLocalPosition();
	void setLocalRotation(const glm::quat& rotation);
	glm::quat getLocalRotation();
	void setLocalScale(const glm::vec3& scale);
	void setLocalScale(float scale);
	void setLocalScale(float x, float y, float z);
	glm::vec3 getLocalScale();

	void setWorldPosition(const glm::vec3& pos);
	void setWorldPosition(float x, float y, float z);
	glm::vec3 getWorldPosition();
	void setWorldRotation(const glm::quat& rotation);
	glm::quat getWorldRotation();
	void setWorldScale(const glm::vec3& scale);
	void setWorldScale(float scale);
	void setWorldScale(float x, float y, float z);
	glm::vec3 getWorldScale();

	std::shared_ptr<GameObject> getFirstChild();
	std::shared_ptr<GameObject> getNextSibling();

	Scene* getBelongScene();

private:
	void __setChildrenBelongScene(Scene* scene);
	void __createChildren2DPhysicsBody();
	void __removeChildren2DPhysicsBody();

	Scene* m_belongScene;
	GameObject* m_parent;
	std::shared_ptr<GameObject> m_child;
	std::shared_ptr<GameObject> m_sibling;

	std::shared_ptr<IAppear> m_appear;
	std::shared_ptr<PhysicsDef2D> m_physicsDef2D;
	b2Body* m_body2d;
	float m_lastAngle;

	glm::vec3 m_localPosition;
	glm::quat m_localRotation;
	glm::vec3 m_localScale;
};

class Light {
public:
	Light();
	virtual ~Light();

	virtual void update(double delta);

	void translate(const glm::vec3& dis);
	void translate(float x, float y, float z);
	
	void setWorldPosition(const glm::vec3& pos);
	void setWorldPosition(float x, float y, float z);
	glm::vec3 getWorldPosition();
	
	void setColor(glm::vec3 color);
	void setColor(float r, float g, float b);
	void setStrength(float s);
	glm::vec3 getColor();
	float getStrenth();

private:
	Scene* m_belongScene;
	
	glm::vec3 m_position;
	
	glm::vec3 m_lightColor;
	float m_lightStrength;
};

class Scene : public b2ContactListener {
	friend class Director;
public:
	Scene();
	virtual ~Scene();

	virtual void init();
	virtual void resume();
	virtual void update(double delta);
	virtual void paused();
	virtual void finished();

	void addChild(const std::shared_ptr<GameObject>& child);
	void removeChild(const std::shared_ptr<GameObject>& child);
	std::shared_ptr<GameObject> getFirstChild();

	void forEach(const std::function<void(GameObject*)>& func);

	b2World* getPhysicsWorld() { return this->m_physicsWorld.get(); }

	void addLight(const std::shared_ptr<Light>& light);
	void removeLight(const std::shared_ptr<Light>& light);

	void setAmbientLightColor(float r, float g, float b);

	std::shared_ptr<GLLib::Camera> getCamera();

private:
	static constexpr int velocityIterations = 8;
	static constexpr int positionIterations = 3;

	void __updateChild(const std::shared_ptr<GameObject>& parent, double delta);

	std::shared_ptr<GameObject> m_rootObject;
	std::unique_ptr<b2World> m_physicsWorld;
	std::vector< std::shared_ptr<Light> > m_lights;

	std::shared_ptr<GLLib::Camera> m_camera;
};

class Director final {
public:
	static Director* getInstance() {
		if (!instance.get()) instance.reset(new Director);
		return instance.get();
	}

	~Director();

	Director(const Director&) = delete;
	Director(Director&&) = delete;
	Director& operator=(const Director&) = delete;
	Director& operator=(Director&&) = delete;

	void setUpdateFrequency(double delta) { this->m_updateDelta = delta; }
	double getUpdateFrequency() { return this->m_updateDelta; }

	void useScene(const std::shared_ptr<Scene>& scene);
	void pushScene(const std::shared_ptr<Scene>& scene);
	void popScene();

	void update();

private:
	void __addChildrenMeshToRenderer(const std::shared_ptr<GameObject>& parent);

	Director();

	static std::unique_ptr<Director> instance;

	std::stack< std::shared_ptr< Scene > > m_sceneStack;
	double m_updateDelta;
	double m_lastTime;
};

class Animation final {
public:
	static Animation* getInstance() {
		if (!instance.get()) instance.reset(new Animation);
		return instance.get();
	}

	~Animation();

	Animation(const Animation&) = delete;
	Animation(Animation&&) = delete;
	Animation& operator=(const Animation&) = delete;
	Animation& operator=(Animation&&) = delete;

	void step(double delta);

private:
	Animation();

	static std::unique_ptr<Animation> instance;

};

class Renderer final {
public:
	static int screenWidth;
	static int screenHeight;

public:
	static Renderer* getInstance() {
		return instance.get();
	}

	static void init(GLFWwindow* window);

	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void addRenderable(const std::shared_ptr<IAppear>& renderable);
	void removeRenderable(const std::shared_ptr<IAppear>& renderable);
	void clearRenderables();

	void render();

	void setAmbientLightColor(float r, float g, float b);
	void setLightPos(int index, float x, float y, float z);
	void setLightColor(int index, float r, float g, float b);
	void setLightStrength(int index, float s);
	void openLight(int index, bool enabled);

	std::shared_ptr<GLLib::Camera> getCamera();

private:
	static constexpr double frameRate = 60.0;
	static constexpr double frameTime = 1.0 / frameRate;

	static constexpr int shadowmapSize = 1024;

	static constexpr float shadowZNear = 0.1f;
	static constexpr float shadowZFar = 100.0f;

	Renderer();

	static std::unique_ptr<Renderer> instance;

	GLFWwindow* m_window;
	double m_frameBeginTime;
	std::vector< std::shared_ptr<IAppear> > m_renderables;
	std::shared_ptr<GLLib::Camera> m_camera;

	int m_useLight[16];
	glm::vec3 m_lightPos[16];
	glm::vec3 m_lightColor[16];
	float m_lightStrength[16];

	GLLib::VertexElementArray m_screenQuadVAO;

	std::shared_ptr<GLLib::RenderProcess> m_shadowMapProcess;
	std::shared_ptr<GLLib::GLTexture> m_cubeShadowMap[16];

	std::shared_ptr<GLLib::RenderProcess> m_renderProcess;
	std::shared_ptr<GLLib::GLRenderbuffer> m_ssDepthRenderbuffer;
	std::shared_ptr<GLLib::GLTexture> m_ssDepthTexture;
	std::shared_ptr<GLLib::GLTexture> m_ssColorTexture;

	std::shared_ptr<GLLib::RenderProcess> m_blendCtrProcess;
	std::shared_ptr<GLLib::RenderProcess> m_blendProcess;
	std::shared_ptr<GLLib::GLTexture> m_headPtrTexture; // record head fragment for OIT
	std::shared_ptr<GLLib::GLTexture> m_fragmentStorageBufferTexture;
	std::shared_ptr<GLLib::GLTexture> m_blendedColorTexture;
	std::shared_ptr<GLLib::GLBuffer> m_ptrInitializer; // initialize head pointer texture every frame
	std::shared_ptr<GLLib::GLBuffer> m_fragmentStorageBuffer;
	std::shared_ptr<GLLib::GLBuffer> m_atomicCounter;

	std::shared_ptr<GLLib::RenderProcess> m_bloomSplitProcess;
	std::shared_ptr<GLLib::RenderProcess> m_bloomMergeProcess;
	std::shared_ptr<GLLib::GLTexture> m_baseColorTexture;
	std::shared_ptr<GLLib::GLTexture> m_highlightColorTexture;
	std::shared_ptr<GLLib::GLRenderbuffer> m_bloomedRenderbuffer;

	std::shared_ptr<GLLib::RenderProcess> m_blurProcess;
	std::shared_ptr<GLLib::GLTexture> m_bluredTexture;
};

class Keyboard final {
public:
	static Keyboard* getInstance() {
		return instance.get();
	}

	~Keyboard();
	
	Keyboard(const Keyboard&) = delete;
	Keyboard(Keyboard&&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	Keyboard& operator=(Keyboard&&) = delete;

	static void init(GLFWwindow* window);

	int getKeyState(int key);
	
private:
	Keyboard(GLFWwindow* window);

	static std::unique_ptr<Keyboard> instance;

	GLFWwindow* m_window;
};

#endif