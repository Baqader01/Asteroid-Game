#pragma once
#include "GameObject.h"

class WeaponPowerup : public GameObject
{
public:
    WeaponPowerup();
    virtual ~WeaponPowerup() = default;

    void Update(int t) override;
    bool CollisionTest(shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;
};