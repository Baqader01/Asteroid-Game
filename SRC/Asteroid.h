#ifndef __ASTEROID_H__
#define __ASTEROID_H__

#include "GameObject.h"

class Asteroid : public GameObject
{
public:
	Asteroid(bool isMenuAsteroid = false);
	~Asteroid(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

	bool IsMenuAsteroid() const { return mIsMenuAsteroid; }

private:
	bool mIsMenuAsteroid;
};

#endif
