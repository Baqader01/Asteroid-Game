#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"

class Asteroid : public GameObject
{
public:
	Asteroid();
	~Asteroid(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

	bool IsMenuAsteroid() const { return mIsMenuAsteroid; }
	void SetIsMenuAsteroid(bool isMenuAsteroid) { mIsMenuAsteroid = isMenuAsteroid; }

private:
	bool mIsMenuAsteroid;
};

#endif
