#include "Button.h"
#include "GameUtil.h"
#include <GL/glut.h>

Button::Button(const std::string& label)
    : mLabel(label), mClickCallback(nullptr)
{
    // Set default button size
    mSize = GLVector2i(100, 30);
    // Set default color (white)
    mColor = GLVector3f(1.0f, 1.0f, 1.0f);
}

Button::~Button()
{
}

void Button::Draw()
{
    if (!mVisible) return;

    glPushAttrib(GL_CURRENT_BIT);

    // Draw button background
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2i(mPosition.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y + mSize.y);
    glVertex2i(mPosition.x, mPosition.y + mSize.y);
    glEnd();

    // Draw button border
    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(mPosition.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y);
    glVertex2i(mPosition.x + mSize.x, mPosition.y + mSize.y);
    glVertex2i(mPosition.x, mPosition.y + mSize.y);
    glEnd();

    // Calculate text position for perfect centering
    int textWidth = 0;
    for (const char& c : mLabel) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
    }

    int horizontalPadding = (mSize.x - textWidth) / 2;
    int verticalPadding = (mSize.y - 18) / 2; // 18 is font height

    // Draw centered text
    glColor3f(mColor.x, mColor.y, mColor.z);
    glRasterPos2i(mPosition.x + horizontalPadding,
        mPosition.y + verticalPadding + 15); // +15 for baseline

    for (const char& c : mLabel) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopAttrib();
}

void Button::OnMouseClick(int x, int y)
{
    if (!mVisible) return;

    if (x >= mPosition.x && x <= mPosition.x + mSize.x &&
        y >= mPosition.y && y <= mPosition.y + mSize.y) {
        if (mClickCallback) {
            mClickCallback();
        }
    }
}