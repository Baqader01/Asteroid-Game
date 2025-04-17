#include "Button.h"

Button::Button(const char* type_name, GLVector3f position, std::string text)
    : MenuObject(type_name, position), mText(text) {
}

void Button::HandleHover(const GLVector2f& mousePos) {
    // Convert 3D position to 2D screen space if needed
    mIsHovered = (mousePos.x >= mScreenPosition.x - mWidth / 2 &&
        mousePos.x <= mScreenPosition.x + mWidth / 2 &&
        mousePos.y >= mScreenPosition.y - mHeight / 2 &&
        mousePos.y <= mScreenPosition.y + mHeight / 2);
}

void Button::HandleClick() {
    if (mIsHovered) {
        // Trigger assigned action
        if (mCallback) mCallback();
    }
}

void Button::Render() {
    GameObject::PreRender();

    // Button-specific rendering
    glBegin(GL_QUADS);
    glVertex2f(-mWidth / 2, -mHeight / 2);
    glVertex2f(mWidth / 2, -mHeight / 2);
    glVertex2f(mWidth / 2, mHeight / 2);
    glVertex2f(-mWidth / 2, mHeight / 2);
    glEnd();

    GameObject::PostRender();
}