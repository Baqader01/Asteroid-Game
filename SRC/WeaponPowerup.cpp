#include "WeaponPowerup.h"
#include "Spaceship.h"
#include "GameWorld.h"

WeaponPowerup::WeaponPowerup() : GameObject("WeaponPowerup")
{
    // Basic movement 
    mAngle = static_cast<float>(rand() % 360);
    mPosition = GLVector3f(rand() % 100 - 50, rand() % 100 - 50, 0);
    mVelocity = GLVector3f(2.0f * cos(DEG2RAD * mAngle),
        2.0f * sin(DEG2RAD * mAngle),
        0.0f);
}

void WeaponPowerup::Update(int t)
{
    GameObject::Update(t);
    // Gentle floating motion
    mVelocity.x = 2.0f * cos(DEG2RAD * (mAngle + t / 100.0f));
    mVelocity.y = 2.0f * sin(DEG2RAD * (mAngle + t / 100.0f));
}

bool WeaponPowerup::CollisionTest(shared_ptr<GameObject> o)
{
    return o->GetType() == GameObjectType("Spaceship") &&
        GameObject::CollisionTest(o);
}

void WeaponPowerup::OnCollision(const GameObjectList& objects)
{
    for (auto obj : objects) {
        if (obj->GetType() == GameObjectType("Spaceship")) {
            if (auto world = GetWorld()) {
                world->FlagForRemoval(GetThisPtr());
            }
            break;
        }
    }
}