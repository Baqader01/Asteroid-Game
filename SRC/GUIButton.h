#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GUIComponent.h"
#include <functional>
#include <string>

class GUIButton : public GUIComponent
{
public:
    GUIButton(const string& label = "");
    virtual ~GUIButton();

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