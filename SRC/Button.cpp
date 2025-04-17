#include "Button.h"
#include "GLUtils.h"

Button::Button(GLVector2f pos, GLVector2f size, std::string text)
    : mPosition(pos), mSize(size), mText(text) {
}
void Button::Render()
{
    // Convert 3D position to 2D screen space
    GLVector2f screenPos = ConvertToScreenCoords(mPosition);

    glPushMatrix();
    glTranslatef(screenPos.x, screenPos.y, 0);

    // Draw button background
    glBegin(GL_QUADS);
    glColor3fv(mIsHovered ? hoverColor : normalColor);
    glVertex2f(-mWidth / 2, -mHeight / 2);
    glVertex2f(mWidth / 2, -mHeight / 2);
    glVertex2f(mWidth / 2, mHeight / 2);
    glVertex2f(-mWidth / 2, mHeight / 2);
    glEnd();

    // Draw text (pseudo-code)
    DrawTextCentered(mText, 0, 0, textColor);

    glPopMatrix();
}

bool Button::CheckHover(GLVector2f mousePos) {
    mIsHovered = (mousePos.x >= mPosition.x - mSize.x / 2 &&
        mousePos.x <= mPosition.x + mSize.x / 2 &&
        mousePos.y >= mPosition.y - mSize.y / 2 &&
        mousePos.y <= mPosition.y + mSize.y / 2);
    return mIsHovered;
}