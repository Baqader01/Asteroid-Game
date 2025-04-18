#ifndef __IBUTTONLISTENER_H__
#define __IBUTTONLISTENER_H__

class GUIButton;

class IButtonListener
{
public:
    virtual ~IButtonListener() {}
    virtual void OnButtonClick(GUIButton* button) = 0;
};

#endif