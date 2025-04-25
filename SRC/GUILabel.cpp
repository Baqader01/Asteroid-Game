#include "GUILabel.h"
#include <GL/glut.h>

GUILabel::GUILabel()
    : mText(""), mFontSize(15), mFontWidth(9), mFontHeight(15)
{
}

GUILabel::GUILabel(const std::string& text)
    : mText(text), mFontSize(15), mFontWidth(9), mFontHeight(15)
{
}

GUILabel::~GUILabel()
{
}

void* GUILabel::GetGLUTFont() const
{
    switch (mFontSize) {
    case 8:  return GLUT_BITMAP_8_BY_13;
    case 9:  return GLUT_BITMAP_9_BY_15;
    case 10: return GLUT_BITMAP_TIMES_ROMAN_10;
    case 12: return GLUT_BITMAP_HELVETICA_12;
    case 18: return GLUT_BITMAP_HELVETICA_18;
    case 24: return GLUT_BITMAP_TIMES_ROMAN_24;
    default: return GLUT_BITMAP_9_BY_15; // Default font
    }
}

void GUILabel::SetFontSize(int size)
{
    mFontSize = size;

    // Update font metrics based on size
    switch (size) {
    case 8:
        mFontWidth = 8;
        mFontHeight = 13;
        break;
    case 9:
        mFontWidth = 9;
        mFontHeight = 15;
        break;
    case 10:
        mFontWidth = 10;
        mFontHeight = 16;
        break;
    case 12:
        mFontWidth = 12;
        mFontHeight = 18;
        break;
    case 18:
        mFontWidth = 18;
        mFontHeight = 24;
        break;
    case 24:
        mFontWidth = 24;
        mFontHeight = 32;
        break;
    default:
        mFontWidth = 9 + (size - 9) * 0.8f; // Approximate for other sizes
        mFontHeight = 15 + (size - 9) * 1.2f;
    }
}

GLVector2i GUILabel::GetPreferredSize()
{
    void* font = GetGLUTFont();
    int width = glutBitmapLength(font, (const unsigned char*)mText.c_str());
    return GLVector2i(width, mFontHeight);
}

void GUILabel::Draw()
{
    if (!mVisible || mText.empty()) return;

    void* font = GetGLUTFont();

    // Calculate text dimensions
    int textWidth = glutBitmapLength(font, (const unsigned char*)mText.c_str());
    int textHeight = mFontHeight;

    // Calculate alignment offsets
    int align_x = 0;
    int align_y = 0;

    if (mHorizontalAlignment == GUI_HALIGN_RIGHT) {
        align_x = -textWidth;
    }
    else if (mHorizontalAlignment == GUI_HALIGN_CENTER) {
        align_x = -textWidth / 2;
    }

    if (mVerticalAlignment == GUI_VALIGN_TOP) {
        align_y = -textHeight;
    }
    else if (mVerticalAlignment == GUI_VALIGN_MIDDLE) {
        align_y = -textHeight / 2;
    }

    // Set drawing position
    int x = mPosition.x + mBorder.x + align_x;
    int y = mPosition.y + mBorder.y + align_y;

    // Draw the text
    glDisable(GL_LIGHTING);
    glColor3f(mColor[0], mColor[1], mColor[2]);
    glRasterPos2i(x, y);

    for (char c : mText) {
        glutBitmapCharacter(font, c);
    }

    glEnable(GL_LIGHTING);
}