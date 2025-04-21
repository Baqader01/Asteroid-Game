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

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);

    // Determine button color based on state
    GLVector3f bgColor;
    if (mIsPressed) {
        bgColor = mPressedColor;
    }
    else if (mIsHovered) {
        bgColor = mHoverColor;
    }
    else {
        bgColor = mNormalColor;
    }

    // Draw button background with current state color
    glColor3f(bgColor.x, bgColor.y, bgColor.z);
    glBegin(GL_QUADS);
    glVertex2i(mPosition.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y + mSize.y);
    glVertex2i(mPosition.x, mPosition.y + mSize.y);
    glEnd();

    // Draw button border (lighter when hovered)
    glColor3f(bgColor.x + 0.3f, bgColor.y + 0.3f, bgColor.z + 0.3f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(mPosition.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y + mSize.y);
    glVertex2i(mPosition.x, mPosition.y + mSize.y);
    glEnd();

    // Calculate text position (centered within button)
    int textWidth = 0;
    for (const char& c : mLabel) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
    }

    int textX = mPosition.x + (mSize.x - textWidth) / 2;
    int textY = mPosition.y + (mSize.y / 2) + 6; // Better vertical centering

    // Draw text with current text color
    glColor3f(mTextColor.x, mTextColor.y, mTextColor.z);
    glRasterPos2i(textX, textY);
    for (const char& c : mLabel) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopAttrib();
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
    // You might want to recalculate position here
}

void Button::SetVerticalAlignment(GUIVerticalAlignment alignment)
{
    mVerticalAlignment = alignment;
    // You might want to recalculate position here
}