#include "GameWorld.h"
#include "Bullet.h"
#include "BoundingSphere.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Bullets live for 2s by default. */
Bullet::Bullet()
	: GameObject("Bullet"), mTimeToLive(2000)
{
}

Bullet::Bullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl, bool isSeeking)
	: GameObject("Bullet", p, v, a, h, r), mTimeToLive(ttl), mIsSeeking(isSeeking)
{
}

/** Construct a new bullet with given position, velocity, acceleration, angle, rotation and lifespan. */


/** Copy constructor. */
Bullet::Bullet(const Bullet& b)
	: GameObject(b),
	mTimeToLive(b.mTimeToLive)
{
}

/** Destructor. */
Bullet::~Bullet(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update bullet, removing it from game world if necessary. */
void Bullet::Update(int t)
{
	// Update position/velocity
	GameObject::Update(t);
	// Reduce time to live
	mTimeToLive = mTimeToLive - t;
	// Ensure time to live isn't negative
	if (mTimeToLive < 0) { mTimeToLive = 0; }
	// If time to live is zero then remove bullet from world
	if (mTimeToLive == 0) {
		if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
	}

	if (mIsSeeking) {
		if (auto target = mTarget.lock()) {  // Get shared_ptr if target exists
			GLVector3f toTarget = target->GetPosition() - mPosition;
			float distance = toTarget.length();
			if (distance > 0) {
				mVelocity += (toTarget / distance) * 0.5f; // Gentle seeking
			}
		}
	}

}

bool Bullet::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Bullet::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}