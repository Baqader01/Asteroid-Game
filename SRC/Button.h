#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "GUIComponent.h"
#include <functional>
#include <string>

class Button : public GUIComponent
{
public:
    Button(const string& label = "");
    virtual ~Button();

    virtual void Draw() override;

    void SetLabel(const string& label) { mLabel = label; }
    string GetLabel() const { return mLabel; }

    void SetHorizontalAlignment(GUIHorizontalAlignment alignment);
    void SetVerticalAlignment(GUIVerticalAlignment alignment);

    void SetClickListener(std::function<void()> callback);

    //to interact with the buttons
    void OnMouseClick(int x, int y);

    bool IsPointInside(int x, int y) const;

protected:
    string mLabel;
    std::function<void()> mClickCallback;
    bool mIsPressed = false;
};

#endif