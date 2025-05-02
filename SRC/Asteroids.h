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
#include "HighScore.h"
#include <vector>

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char* argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	void StartGame();
	virtual void Stop(void);

	enum class GameState {
		MENU,
		IN_GAME,
		INSTRUCTIONS,
		DIFFICULTY,
		HIGH_SCORES,
		GAME_OVER
	};

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

	HighScore highScores;
	vector<shared_ptr<GUILabel>> mLabels;

	GameState mCurrentState;

	uint mLevel;
	uint mAsteroidCount;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();

	void CreateMenu();
	void CreateInstructions();
	void CreateHighScore();
	void CreateGameOver();
	void CreateDifficulty();

	// Difficulty (default all enabled)
	bool mEnableExtraLives = true;
	bool mEnableBlackHoles = true;
	bool mEnableWeaponPowerup = true;

	void AddHighScore(const std::string& name, int score);

	void ShowNameInput();
	void CleanupNameInput();

	bool mWaitingForNameInput = false;
	string mPlayerName;
	shared_ptr<GUILabel> mNameInputLabel;
	shared_ptr<GUILabel> mEnterNameLabel;

	void DeleteLabels();

	//powerups
	void CreateAsteroids(const uint num_asteroids);
	void CreateExtraLives(int count);
	void CreateBlackHole(int count);

	// Weapon upgrade
	void CreateWeaponPowerup(int count);

	shared_ptr<GameObject> CreateExplosion();

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif