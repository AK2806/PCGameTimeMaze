#include "GameScenes.h"

using namespace GLLib;
using namespace glm;

void MasterScene::init()
{
	Scene::init();

	this->hero.reset(new Hero(b2Vec2(0, 6)));

	this->addLight(this->hero->getLight());

	this->wall.reset(new Wall(50, 10, b2Vec2(0, 5)));
	this->floor.reset(new Floor(50, 1, b2Vec2(0, -0.5)));
	this->floor2.reset(new Floor(5, 1, b2Vec2(-2.6, 1.5)));
	this->floor3.reset(new Floor(10, 1, b2Vec2(0, 0.5)));

	shared_ptr<Barrier> barrier1(new Barrier(4.0f, b2Vec2(10, 2)));

	this->barriers.push_back(barrier1);
	this->pedestal.reset(new Pedestal(b2Vec2(-2, 2)));
	barrier1->setColor(vec3(0.516f, 0.824f, 1.848f));
	
	this->addChild(this->hero);
	this->addChild(this->floor);
	this->addChild(this->wall);
	this->addChild(this->floor3);
	this->addChild(this->floor2);
	this->addChild(barrier1);
	this->addChild(this->pedestal);
	//this->addChild(this->door);
}

void MasterScene::update(double delta)
{
	Scene::update(delta);

	shared_ptr<Camera> camera = this->getCamera();

	vec3 heroWorldPos = this->hero->getWorldPosition();

	camera->setPosition(heroWorldPos.x + 1, heroWorldPos.y + 1, 5);
	camera->focusOn(heroWorldPos.x + 1, heroWorldPos.y, heroWorldPos.z);

	for (shared_ptr<Barrier> barrier : this->barriers) {
		if (this->hero->getType() == barrier->getType()) {
			barrier->setBlock(false);
		} else {
			barrier->setBlock(true);
		}
	}
}

void MasterScene::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if ((fixtureA->GetUserData() && strcmp((char *)fixtureA->GetUserData(), "hero") == 0)
		|| (fixtureB->GetUserData() && strcmp((char *)fixtureB->GetUserData(), "hero") == 0)) {
		b2WorldManifold manifold;
		contact->GetWorldManifold(&manifold);
		if (b2Dot(manifold.normal, b2Vec2(0, 1)) >= 0.5f) {
			this->hero->setOnFloor(true);
		}
		if (!(fixtureA->GetUserData() && fixtureB->GetUserData())) {
			if (b2Dot(manifold.normal, b2Vec2(0, 1)) >= 0.866f) {
				contact->SetFriction(5.0f);
			}
		}
	}
}

void MasterScene::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	if ((fixtureA->GetUserData() && strcmp((char *)fixtureA->GetUserData(), "hero") == 0)
		|| (fixtureB->GetUserData() && strcmp((char *)fixtureB->GetUserData(), "hero") == 0)) {
		this->hero->setOnFloor(false);
	}
}
