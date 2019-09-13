#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include "Utilities.h"
#include "Appearances.h"

#include <memory>

enum HeroBarrierType { WHITE, BLUE, GREEN, YELLOW, RED };

class Hero : public GameObject {
public:
	Hero();
	Hero(const b2Vec2& pos);

	void setOnFloor(bool value);
	bool isOnFloor();

	void update(double delta) override;

	void setColor(const glm::vec3& color);
	glm::vec3 getColor();

	void setType(HeroBarrierType type);
	HeroBarrierType getType();

	std::shared_ptr<Light> getLight();
private:
	bool m_onFloor;
	glm::vec3 m_color;
	HeroBarrierType m_type;
	std::shared_ptr<Light> m_heroLight;
};

class Reactable {
public:
	virtual void react(Hero* hero) = 0;
};

class Floor : public GameObject {
public:
	Floor(float length, float height);
	Floor(float length, float height, const b2Vec2& pos);

private:

};

class Wall : public GameObject {
public:
	Wall(float width, float height);
	Wall(float width, float height, const b2Vec2& pos);

private:

};

class IronDoor : public GameObject {
public:
	IronDoor();
	IronDoor(const b2Vec2& pos);

	void down();
	void up();
	void setMovingRange(float minY, float maxY);

	void update(double delta) override;
private:
	std::shared_ptr<GameObject> base;
	std::shared_ptr<GameObject> door;
	float move_minY;
	float move_maxY;
};

class Barrier : public GameObject {
public:
	Barrier(float height, const b2Vec2& pos);

	void setColor(const glm::vec3& color);
	glm::vec3 getColor();

	void setType(HeroBarrierType type);
	HeroBarrierType getType();

	void setBlock(bool block);

	void update(double delta) override;

private:
	glm::vec3 m_color;
	std::shared_ptr<BarrierAppear> m_appear;
	std::shared_ptr<PhysicsDef2D> m_phyDef;
	bool m_block;
	HeroBarrierType m_type;
};

class Pedestal : public GameObject, public Reactable {
public:
	Pedestal();
	Pedestal(const b2Vec2& pos);

	void setType(HeroBarrierType type);
	HeroBarrierType getType();

	void react(Hero* hero) override;

private:
	glm::vec3 m_color;
	HeroBarrierType m_type;
};
#endif