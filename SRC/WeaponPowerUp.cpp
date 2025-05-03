#include "WeaponPowerup.h"
#include "Spaceship.h"
#include "GameWorld.h"

WeaponPowerup::WeaponPowerup() : GameObject("WeaponPowerup")
{
    // Initialise movement 
    mAngle = rand() % 360;
    mRotation = 0; 
    mPosition.x = rand() / 2;
    mPosition.y = rand() / 2;
    mPosition.z = 0.0;
    mVelocity.x = 10.0 * cos(DEG2RAD * mAngle);
    mVelocity.y = 10.0 * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;
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
