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
	mWeaponLevel = 1;
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

	// Weapon level 1 - single shot
	if (mWeaponLevel == 1) {
		FireSingleBullet(0); // Center shot
	}
	// Weapon level 2 - double shot
	else if (mWeaponLevel == 2) {
		FireSingleBullet(-10); // Left offset
		FireSingleBullet(10);  // Right offset
	}
	// Weapon level 3 - triple shot
	else if (mWeaponLevel >= 3) {
		FireSingleBullet(0);   // Center
		FireSingleBullet(-20); // Left
		FireSingleBullet(20);  // Right
	}
}

void Spaceship::FireSingleBullet(float angleOffset)
{
	GLVector3f spaceship_heading(cos(DEG2RAD * (mAngle + angleOffset)),
		sin(DEG2RAD * (mAngle + angleOffset)), 0);
	spaceship_heading.normalize();

	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * 30; // 30 = bullet speed

	shared_ptr<GameObject> bullet(new Bullet(bullet_position, bullet_velocity,
		mAcceleration, mAngle + angleOffset, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	mWorld->AddObject(bullet);
}
bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	// Allow collisions with Asteroids and ExtraLives
	if (o->GetType() != GameObjectType("Asteroid") &&
		o->GetType() != GameObjectType("ExtraLives") &&
		o->GetType() != GameObjectType("BlackHoleCoin"))  {
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