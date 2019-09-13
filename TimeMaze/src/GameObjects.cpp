#include "GameObjects.h"
#include "Appearances.h"

#include <IL/il.h>

using namespace std;
using namespace glm;
using namespace GLLib;

Hero::Hero() :
	m_onFloor(false), m_color(1.0f), m_type(WHITE)
{
	this->m_heroLight.reset(new Light);
	this->m_heroLight->setColor(this->m_color);

	shared_ptr<GameObject> armA(new GameObject);
	shared_ptr<GameObject> armB(new GameObject);
	shared_ptr<GameObject> armC(new GameObject);

	armA->setAppear(shared_ptr<IAppear>(new RingAppear(0.5f)));
	armB->setAppear(shared_ptr<IAppear>(new RingAppear(0.5f)));
	armC->setAppear(shared_ptr<IAppear>(new RingAppear(0.5f)));

	armA->rotate(pi<float>() * 0.5f, 1, 0, 0);
	armB->rotate(pi<float>() * 0.5f, 0, 0, 1);
	
	this->addChild(armA);
	this->addChild(armB);
	this->addChild(armC);
}

Hero::Hero(const b2Vec2& pos) :
	Hero()
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = pos;
	bodyDef.angularDamping = 20.0f;
	shared_ptr<b2CircleShape> shapeDef(new b2CircleShape);
	shapeDef->m_p.Set(0, 0);
	shapeDef->m_radius = 0.5f;
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.1f;
	fixtureDef.userData = "hero";
	this->set2DPhysics(shared_ptr<PhysicsDef2D>(new PhysicsDef2D{ bodyDef, fixtureDef, shapeDef }));
}

void Hero::setOnFloor(bool value)
{
	this->m_onFloor = value;
}

bool Hero::isOnFloor()
{
	return this->m_onFloor;
}

void Hero::update(double delta)
{
	GameObject::update(delta);

	b2Body* body = this->get2DPhysicsBody();
	Keyboard* keyboard = Keyboard::getInstance();

	int keyState = keyboard->getKeyState(GLFW_KEY_LEFT);
	if (keyState == GLFW_PRESS) {
		body->ApplyTorque(12.0f, true);
	}
	keyState = keyboard->getKeyState(GLFW_KEY_RIGHT);
	if (keyState == GLFW_PRESS) {
		body->ApplyTorque(-12.0f, true);
	}
	keyState = keyboard->getKeyState(GLFW_KEY_UP);
	if (keyState == GLFW_PRESS) {
		if (this->isOnFloor()) {
			body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, 5));
		}
	}
	keyState = keyboard->getKeyState(GLFW_KEY_LEFT_CONTROL);
	if (keyState == GLFW_PRESS) {
		Scene* scene = this->getBelongScene();
		if (scene) {
			scene->forEach([this](GameObject* object) {
				Reactable* reactable = dynamic_cast<Reactable*>(object);
				if (reactable) {
					float dist = glm::distance(object->getWorldPosition(), this->getWorldPosition());
					if (dist < 0.8f) {
						reactable->react(this);
					}
				}
			});
		}
	}
	keyState = keyboard->getKeyState(GLFW_KEY_SPACE);
	if (keyState == GLFW_PRESS) {

	}

	vec3 heroWorldPos = this->getWorldPosition();
	this->m_heroLight->setWorldPosition(heroWorldPos.x, heroWorldPos.y, heroWorldPos.z);
}

void Hero::setColor(const vec3& color)
{
	this->m_color = color;
	this->m_heroLight->setColor(color);
}

vec3 Hero::getColor()
{
	return this->m_color;
}

void Hero::setType(HeroBarrierType type)
{
	this->m_type = type;
}

HeroBarrierType Hero::getType()
{
	return this->m_type;
}

shared_ptr<Light> Hero::getLight()
{
	return this->m_heroLight;
}

