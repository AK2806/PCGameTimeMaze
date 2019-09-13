#ifndef GAMESCENES_H
#define GAMESCENES_H

#include "Utilities.h"
#include "GameObjects.h"

#include <memory>
#include <vector>

using namespace std;

class MasterScene final : public Scene {
public:
	void init() override;
	void update(double delta) override;

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	void EndContact(b2Contact* contact) override;

private:
	vector< shared_ptr<Barrier> > barriers;
	shared_ptr<Hero> hero;
	shared_ptr<Wall> wall;
	shared_ptr<Floor> floor;
	shared_ptr<Floor> floor2;
	shared_ptr<Floor> floor3;
	shared_ptr<Pedestal> pedestal;
};

#endif
