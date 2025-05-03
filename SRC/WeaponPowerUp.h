#pragma once
#include "GameObject.h"

class WeaponPowerup : public GameObject
{
public:
    WeaponPowerup();
    virtual ~WeaponPowerup() = default;

    bool CollisionTest(shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;
};