Wall::Wall(float width, float height)
{
	shared_ptr<GameObject> faceFront(new GameObject);
	shared_ptr<GameObject> faceTop(new GameObject);
	shared_ptr<GameObject> faceLeft(new GameObject);
	shared_ptr<GameObject> faceRight(new GameObject);
	//shared_ptr<GameObject> faceBack(new GameObject);

	faceFront->setAppear(shared_ptr<IAppear>(new WallAppear(width, height)));
	faceTop->setAppear(shared_ptr<IAppear>(new WallAppear(width, 4.0f)));
	faceLeft->setAppear(shared_ptr<IAppear>(new WallAppear(4.0f, height)));
	faceRight->setAppear(shared_ptr<IAppear>(new WallAppear(4.0f, height)));
	//faceBack->setAppear(shared_ptr<IAppear>(new WallAppear(width, height)));

	faceFront->translate(0, 0, -2.0f);
	faceTop->translate(0, height * 0.5f, 0);
	faceTop->rotate(pi<float>() * 0.5f, 1, 0, 0);
	faceLeft->translate(-width * 0.5f, 0, 0);
	faceLeft->rotate(pi<float>() * 0.5f, 0, 1, 0);
	faceRight->translate(width * 0.5f, 0, 0);
	faceRight->rotate(pi<float>() * 0.5f, 0, -1, 0);
	//faceBack->translate(0, 0, 2.0f);
	//faceBack->rotate(pi<float>(), 0, 1, 0);

	this->addChild(faceFront);
	this->addChild(faceTop);
	this->addChild(faceLeft);
	this->addChild(faceRight);
	//this->addChild(faceBack);
}

Wall::Wall(float width, float height, const b2Vec2 & pos) :
	Wall(width, height)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position = pos;

	b2Vec2 vs[4];
	vs[0].Set(-width * 0.5f, -height * 0.5f);
	vs[1].Set(-width * 0.5f, height * 0.5f);
	vs[2].Set(width * 0.5f, height * 0.5f);
	vs[3].Set(width * 0.5f, -height * 0.5f);
	shared_ptr<b2ChainShape> shapeDef(new b2ChainShape);
	shapeDef->CreateChain(vs, 4);

	b2FixtureDef fixtureDef;
	fixtureDef.friction = 0.4f;

	this->set2DPhysics(shared_ptr<PhysicsDef2D>(new PhysicsDef2D{ bodyDef, fixtureDef, shapeDef }));
}

Floor::Floor(float length, float height)
{
	this->setAppear(shared_ptr<IAppear>(new FloorAppear(length, 4.0f, height)));
}

Floor::Floor(float length, float height, const b2Vec2& pos) :
	Floor(length, height)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position = pos;
	shared_ptr<b2PolygonShape> shapeDef(new b2PolygonShape);
	shapeDef->SetAsBox(length * 0.5f, height * 0.5f);
	b2FixtureDef fixtureDef;
	fixtureDef.friction = 0.4f;
	this->set2DPhysics(shared_ptr<PhysicsDef2D>(new PhysicsDef2D{ bodyDef, fixtureDef, shapeDef }));
}

IronDoor::IronDoor()
{
	this->move_maxY = 3.5f;
	this->move_minY = -3.5f;

	this->base.reset(new GameObject);
	this->door.reset(new GameObject);

	this->base->setAppear(shared_ptr<IAppear>(new IronDoorBaseAppear()));
	this->door->setAppear(shared_ptr<IAppear>(new IronDoorAppear()));
	
	this->addChild(this->base);
	this->addChild(this->door);

	this->rotate(pi<float>() * 0.5, 0, 1, 0);
}

IronDoor::IronDoor(const b2Vec2& pos)
{
	this->base.reset(new GameObject);
	this->door.reset(new GameObject);

	this->base->setAppear(shared_ptr<IAppear>(new IronDoorBaseAppear()));
	this->door->setAppear(shared_ptr<IAppear>(new IronDoorAppear()));

	b2BodyDef baseBodyDef;
	baseBodyDef.type = b2_staticBody;
	baseBodyDef.position = pos - b2Vec2(0.0f, 3.5555f);
	shared_ptr<b2PolygonShape> baseShapeDef(new b2PolygonShape);
	b2Vec2 vs[4];
	vs[0].Set(-0.6f, 0);
	vs[1].Set(0.6f, 0);
	vs[2].Set(0.4f, 0.08f);
	vs[3].Set(-0.4f, 0.08f);
	baseShapeDef->Set(vs, 4);
	b2FixtureDef baseFixtureDef;
	baseFixtureDef.friction = 0.4f;
	this->base->set2DPhysics(shared_ptr<PhysicsDef2D>(new PhysicsDef2D{ baseBodyDef, baseFixtureDef, baseShapeDef }));

	b2BodyDef doorBodyDef;
	doorBodyDef.type = b2_kinematicBody;
	doorBodyDef.position = pos;
	shared_ptr<b2PolygonShape> doorShapeDef(new b2PolygonShape);
	doorShapeDef->SetAsBox(0.0535f, 3.5555f);
	b2FixtureDef doorFixtureDef;
	doorFixtureDef.friction = 0.4f;
	this->door->set2DPhysics(shared_ptr<PhysicsDef2D>(new PhysicsDef2D{ doorBodyDef, doorFixtureDef, doorShapeDef }));

	this->addChild(this->base);
	this->addChild(this->door);

	this->rotate(pi<float>() * 0.5, 0, 1, 0);

	this->move_maxY = pos.y;
	this->move_minY = pos.y - 7;
}

