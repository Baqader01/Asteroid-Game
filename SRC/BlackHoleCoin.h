#pragma once
#include "GameObject.h"
#include "GameWorld.h"

class BlackHoleCoin : public GameObject
{
public:
    BlackHoleCoin();
    virtual ~BlackHoleCoin();

    void OnCollision(const GameObjectList& objects) override;
    bool CollisionTest(shared_ptr<GameObject> o) override;
};