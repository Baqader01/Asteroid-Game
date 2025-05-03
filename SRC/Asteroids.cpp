#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "ExtraLives.h"
#include "BlackHoleCoin.h"
#include "BlackHole.h"
#include "WeaponPowerup.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv), mCurrentState(GameState::MENU)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	https://wenrexa.itch.io/laser2020
	AnimationManager::GetInstance().CreateAnimationFromFile("bullet", 122, 119, 122, 119, "PowerupBullet.png");

	//Hansjörg Malthaner : http://opengameart.org/users/varkalandar
	AnimationManager::GetInstance().CreateAnimationFromFile("wormhole", 128, 96, 128, 96, "Wormhole.png");

	// game asset from itch io by goblin mode games: https://goblin-mode-games.itch.io/pixel-art-heart-capsules
	AnimationManager::GetInstance().CreateAnimationFromFile("heart", 80, 80, 80, 80, "Hearts.png");

	//creating the menu
	CreateMenu();

	// Create some asteroids and add them to the world
	CreateAsteroids(10);

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

void Asteroids::StartGame()
{
	mCurrentState = GameState::IN_GAME;
	mLevel = 0;
	mScoreKeeper.ResetScore();
	mPlayer.ResetLives();

	// Clean and recreate UI
	DeleteLabels();

	CreateGUI();

	// Create powerups if enabled
	if (mEnableExtraLives) CreateExtraLives(2);
	if (mEnableBlackHoles) CreateBlackHole(2);
	if (mEnableWeaponPowerup) CreateWeaponPowerup(2);

	// Create player spaceship
	mGameWorld->AddObject(CreateSpaceship());
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	const char lowerKey = tolower(key);


	if (mCurrentState == GameState::IN_GAME)
	{
		switch (key)
		{
		case ' ':
			mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
	else if (mCurrentState == GameState::DIFFICULTY) {
		switch (lowerKey) {
		case 'l':
			mEnableExtraLives = !mEnableExtraLives;
			CreateDifficulty(); // Refresh display
			break;
		case 'b':
			mEnableBlackHoles = !mEnableBlackHoles;
			CreateDifficulty(); // Refresh display
			break;
		case 'w':
			mEnableWeaponPowerup = !mEnableWeaponPowerup;
			CreateDifficulty(); // Refresh display
			break;
		case 'm':
			CreateMenu();
			break;
		}
	}
	else if (mCurrentState == GameState::GAME_OVER)
	{
		if (mWaitingForNameInput) {
			if (!mNameInputLabel) return;

			if (key == 13) { // Enter key
				if (!mPlayerName.empty()) {
					AddHighScore(mPlayerName, mScoreKeeper.GetScore());
				}
				CleanupNameInput();
				CreateHighScore();
			}
			else if (key == 8 && !mPlayerName.empty()) { // Backspace
				mPlayerName.pop_back();
				mNameInputLabel->SetText(mPlayerName);
			}
			else if (isalpha(key) && mPlayerName.length() < 10) { // Letter input
				mPlayerName += key;
				mNameInputLabel->SetText(mPlayerName);
			}
			return;
		}

		switch (lowerKey) {
		case 'm':
			CreateMenu();
			break;
		}
	}
	else
	{
		switch (lowerKey) {
		case 'p': StartGame(); break;
		case 'd': CreateDifficulty(); break;
		case 'i': CreateInstructions(); break;
		case 'h': CreateHighScore(); break;
		case 'm': CreateMenu(); break;
		}
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		if (mCurrentState == GameState::IN_GAME)
		{
			shared_ptr<GameObject> explosion = CreateExplosion();
			explosion->SetPosition(object->GetPosition());
			explosion->SetRotation(object->GetRotation());
			mGameWorld->AddObject(explosion);
			mAsteroidCount--;
			if (mAsteroidCount <= 0)
			{
				SetTimer(500, START_NEXT_LEVEL);
			}
		}
	}
	else if (object->GetType() == GameObjectType("ExtraLives")) {
		if (mLivesLabel) {  // Check label exists
			mPlayer.IncreaseLives();
			mLivesLabel->SetText("Lives: " + std::to_string(mPlayer.GetLives()));
		}
		return;
	}
	else if (object->GetType() == GameObjectType("BlackHoleCoin")) {
		// When a BlackHoleCoin is collected 
		auto coin = dynamic_pointer_cast<BlackHoleCoin>(object);

		// Create an active black hole 
		auto newBlackHole = make_shared<BlackHole>();
		newBlackHole->SetScale(0.2);
		newBlackHole->SetBoundingShape(make_shared<BoundingSphere>(newBlackHole->GetThisPtr(), 20)); // Initialize bounding shape

		// Set visual properties
		Animation* anim = AnimationManager::GetInstance().GetAnimationByName("wormhole");
		if (anim) {
			newBlackHole->SetSprite(make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim));
		}

		mGameWorld->AddObject(newBlackHole);
	}
	else if (object->GetType() == GameObjectType("WeaponPowerup"))
	{
		// Activate powerup on spaceship if it exists
		if (mSpaceship) {
			mSpaceship->ActivateMissilePowerup();
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);

		// Create powerups if enabled, with maximum limits
		const int MAX_POWERUPS = 6;
		const int SPAWN_COUNT = 2;

		// Check and create each powerup type
		if (mEnableExtraLives) {
			int currentExtraLives = mGameWorld->CountObjectsOfType("ExtraLives");
			if (currentExtraLives <= MAX_POWERUPS - SPAWN_COUNT) {
				CreateExtraLives(SPAWN_COUNT);
			}
		}

		if (mEnableBlackHoles) {  // Changed from else-if to if for independent checks
			int currentBlackHoles = mGameWorld->CountObjectsOfType("BlackHole");
			if (currentBlackHoles <= MAX_POWERUPS - SPAWN_COUNT) {
				CreateBlackHole(SPAWN_COUNT);
			}
		}

		if (mEnableWeaponPowerup) {
			int currentWeapons = mGameWorld->CountObjectsOfType("WeaponPowerup");
			if (currentWeapons <= MAX_POWERUPS - SPAWN_COUNT) {
				CreateWeaponPowerup(SPAWN_COUNT);
			}
		}
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateExtraLives(int count) {
	Animation* anim = AnimationManager::GetInstance().GetAnimationByName("heart");
	if (!anim) return;

	for (int i = 0; i < count; i++) {
		auto extraLife = make_shared<ExtraLives>();
		extraLife->SetBoundingShape(make_shared<BoundingSphere>(extraLife->GetThisPtr(), 2)); // Smaller bounding sphere
		auto sprite = make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim);
		extraLife->SetSprite(sprite);
		extraLife->SetScale(0.1f);  // Add scaling

		mGameWorld->AddObject(extraLife);
	}
}

void Asteroids::CreateBlackHole(int count)
{
	Animation* anim = AnimationManager::GetInstance().GetAnimationByName("wormhole");
	if (!anim) return;

	for (int i = 0; i < count; i++) {
		auto coin = make_shared<BlackHoleCoin>();

		// Make it smaller than the actual black hole
		coin->SetBoundingShape(make_shared<BoundingSphere>(coin->GetThisPtr(), 2));
		auto sprite = make_shared<Sprite>(anim->GetWidth() / 2, anim->GetHeight() / 2, anim);
		coin->SetSprite(sprite);
		coin->SetScale(0.2f); // Smaller scale for the coin

		mGameWorld->AddObject(coin);
	}
}

void Asteroids::CreateWeaponPowerup(int count)
{
	Animation* anim = AnimationManager::GetInstance().GetAnimationByName("bullet");
	if (!anim) return;

	for (int i = 0; i < count; i++) {
		auto powerUp = make_shared<WeaponPowerup>();
		powerUp->SetBoundingShape(make_shared<BoundingSphere>(powerUp->GetThisPtr(), 2.0f));
		auto sprite = make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim);
		powerUp->SetSprite(sprite);
		powerUp->SetScale(0.1f);
		mGameWorld->AddObject(powerUp);
	}
}

void Asteroids::CreateGUI()
{
	// Clear any existing GUI labels first
	DeleteLabels();

	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));

	// Create and add score label (top-left)
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> score_component = static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));
	mLabels.push_back(mScoreLabel);  // Add to unified label container

	// Create and add lives label (bottom-left)
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));
	mLabels.push_back(mLivesLabel);  // Add to unified label container

	// Create and add game over label (centered, initially hidden)
	mGameOverLabel = make_shared<GUILabel>("GAME OVER");
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameOverLabel->SetVisible(false);
	shared_ptr<GUIComponent> game_over_component = static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
	mLabels.push_back(mGameOverLabel);  // Add to unified label container
}

