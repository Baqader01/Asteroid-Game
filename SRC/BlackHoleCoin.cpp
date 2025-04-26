#include "BlackHoleCoin.h"
#include "Spaceship.h"
#include "Asteroids.h"
#include "BlackHole.h"

BlackHoleCoin::BlackHoleCoin()
    : GameObject("BlackHoleCoin")
{
    // Initialize movement properties 
    mAngle = rand() % 360;
    mRotation = 0;

    GetRandomPosition();

    // Set velocity 
    mVelocity.x = 10 * cos(DEG2RAD * mAngle);
    mVelocity.y = 10 * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;
}

BlackHoleCoin::~BlackHoleCoin()
{
}

bool BlackHoleCoin::CollisionTest(shared_ptr<GameObject> o)
{
    if (o->GetType() == GameObjectType("Spaceship")) {
        return GameObject::CollisionTest(o);
    }
    return false;
}

void BlackHoleCoin::OnCollision(const GameObjectList& objects)
{
    // Find the spaceship in the collided objects
    for (auto obj : objects) {
        if (obj->GetType() == GameObjectType("Spaceship")){
            // Remove the blackhole object 
            if (auto world = GetWorld()) {
                world->FlagForRemoval(GetThisPtr());
            }
            break;  // Exit after handling
        }
    }
}