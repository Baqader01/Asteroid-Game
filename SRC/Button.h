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
    void OnMouseClick(int x, int y);

    void SetLabel(const string& label) { mLabel = label; }
    string GetLabel() const { return mLabel; }

    void SetClickListener(std::function<void()> callback) { mClickCallback = callback; }

protected:
    string mLabel;
    function<void()> mClickCallback;
};

#endif