void Asteroids::CreateMenu()
{
	DeleteLabels();

	mCurrentState = GameState::MENU;

	// Title and menu items with their colors
	const vector<pair<string, GLVector3f>> menuItems = {
		{"ASTEROIDS", GLVector3f(0, 1, 0)},  // Green title
		{"Press 'P' to start the game", GLVector3f(1, 1, 1)},
		{"Press 'D' to change the difficulty", GLVector3f(1, 1, 1)},
		{"Press 'H' to look at the high scores", GLVector3f(1, 1, 1)},
		{"Press 'I' to look at the instructions", GLVector3f(1, 1, 1)},
		{"Press 'M' to go back to the menu", GLVector3f(1, 1, 1)}
	};

	float yPos = 0.8f;
	const float spacing = 0.15f;

	for (const auto& item : menuItems) {
		auto label = make_shared<GUILabel>(item.first);
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetColor(item.second);

		// Properly cast to GUIComponent before adding
		shared_ptr<GUIComponent> label_component =
			static_pointer_cast<GUIComponent>(label);
		mGameDisplay->GetContainer()->AddComponent(label_component, GLVector2f(0.5f, yPos));

		mLabels.push_back(label);
		yPos -= spacing;
	}
}

void Asteroids::CreateInstructions()
{
	// Clear any existing menu or game state
	DeleteLabels();
	mCurrentState = GameState::INSTRUCTIONS;

	// Instruction text lines
	vector<string> instructions = {
		"HOW TO PLAY",
		"------------------",
		"Movement:",
		"UP ARROW - Thrust Forward",
		"LEFT/RIGHT ARROWS - Rotate",
		"",
		"Actions:",
		"SPACEBAR - Fire Laser",
		"",
		"Objective:",
		"Destroy all asteroids!",
		"Avoid collisions with them.",
		"M - Return to Menu"

	};

	// Create and position labels
	float yPos = 0.9f; // Start near top (10% from top)
	for (const string& line : instructions) {
		auto label = make_shared<GUILabel>(line);
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetColor(GLVector3f(1, 1, 1)); // White text
		mGameDisplay->GetContainer()->AddComponent(label, GLVector2f(0.5f, yPos));
		mLabels.push_back(label);
		yPos -= 0.07f; // Move down 7% per line
	}

	// Force immediate redraw
	glutPostRedisplay();
}

