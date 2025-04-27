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
#include <Windows.h>

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

	void CreateExtraLives(int count);

	void CreateBulletPowerUp(int count);

	enum class GameState {
		MENU,
		IN_GAME,
		INSTRUCTIONS,
		HIGH_SCORES,
		GAME_OVER
	};

	void CreateAsteroids(int count);
	void DeleteAllGameObjects();

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
	void ShowNameInput();
	void CleanupNameInput();

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

	// GUI
	void CreateGUI();

	// Menu
	void ReturnToMenu();

	void CreateMenu();
	void ShowMenu();
	void HideMenu();

	void ShowInstructions();
	void HideInstructions();

	void ShowHighScore();
	void HideHighScore();

private:
	void InitializeResources();

	void LoadHighScores();
	void SaveHighScores();
	void AddHighScore(const std::string& name, int score);

	void CreateMenuLabels();
	void UpdateLabelLayout();
	void DrawMenuTitle();

	int mWeaponLevel = 1; // Start with basic weapon
	const int MAX_WEAPON_LEVEL = 3; // Maximum upgrade level

	bool mWaitingForNameInput = false;
	string mPlayerName;
	shared_ptr<GUILabel> mNameInputLabel;
	shared_ptr<GUILabel> mEnterNameLabel;

	GameState mCurrentState;
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mInstructionsLabel;
	shared_ptr<Asteroids> mSelfPtr; // Prevents deletion while listening

	vector<pair<string, int>> mHighScores;
	vector<shared_ptr<GUILabel>> mMenuLabels;
	vector<shared_ptr<GUILabel>> mInstructionLabels;
	vector<shared_ptr<GUILabel>> mHighScoreLabels;

	vector<shared_ptr<GameObject>> mAsteroids;
	vector<shared_ptr<IMouseListener>> mMouseListeners;

	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	int mScreenWidth;
	int mScreenHeight;

	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	shared_ptr<GameObject> CreateExplosion();
	
	// to start fighting the astroids
	void StartGame();
	void StopGame();

	void CreateBlackHole(int count);

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