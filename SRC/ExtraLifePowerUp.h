#ifndef __EXTRALIFEPOWERUP_H__
#define __EXTRALIFEPOWERUP_H__

//my class
#include "GameWorld.h"
#include "PowerUp.h"
#include "BoundingSphere.h"
#include "Player.h"

class ExtraLifePowerUp : public PowerUp {
public:
	ExtraLifePowerUp();
	virtual ~ExtraLifePowerUp() = default;

	//adding an increase lives function
	void Apply() override {
		player.IncreaseLives();
	};

private:
	Player player;
};

#endif