void Asteroids::CreateHighScore()
{
	mCurrentState = GameState::HIGH_SCORES;
	highScores.Load("HighScore.txt");

	DeleteLabels();

	// Helper function to add labels
	auto AddLabel = [this](const std::string& text, float y, const GLVector3f& color) {
		auto label = std::make_shared<GUILabel>(text);
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetColor(color);
		mGameDisplay->GetContainer()->AddComponent(
			std::static_pointer_cast<GUIComponent>(label),
			GLVector2f(0.5f, y));
		mLabels.push_back(label);
		};

	// Add title and scores
	AddLabel("HIGH SCORES", 0.9f, GLVector3f(1, 1, 0));

	float yPos = 0.7f;
	const auto& scores = highScores.GetScores();
	for (size_t i = 0; i < 5; i++) {
		std::string text = std::to_string(i + 1) + ". ";
		if (i < scores.size()) {
			text += scores[i].first + " " + std::to_string(scores[i].second);
		}
		AddLabel(text, yPos, GLVector3f(1, 1, 1));
		yPos -= 0.1f;
	}

	AddLabel("Press 'M' to return", 0.2f, GLVector3f(0, 1, 0));
	glutPostRedisplay();
}

void Asteroids::CreateGameOver()
{
	mCurrentState = GameState::GAME_OVER;

	DeleteLabels();
	mWaitingForNameInput = true;

	// Create input prompt
	mEnterNameLabel = make_shared<GUILabel>("Enter your name (Press ENTER when done):");
	mEnterNameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mEnterNameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mEnterNameLabel->SetColor(GLVector3f(1, 1, 0));
	mGameDisplay->GetContainer()->AddComponent(mEnterNameLabel, GLVector2f(0.5f, 0.6f));

	// Create label that will show the name as it's typed
	mNameInputLabel = make_shared<GUILabel>("");
	mNameInputLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mNameInputLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mNameInputLabel->SetColor(GLVector3f(1, 1, 1));
	mGameDisplay->GetContainer()->AddComponent(mNameInputLabel, GLVector2f(0.5f, 0.5f));

	mLabels.push_back(mEnterNameLabel);
	mLabels.push_back(mEnterNameLabel);
}

