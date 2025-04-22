#include <stdlib.h>
#include "GameUtil.h"
#include "Asteroid.h"
#include "BoundingShape.h"

Asteroid::Asteroid(bool isMenuAsteroid) : GameObject("Asteroid"), mIsMenuAsteroid(isMenuAsteroid)
{
    mAngle = rand() % 360;
    mRotation = 0;
    mPosition.x = rand() % 100 - 50;
    mPosition.y = rand() % 100 - 50;
    mPosition.z = 0.0;

    // Slower movement for menu asteroids
    float speed = isMenuAsteroid ? 1.5f : 10.0f;
    mVelocity.x = speed * cos(DEG2RAD * mAngle);
    mVelocity.y = speed * sin(DEG2RAD * mAngle);
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
    // Menu asteroids can't be destroyed
    if (!mIsMenuAsteroid) {
        mWorld->FlagForRemoval(GetThisPtr());
    }

	mWorld->FlagForRemoval(GetThisPtr());
}
