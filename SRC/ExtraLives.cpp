#include "ExtraLives.h"
#include "Spaceship.h"
#include "Player.h"

ExtraLives::ExtraLives()
    : GameObject("ExtraLives")
{
    // Initialize movement properties 
    mAngle = rand() % 360;
    mRotation = 0;

    // Set random position 
    mPosition.x = rand() / 2; 
    mPosition.y = rand() / 2;
    mPosition.z = 0.0;

    // Set velocity 
    mVelocity.x = 10 * cos(DEG2RAD * mAngle);
    mVelocity.y = 10 * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;

}


bool ExtraLives::CollisionTest(shared_ptr<GameObject> o)
{
    if (o->GetType() == GameObjectType("Spaceship")) {
        return GameObject::CollisionTest(o);
    }
    return false;
}

void ExtraLives::OnCollision(const GameObjectList& objects)
{
    // Find the spaceship in the collided objects
    for (auto obj : objects) {
        if (!(obj->GetType() == GameObjectType("Asteroid"))) {
            // Remove the ExtraLives object 
            if (auto world = GetWorld()) {
                world->FlagForRemoval(GetThisPtr());
            }
            break;  // Exit after handling
        }
    }


}