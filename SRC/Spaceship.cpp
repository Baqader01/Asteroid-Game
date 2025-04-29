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

	if (mHasMissilePowerup) {
		// Create seeking missile
		// 
		// Calculate forward direction
		GLVector3f direction(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
		direction.normalize();

		// Create missile slightly ahead of ship
		GLVector3f missile_pos = mPosition + (direction * 4);
		GLVector3f missile_vel = mVelocity + direction * 30.0f;

		// Find nearest asteroid
		shared_ptr<GameObject> target;
		float closest_dist = FLT_MAX;
		for (auto obj : mWorld->GetGameObjects()) {
			if (obj->GetType() == GameObjectType("Asteroid")) {
				float dist = (obj->GetPosition() - mPosition).length();
				if (dist < closest_dist) {
					closest_dist = dist;
					target = obj;
				}
			}
		}

		// Add seeking behavior if target found
		if (target) {
			GLVector3f to_target = target->GetPosition() - missile_pos;
			float distance = to_target.length();

			// Only normalize if not zero vector
			if (distance > 0) {
				to_target /= distance;  // Manual normalization
				missile_vel += to_target * 60.0f; // Add seeking force
			}
		}

		// Create missile
		shared_ptr<GameObject> missile(new Bullet(missile_pos, missile_vel,
			mAcceleration, mAngle, 0, 2000));
		missile->SetBoundingShape(make_shared<BoundingSphere>(missile->GetThisPtr(), 2.0f));
		missile->SetShape(mBulletShape);
		mWorld->AddObject(missile);

	}
	else {
		// Normal bullet

		// Calculate bullet direction 
		GLVector3f bullet_direction(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
		bullet_direction.normalize();

		// Calculate bullet position and velocity
		GLVector3f bullet_position = mPosition + (bullet_direction * 4);
		GLVector3f bullet_velocity = mVelocity + bullet_direction * 30.0f;

		// Create bullet
		shared_ptr<GameObject> bullet(new Bullet(bullet_position, bullet_velocity,
			mAcceleration, mAngle, 0, 2000));
		bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
		bullet->SetShape(mBulletShape);
		mWorld->AddObject(bullet);
	}
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