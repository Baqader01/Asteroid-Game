#ifndef __GUILABEL_H__
#define __GUILABEL_H__

#include "GUIComponent.h"
#include <string>

class GUILabel : public GUIComponent
{
public:
    GUILabel();
    GUILabel(const std::string& text);
    virtual ~GUILabel();

    void Draw() override;
    GLVector2i GetPreferredSize() override;

    void SetText(const std::string& text) { mText = text; }
    std::string GetText() const { return mText; }

    void SetFontSize(int size);
    int GetFontSize() const { return mFontSize; }

private:
    std::string mText;
    int mFontSize;
    int mFontWidth;
    int mFontHeight;

    void* GetGLUTFont() const;
};

#endif