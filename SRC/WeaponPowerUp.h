#ifndef BULLET_POWERUP_H
#define BULLET_POWERUP_H

#include "GameObject.h"
#include "Animation.h"
#include "BoundingSphere.h"

class WeaponPowerUp : public GameObject
{
public:
    WeaponPowerUp();
    virtual ~WeaponPowerUp(void);

    virtual void Update(int t);
    virtual void Render(void);
    virtual bool CollisionTest(shared_ptr<GameObject> o);
    virtual void OnCollision(const GameObjectList& objects);
};

#endif


