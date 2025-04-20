#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include "Button.h"
#include "IButtonListener.h"
#include <vector>

class GameObject;
class Spaceship;
class GUILabel;
class GUIButton;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	
	//mouse click
	void OnMouseButton(int button, int state, int x, int y);
	void OnMouseMove(int x, int y) {};
	void OnMouseDrag(int x, int y) {};
	
	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<Button> mStartButton;
	shared_ptr<Button> mHelpButton;

	std::vector<std::shared_ptr<Button>> mButtons;

	enum class GameState { MENU, PLAYING, GAME_OVER };
	::GameState mCurrentState = ::GameState::MENU; 

	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	
	virtual void OnButtonClick(Button* button);

	// GUI
	void CreateGUI();

	// Menu
	void CreateMenu();
	void ShowMenu();
	void HideMenu();

	// to start fighting the astroids
	void StartGame();

	void CreateAsteroids(const uint num_asteroids);
	shared_ptr<GameObject> CreateExplosion();

	// Screen dimensions
	GLVector2f screenSize = GLVector2f(800.0f, 600.0f);

	// Button dimensions
	GLVector2f buttonSize = GLVector2f(200.0f, 50.0f);

	// Position calculation (top center)
	GLVector2f buttonPos = GLVector2f(screenSize.x / 2, screenSize.y * 0.2f);

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif