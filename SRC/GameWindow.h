#ifndef __GAMEWINDOW_H__
#define __GAMEWINDOW_H__

#include "GameUtil.h"
#include "GlutWindow.h"
#include "IKeyboardListener.h"
#include "IMouseListener.h"
#include <vector>

class GameWorld;
class GameDisplay;

class GameWindow : public GlutWindow, public IMouseListener
{
public:
	GameWindow(int w, int h, int x, int y, char* t);
	virtual ~GameWindow();

	// Mouse handling
	void OnMouseButton(int button, int state, int x, int y) override;
	void OnMouseMove(int x, int y) {}
	void OnMouseDrag(int x, int y) {}

	// Callback registration
	void AddMouseListener(std::shared_ptr<IMouseListener> listener);
	void RemoveMouseListener(std::shared_ptr<IMouseListener> listener);

	virtual void OnDisplay(void);
	virtual void OnIdle(void);
	virtual void OnWindowReshaped(int w, int h);

	void OnMouseClick(int button, int state, int x, int y);

	void UpdateWorldSize(void);
	void UpdateDisplaySize(void);

	void SetWorld(GameWorld* w);
	GameWorld* GetWorld();

	void SetDisplay(GameDisplay* w);
	GameDisplay* GetDisplay();

protected:
	static const int ZOOM_LEVEL;
	vector<std::shared_ptr<IMouseListener>> mMouseListeners;

	GameWorld* mWorld;
	GameDisplay* mDisplay;
};

#endif