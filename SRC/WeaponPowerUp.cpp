#include "WeaponPowerUp.h"
#include "Spaceship.h"
#include "GameWorld.h"
#include "AnimationManager.h"

WeaponPowerUp::WeaponPowerUp()
    : GameObject("WeaponPowerUp")
{
    // Initialize movement properties 
    mAngle = rand() % 360;
    mRotation = 0;

    // Set random position 
    mPosition.x = rand() / 2;
    mPosition.y = rand() / 2;
    mPosition.z = 0.0;

    // Set velocity 
    mVelocity.x = 10 * cos(DEG2RAD * mAngle);
    mVelocity.y = 10 * sin(DEG2RAD * mAngle);
    mVelocity.z = 0.0;

    // Set up visual appearance
    Animation* anim = AnimationManager::GetInstance().GetAnimationByName("WeaponPowerUp");
    if (anim) {
        SetSprite(make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim));
        SetScale(0.1f);
        SetBoundingShape(make_shared<BoundingSphere>(GetThisPtr(), 1.0f));
    }
}

WeaponPowerUp::~WeaponPowerUp(void) {}

void WeaponPowerUp::Update(int t)
{
    GameObject::Update(t);
    // Add any special update behavior here
}

void WeaponPowerUp::Render(void)
{
    if (mSprite.get()) {
        mSprite->Render();
    }
    GameObject::Render();
}

bool WeaponPowerUp::CollisionTest(shared_ptr<GameObject> o)
{
    if (o->GetType() == GameObjectType("Spaceship")) {
        return GameObject::CollisionTest(o);
    }
    return false;
}

void WeaponPowerUp::OnCollision(const GameObjectList& objects)
{
    for (auto obj : objects) {
        if (obj->GetType() == GameObjectType("Spaceship")) {
            if (auto world = GetWorld()) {
                world->FlagForRemoval(GetThisPtr());
            }
            break;
        }
    }
}