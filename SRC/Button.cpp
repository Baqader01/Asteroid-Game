#include "Button.h"
#include "GameWindow.h"
#include <GL/glut.h>
#include <iostream>

Button::Button(const std::string& label)
    : mLabel(label), mClickCallback(nullptr)
{
    // Set default button size
    mSize = GLVector2i(200, 40); // More standard button size
}

Button::~Button()
{
}

void Button::Draw()
{
    if (!mVisible) return;

    // Determine button background color based on state
    GLVector3f bgColor = mNormalColor;
    if (mIsPressed)
        bgColor = mPressedColor;
    else if (mIsHovered)
        bgColor = mHoverColor;

    // Draw button background
    glColor3f(bgColor.x, bgColor.y, bgColor.z);
    glBegin(GL_QUADS);
        glVertex2i(mPosition.x, mPosition.y);
        glVertex2i(mPosition.x + mSize.x, mPosition.y);
        glVertex2i(mPosition.x + mSize.x, mPosition.y + mSize.y);
        glVertex2i(mPosition.x, mPosition.y + mSize.y);
    glEnd();

    // Draw text label in center
    glColor3f(mTextColor.x, mTextColor.y, mTextColor.z);

    void* font = GLUT_BITMAP_HELVETICA_18; // Choose appropriate font

    int textWidth = 0;
    for (char c : mLabel) {
        textWidth += glutBitmapWidth(font, c);
    }

    int textHeight = 18; // Approx height of HELVETICA_18

    int textX = mPosition.x + (mSize.x - textWidth) / 2;
    int textY = mPosition.y + (mSize.y - textHeight) / 2 + 4; // +4 offset for vertical visual balance

    glRasterPos2i(textX, textY);
    for (char c : mLabel) {
        glutBitmapCharacter(font, c);
    }
}


bool Button::IsPointInside(int x, int y) const
{
    // Convert y-coordinate from top-left to bottom-left origin
    y = mScreenHeight - y;

    return x >= mPosition.x &&
        x <= mPosition.x + mSize.x &&
        y >= mPosition.y &&
        y <= mPosition.y + mSize.y;
}

void Button::OnMouseClick(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON) return;

    bool inside = IsPointInside(x, y);

    if (state == GLUT_DOWN) {
        mIsPressed = inside;
    }
    else if (state == GLUT_UP && inside && mIsPressed) {
        if (mClickCallback) mClickCallback();
        mIsPressed = false;
    }
}

void Button::OnMouseMove(int x, int y)
{
    mIsHovered = IsPointInside(x, y);

    // Cancel press if mouse leaves while pressed
    if (mIsPressed && !mIsHovered) {
        mIsPressed = false;
    }
}


void Button::OnMouseDrag(int x, int y)
{
}

void Button::SetHorizontalAlignment(GUIHorizontalAlignment alignment)
{
    mHorizontalAlignment = alignment;
}

void Button::SetVerticalAlignment(GUIVerticalAlignment alignment)
{
    mVerticalAlignment = alignment;
}