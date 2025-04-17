#include "ExtraLifePowerUp.h"
#include "Player.h"

ExtraLifePowerUp::ExtraLifePowerUp()
    : GameObject("ExtraLifePowerUp"),
    PowerUp()  // Initialize PowerUp base if needed
{
    // Set power-up properties
    mBoundingShape = make_shared<BoundingSphere>(this, 3.0f);  
    // Set visual properties 
}

void ExtraLifePowerUp::ApplyEffect(Player& player) {
        player.IncreaseLives();
}