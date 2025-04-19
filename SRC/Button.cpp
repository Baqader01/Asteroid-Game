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

    // Calculate text position (centered within button)
    int textWidth = 0;
    for (const char& c : mLabel) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);
    }

    int textX = mPosition.x + (mSize.x - textWidth) / 2;
    int textY = mPosition.y + 18 + 5; // Font height + padding

    // Draw centered text
    glColor3f(mColor.x, mColor.y, mColor.z);
    glRasterPos2i(textX, textY);

    for (const char& c : mLabel) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopAttrib();
}

bool Button::IsPointInside(int x, int y) const {
    return x >= mPosition.x &&
        x <= mPosition.x + mSize.x &&
        y >= mPosition.y &&
        y <= mPosition.y + mSize.y;
}

void Button::OnMouseClick(int x, int y)
{
    bool inside = (x >= mPosition.x) && (x <= mPosition.x + mSize.x) &&
        (y >= mPosition.y) && (y <= mPosition.y + mSize.y);

    std::cout << "Button \"" << mLabel << "\" click test: "
        << "Mouse(" << x << "," << y << ") "
        << "Button(" << mPosition.x << "," << mPosition.y << ")-("
        << mPosition.x + mSize.x << "," << mPosition.y + mSize.y << ") "
        << "Inside: " << inside << std::endl;

    if (inside && mClickCallback) {
        std::cout << "Triggering button callback" << std::endl;
        mClickCallback();
    }
}

void Button::SetClickListener(std::function<void()> callback) {
    mClickCallback = callback;
}
void Button::SetHorizontalAlignment(GUIHorizontalAlignment alignment) {
    mHorizontalAlignment = alignment;
}

void Button::SetVerticalAlignment(GUIVerticalAlignment alignment) {
    mVerticalAlignment = alignment;
}



