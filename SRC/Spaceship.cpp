#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0)
{
}

/** Copy constructor. */
Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0)
{
}

/** Destructor. */
Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void Spaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);

	// Update powerup timer
	if (mHasMissilePowerup) {
		mPowerupTimer -= t;
		if (mPowerupTimer <= 0) {
			mHasMissilePowerup = false;
		}
	}
}

/** Render this spaceship. */
void Spaceship::Render(void)
{
	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();

	// If ship is thrusting
	if ((mThrust > 0) && (mThrusterShape.get() != NULL)) {
		mThrusterShape->Render();
	}

	GameObject::Render();
}

/** Fire the rockets. */
void Spaceship::Thrust(float t)
{
	mThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mThrust*cos(DEG2RAD*mAngle);
	mAcceleration.y = mThrust*sin(DEG2RAD*mAngle);
}

/** Set the rotation. */
void Spaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet. */
void Spaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;

	// Calculate direction
	float angleRad = DEG2RAD * mAngle;
	GLVector3f direction(cos(angleRad), sin(angleRad), 0);
	GLVector3f bulletPos = mPosition + (direction * 4);
	GLVector3f bulletVel = mVelocity + (direction * 30.0f);

	bool isSeeking = mHasMissilePowerup;
	shared_ptr<GameObject> target = isSeeking ? FindNearestAsteroid() : nullptr;

	auto bullet = make_shared<Bullet>(bulletPos, bulletVel, mAcceleration,
		mAngle, 0, 2000, isSeeking);

	if (isSeeking && target) {
		bullet->SetTarget(target);
	}

	// Create bullet
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	mWorld->AddObject(bullet);
}

shared_ptr<GameObject> Spaceship::FindNearestAsteroid()
{
	if (!mWorld) return nullptr; // Safety check

	shared_ptr<GameObject> nearestAsteroid;
	float closestDistance = FLT_MAX; // Start with maximum possible distance

	// Get all game objects from the world
	const GameObjectList& gameObjects = mWorld->GetGameObjects();

	for (auto obj : gameObjects) {
		if (obj->GetType() == GameObjectType("Asteroid")) {
			// Calculate distance between ship and asteroid
			float distance = (obj->GetPosition() - mPosition).length();

			// Check if this is the closest so far
			if (distance < closestDistance) {
				closestDistance = distance;
				nearestAsteroid = obj;
			}
		}
	}

	return nearestAsteroid;
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	// Allow collisions with Asteroids and ExtraLives
	if (o->GetType() != GameObjectType("Asteroid") &&
		o->GetType() != GameObjectType("ExtraLives") && 
		o->GetType() != GameObjectType("WeaponPowerup") &&
		o->GetType() != GameObjectType("BlackHoleCoin")) 
	{
		return false;
	}

	if (!mBoundingShape || !o->GetBoundingShape()) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList& objects)
{
	for (auto obj : objects) {
		if (obj->GetType() == GameObjectType("Asteroid")) {
			// Only destroy ship when hitting asteroids
			mWorld->FlagForRemoval(GetThisPtr());
			break;
		}
	}
}