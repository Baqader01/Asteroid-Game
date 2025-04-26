#include "BlackHole.h"
#include "Asteroid.h"
#include "GameWorld.h"

BlackHole::BlackHole()
    : GameObject("BlackHole"),
    mGravityRadius(50.0f),
    mGravityForce(25.0f),
    mLifetime(2500) // 2.5 seconds
{
    // Black hole doesn't move
    mVelocity = GLVector3f(0, 0, 0);
}

BlackHole::~BlackHole()
{
}

void BlackHole::Update(int t)
{
    GameObject::Update(t);

    // Reduce lifetime
    mLifetime -= t;
    if (mLifetime <= 0) {
        if (auto world = GetWorld()) {
            world->FlagForRemoval(GetThisPtr());
        }
        return;
    }

    // Apply gravity to nearby objects
    if (auto world = GetWorld()) {
        for (auto obj : world->GetObjects()) {
            if (obj->GetType() == GameObjectType("Asteroid")) {
                GLVector3f dir = mPosition - obj->GetPosition();
                float distance = dir.length();

                if (distance < mGravityRadius && distance > 0) {
                    // Normalize direction and apply force
                    dir = dir.normalize();
                    float force = mGravityForce / distance; // Stronger when closer
                    obj->SetVelocity(obj->GetVelocity() + dir * force);
                }
            }
        }
    }
}

bool BlackHole::CollisionTest(shared_ptr<GameObject> o)
{
    if (o->GetType() == GameObjectType("Asteroid")) {
        return GameObject::CollisionTest(o);
    }
    return false;
}

void BlackHole::OnCollision(const GameObjectList& objects)
{
    for (auto obj : objects) {
        if (obj->GetType() == GameObjectType("Asteroid")) {
            if (auto world = GetWorld()) {
                // Destroy the asteroid
                world->FlagForRemoval(obj);
            }
        }
    }
}