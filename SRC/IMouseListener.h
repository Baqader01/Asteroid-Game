#ifndef __IMOUSELISTENER_H__
#define __IMOUSELISTENER_H__

class IMouseListener {
public:
    virtual void OnMouseClick(int button, int state, int x, int y) = 0;
    virtual void OnMouseMove(int x, int y) = 0;
    virtual void OnMouseDrag(int x, int y) = 0;
};

#endif
