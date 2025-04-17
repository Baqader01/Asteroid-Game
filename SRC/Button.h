#pragma once
#include "GLVector.h"
#include <string>
#include <functional>

class Button {
public:
    Button(GLVector2f pos, GLVector2f size, std::string text);

    void Render();
    bool CheckHover(GLVector2f mousePos);
    void SetCallback(std::function<void()> action);

private:
    GLVector2f mPosition;
    GLVector2f mSize;
    std::string mText;
    bool mIsHovered = false;
    std::function<void()> mCallback;
};