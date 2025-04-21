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

    // Retro Asteroids color scheme
    mNormalColor = GLVector3f(0.1f, 0.1f, 0.8f);   // Deep blue
    mHoverColor = GLVector3f(0.3f, 0.3f, 1.0f);    // Bright blue
    mPressedColor = GLVector3f(0.0f, 0.0f, 0.6f);  // Dark blue

    // Draw button outline
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // White border
    glBegin(GL_LINE_LOOP);
    glVertex2i(mPosition.x - 1, mPosition.y - 1);
    glVertex2i(mPosition.x + mSize.x + 1, mPosition.y - 1);
    glVertex2i(mPosition.x + mSize.x + 1, mPosition.y + mSize.y + 1);
    glVertex2i(mPosition.x - 1, mPosition.y + mSize.y + 1);
    glEnd();

    // Draw button face
    GLVector3f bgColor = mIsPressed ? mPressedColor :
        (mIsHovered ? mHoverColor : mNormalColor);
    glColor3f(bgColor.x, bgColor.y, bgColor.z);
    glBegin(GL_QUADS);
    glVertex2i(mPosition.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y + mSize.y);
    glVertex2i(mPosition.x, mPosition.y + mSize.y);
    glEnd();

    // PERFECT CENTERING CALCULATION
    int textWidth = 0;
    for (const char& c : mLabel) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Horizontal center
    int textX = mPosition.x + (mSize.x - textWidth) / 2;

    // Vertical center (key fix)
    int fontHeight = 12; // GLUT_BITMAP_HELVETICA_18 is actually 12px tall
    int textY = mPosition.y + (mSize.y / 2) + (fontHeight / 2) - 2; // -2 is the magic number

    // Draw text with outline
    glColor3f(0.0f, 0.0f, 0.0f); // Black outline
    glRasterPos2i(textX + 1, textY - 1);
    for (const char& c : mLabel) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Main text (white)
    glColor3f(1.0f, 1.0f, 1.0f);
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
}

void Button::SetVerticalAlignment(GUIVerticalAlignment alignment)
{
    mVerticalAlignment = alignment;
}