void Asteroids::CreateDifficulty()
{
	DeleteLabels();
	mCurrentState = GameState::DIFFICULTY;

	float yPos = 0.8f;
	const float spacing = 0.15f;

	// Title
	auto title = make_shared<GUILabel>("DIFFICULTY SETTINGS");
	title->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	title->SetColor(GLVector3f(0, 1, 0)); // Green
	mGameDisplay->GetContainer()->AddComponent(title, GLVector2f(0.5f, yPos));
	mLabels.push_back(title);
	yPos -= spacing;

	// Extra Lives Toggle
	string livesText = "Extra Lives: " + string(mEnableExtraLives ? "ON (Press L)" : "OFF (Press L)");
	auto livesLabel = make_shared<GUILabel>(livesText);
	livesLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	livesLabel->SetColor(GLVector3f(1, 1, 1));
	mGameDisplay->GetContainer()->AddComponent(livesLabel, GLVector2f(0.5f, yPos));
	mLabels.push_back(livesLabel);
	yPos -= spacing;

	// Black Holes Toggle
	string holesText = "Black Holes: " + string(mEnableBlackHoles ? "ON (Press B)" : "OFF (Press B)");
	auto holesLabel = make_shared<GUILabel>(holesText);
	holesLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	holesLabel->SetColor(GLVector3f(1, 1, 1));
	mGameDisplay->GetContainer()->AddComponent(holesLabel, GLVector2f(0.5f, yPos));
	mLabels.push_back(holesLabel);
	yPos -= spacing;

	// Weapon powerup Toggle
	string weaponText = "Weapon power up: " + string(mEnableWeaponPowerup ? "ON (Press W)" : "OFF (Press W)");
	auto weaponLabel = make_shared<GUILabel>(weaponText);
	weaponLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	weaponLabel->SetColor(GLVector3f(1, 1, 1));
	mGameDisplay->GetContainer()->AddComponent(weaponLabel, GLVector2f(0.5f, yPos));
	mLabels.push_back(weaponLabel);
	yPos -= spacing;

	// Back to menu
	auto backLabel = make_shared<GUILabel>("Press M to return to Menu");
	backLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	backLabel->SetColor(GLVector3f(1, 1, 0)); // Yellow
	mGameDisplay->GetContainer()->AddComponent(backLabel, GLVector2f(0.5f, yPos));
	mLabels.push_back(backLabel);

	glutPostRedisplay();
}

void Asteroids::AddHighScore(const std::string& name, int score)
{
	// Add the new score
	highScores.Add(name, score);

	// Refresh the display if we're currently showing highscores
	if (mCurrentState == GameState::HIGH_SCORES) {
		CreateHighScore(); // Rebuild the display
	}
}

void Asteroids::ShowNameInput()
{
	mWaitingForNameInput = true;
	mPlayerName.clear();

	// Create input prompt
	mEnterNameLabel = make_shared<GUILabel>("Enter your name (Press ENTER when done):");
	mEnterNameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mEnterNameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mEnterNameLabel->SetColor(GLVector3f(1, 1, 0));
	mGameDisplay->GetContainer()->AddComponent(mEnterNameLabel, GLVector2f(0.5f, 0.6f));

	// Create label that will show the name as it's typed
	mNameInputLabel = make_shared<GUILabel>("");
	mNameInputLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mNameInputLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mNameInputLabel->SetColor(GLVector3f(1, 1, 1));
	mGameDisplay->GetContainer()->AddComponent(mNameInputLabel, GLVector2f(0.5f, 0.5f));
}

void Asteroids::CleanupNameInput()
{
	mWaitingForNameInput = false;
	if (mNameInputLabel) {
		mGameDisplay->GetContainer()->RemoveComponent(mNameInputLabel);
		mNameInputLabel.reset();
	}
	if (mEnterNameLabel) {
		mGameDisplay->GetContainer()->RemoveComponent(mEnterNameLabel);
		mEnterNameLabel.reset();
	}
}


void Asteroids::DeleteLabels()
{
	auto container = mGameDisplay->GetContainer();

	// Remove from container 
	for (auto& label : mLabels) {
		if (label) {
			// Remove from display container
			mGameDisplay->GetContainer()->RemoveComponent(
				static_pointer_cast<GUIComponent>(label));
		}
	}
	mLabels.clear();

	// Reset all label pointers
	mScoreLabel.reset();
	mLivesLabel.reset();
	mGameOverLabel.reset();
	mEnterNameLabel.reset();
	mNameInputLabel.reset();

	// Then clear the vector
	glutPostRedisplay();
}

void Asteroids::OnScoreChanged(int score)
{
	// Only update if label exists
	if (mScoreLabel) {
		// Format the score message using an string-based stream
		std::ostringstream msg_stream;
		msg_stream << "Score: " << score;
		// Get the score message as a string
		std::string score_msg = msg_stream.str();
		mScoreLabel->SetText(score_msg);
	}
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Only update if label exists
	if (mLivesLabel) {
		// Format the lives left message using an string-based stream
		std::ostringstream msg_stream;
		msg_stream << "Lives: " << lives_left;
		// Get the lives left message as a string
		std::string lives_msg = msg_stream.str();
		mLivesLabel->SetText(lives_msg);
	}

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		// Show game over and prompt for name
		mGameOverLabel->SetText("GAME OVER");
		mGameOverLabel->SetVisible(true);
		CreateGameOver();
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}