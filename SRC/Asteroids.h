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
#include "GlutSession.h"
#include <vector>

class GameObject;
class Spaceship;
class GUILabel;
class GUIButton;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener, public enable_shared_from_this<Asteroids>
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	enum class GameState {
		MENU,
		IN_GAME,
		INSTRUCTIONS,
		HIGH_SCORES,
		GAME_OVER
	};

	void CreateAsteroids(int count, bool forMenu);
	void DeleteAllAsteroids();

	void SetupInputListeners();
	
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

	// GUI
	void CreateGUI();

	void InitializeResources();

	// Menu
	void ReturnToMenu();

	void CreateMenu();
	void ShowMenu();
	void HideMenu();
	void DrawMenuBackground();

	void ShowInstructions();
	void HideInstructions();


	bool HandleEscapeKey();

private:
	GameState mCurrentState;

	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mInstructionsLabel;

	shared_ptr<Asteroids> mSelfPtr; // Prevents deletion while listening

	vector<shared_ptr<GUILabel>> mMenuLabels;
	void CreateMenuLabels();
	void UpdateLabelLayout();
	void DrawMenuTitle();

	vector<shared_ptr<GUILabel>> mInstructionLabels;

	vector<shared_ptr<GameObject>> mMenuAsteroids;
	vector<shared_ptr<GameObject>> mGameAsteroids;
	vector<shared_ptr<IMouseListener>> mMouseListeners;

	int mScreenWidth;
	int mScreenHeight;

	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	shared_ptr<GameObject> CreateExplosion();
	
	virtual void OnButtonClick(Button* button);

	// to start fighting the astroids
	void StartGame();


	// button dimensions
	GLVector2f screenSize = GLVector2f(800.0f, 600.0f);
	GLVector2f buttonSize = GLVector2f(200.0f, 50.0f);
	GLVector2f buttonPos = GLVector2f(screenSize.x / 2, screenSize.y * 0.2f);

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif