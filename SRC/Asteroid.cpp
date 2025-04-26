#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"

Asteroid::Asteroid() : GameObject("Asteroid")
{
	mIsMenuAsteroid = false;
	mAngle = rand() % 360;
	mRotation = 0; // rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 10.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 10.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

Asteroid::~Asteroid(void)
{
}

bool Asteroid::CollisionTest(shared_ptr<GameObject> o)
{
    // Menu asteroids don't collide with anything
    if (mIsMenuAsteroid) return false;

	if (GetType() == o->GetType()) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Asteroid::OnCollision(const GameObjectList& objects)
{
	// Find the spaceship in the collided objects
	for (auto obj : objects) {
        // Only destroy asteroid if colliding with a bullet or a black hole
        if (obj->GetType() == GameObjectType("Bullet") || obj->GetType() == GameObjectType("BlackHole")){
			if (auto world = GetWorld()) {
				world->FlagForRemoval(GetThisPtr());
			}
			break;  // Exit after handling
		}
	}
}
