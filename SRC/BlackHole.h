#pragma once
#include "GameObject.h"
#include "GameWorld.h"

class BlackHole : public GameObject
{
public:
    BlackHole();
	virtual ~BlackHole() = default;

    void Update(int t) override;
    bool CollisionTest(shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;

private:
    float mGravityRadius;
    float mGravityForce;
    int mLifetime;
};