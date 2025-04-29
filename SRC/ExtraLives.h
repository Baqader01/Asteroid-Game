#ifndef __EXTRALIVES_H__
#define __EXTRALIVES_H__

#include "GameObject.h"

class ExtraLives : public GameObject
{
public:
    ExtraLives();
    bool CollisionTest(shared_ptr<GameObject> o) override;
    void OnCollision(const GameObjectList& objects) override;
};
#endif