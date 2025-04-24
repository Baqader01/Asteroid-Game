#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "GUIComponent.h"
#include "IMouseListener.h"
#include <functional>
#include <string>

class Button : public GUIComponent, public IMouseListener
{
public:
    Button(const string& label);
    virtual ~Button();

     void SetActive(bool active) { mActive = active; }
     bool IsActive() { return mActive; }

    virtual void Draw() override;

    void SetLabel(const string& label) { mLabel = label; }
    string GetLabel() const { return mLabel; }

    void SetHorizontalAlignment(GUIHorizontalAlignment alignment);
    void SetVerticalAlignment(GUIVerticalAlignment alignment);

    void SetClickListener(function<void()> callback) { mClickCallback = callback; }

    // IMouseListener implementation
    void OnMouseClick(int b, int state, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseDrag(int x, int y) override;

    // Visual customization
    void SetNormalColor(const GLVector3f& color) { mNormalColor = color; }
    void SetHoverColor(const GLVector3f& color) { mHoverColor = color; }
    void SetPressedColor(const GLVector3f& color) { mPressedColor = color; }
    void SetTextColor(const GLVector3f& color) { mTextColor = color; }

    void SetScreenSize(int width, int height) {
        mScreenWidth = width;
        mScreenHeight = height;
    }

protected:

    bool IsPointInside(int x, int y) const;
    bool mActive;

    string mLabel;
    function<void()> mClickCallback;

    int mScreenHeight = 600;
    int mScreenWidth = 800;

    // Visual states
    bool mIsHovered = false;
    bool mIsPressed = false;

    // Colors
    GLVector3f mNormalColor = GLVector3f(0.2f, 0.2f, 0.2f);
    GLVector3f mHoverColor = GLVector3f(0.3f, 0.3f, 0.3f);
    GLVector3f mPressedColor = GLVector3f(0.1f, 0.1f, 0.1f);
    GLVector3f mTextColor = GLVector3f(1.0f, 1.0f, 1.0f);
};

#endif