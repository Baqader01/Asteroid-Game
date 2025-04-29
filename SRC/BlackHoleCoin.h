#pragma once
#include "GameObject.h"
#include "GameWorld.h"

class BlackHoleCoin : public GameObject
{
public:
    BlackHoleCoin();
    virtual ~BlackHoleCoin();

    GLVector3f GetRandomPosition() {
        return GLVector3f(
            // Set random position 
            mPosition.x = rand() / 2,
            mPosition.y = rand() / 2,
            mPosition.z = 0.0);
    }

    void OnCollision(const GameObjectList& objects) override;
    bool CollisionTest(shared_ptr<GameObject> o) override;
};