void IronDoor::down()
{
	this->door->get2DPhysicsBody()->SetLinearVelocity(b2Vec2(0, -0.8f));
}

void IronDoor::up()
{
	this->door->get2DPhysicsBody()->SetLinearVelocity(b2Vec2(0, 0.8f));
}

void IronDoor::setMovingRange(float minY, float maxY)
{
	this->move_minY = minY;
	this->move_maxY = maxY;
}

void IronDoor::update(double delta)
{
	GameObject::update(delta);
	b2Body* body = this->door->get2DPhysicsBody();
	if (body->GetPosition().y >= this->move_maxY && body->GetLinearVelocity().y > 0) {
		body->SetLinearVelocity(b2Vec2_zero);
	}
	if (body->GetPosition().y <= this->move_minY && body->GetLinearVelocity().y < 0) {
		body->SetLinearVelocity(b2Vec2_zero);
	}
}

Barrier::Barrier(float height, const b2Vec2 & pos) :
	m_type(BLUE)
{
	this->m_block = true;
	this->m_appear.reset(new BarrierAppear(0.08f, 3.9f, height));
	this->setAppear(this->m_appear);

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position = pos;
	shared_ptr<b2PolygonShape> shapeDef(new b2PolygonShape);
	shapeDef->SetAsBox(0.04f, height * 0.5f);
	b2FixtureDef fixtureDef;
	fixtureDef.friction = 0;
	fixtureDef.userData = "barrier";
	this->m_phyDef.reset(new PhysicsDef2D{ bodyDef, fixtureDef, shapeDef });
	this->set2DPhysics(this->m_phyDef);
}

void Barrier::setColor(const vec3& color)
{
	this->m_color = color;
	this->m_appear->setColor(color);
}

vec3 Barrier::getColor()
{
	return this->m_color;
}

void Barrier::setType(HeroBarrierType type)
{
	this->m_type = type;
}

HeroBarrierType Barrier::getType()
{
	return this->m_type;
}

void Barrier::setBlock(bool block)
{
	this->m_block = block;
}

void Barrier::update(double delta)
{
	GameObject::update(delta);

	if (this->m_block) {
		if (!this->get2DPhysicsBody()) {
			this->set2DPhysics(this->m_phyDef);
		}
	} else {
		if (this->get2DPhysicsBody()) {
			this->set2DPhysics(shared_ptr<PhysicsDef2D>(nullptr));
		}
	}
}

Pedestal::Pedestal() :
	m_color(0.258f, 0.412f, 0.924f), m_type(BLUE)
{
	shared_ptr<GameObject> effect(new GameObject);
	shared_ptr<PedestalEffectAppear> effectAppear(new PedestalEffectAppear);
	effect->setAppear(effectAppear);
	effectAppear->setColor(this->m_color);
	effect->setLocalPosition(0, 0.05f, 0);
	effect->setLocalScale(0.6f, 2.0f, 0.6f);
	this->addChild(effect);

	this->setAppear(shared_ptr<IAppear>(new PedestalAppear));
}

Pedestal::Pedestal(const b2Vec2 & pos) :
	Pedestal()
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position = pos;
	shared_ptr<b2PolygonShape> shapeDef(new b2PolygonShape);
	b2Vec2 vs[4];
	vs[0].Set(-0.6f, 0);
	vs[1].Set(0.6f, 0);
	vs[2].Set(0.6f, 0.1f);
	vs[3].Set(-0.6f, 0.1f);
	shapeDef->Set(vs, 4);
	b2FixtureDef fixtureDef;
	fixtureDef.friction = 0.4f;
	this->set2DPhysics(shared_ptr<PhysicsDef2D>(new PhysicsDef2D{ bodyDef, fixtureDef, shapeDef }));
}

void Pedestal::setType(HeroBarrierType type)
{
	this->m_type = type;
}

HeroBarrierType Pedestal::getType()
{
	return this->m_type;
}

void Pedestal::react(Hero* hero)
{
	hero->setColor(this->m_color);
	hero->setType(this->m_type);
}
