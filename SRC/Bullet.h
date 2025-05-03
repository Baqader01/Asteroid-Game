#ifndef __BULLET_H__
#define __BULLET_H__

#include "GameUtil.h"
#include "GameObject.h"

class Bullet : public GameObject
{
public:
	Bullet();
	Bullet(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r, int ttl, bool isSeeking = false);
	Bullet(const Bullet& b);
	virtual ~Bullet(void);

	virtual void Update(int t);

	void SetTarget(shared_ptr<GameObject> target) { mTarget = target; }

	void SetTimeToLive(int ttl) { mTimeToLive = ttl; }
	int GetTimeToLive(void) { return mTimeToLive; }

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

protected:
	bool mIsSeeking = false;
	weak_ptr<GameObject> mTarget;

	int mTimeToLive;
};

#endif