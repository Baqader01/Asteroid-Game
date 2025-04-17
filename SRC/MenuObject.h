#pragma once
#include "GameObject.h"

class MenuObject : public GameObject {
public:
    MenuObject(const char* type_name, GLVector3f position);

    virtual void HandleHover(const GLVector2f& mousePos) = 0;
    virtual void HandleClick() = 0;
    bool IsHovered() const { return mIsHovered; }

protected:
    bool mIsHovered = false;
    GLVector2f mScreenPosition; // 2D menu position
};