#include "Asteroids.h"
#include "Asteroid.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "GameSession.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include <Windows.h>
#include "ExtraLives.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv), mCurrentState(GameState::MENU)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mScreenWidth = 700; 
	mScreenHeight = 600;

	SetupInputListeners();
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	InitializeResources();
	CreateMenu();

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

void Asteroids::CreateExtraLives(int count)
{
	Animation* anim = AnimationManager::GetInstance().GetAnimationByName("heart");
	if (!anim) return;

	for (int i = 0; i < count; i++) {
		auto extraLife = make_shared<ExtraLives>();

		// Set visual properties
		extraLife->SetBoundingShape(make_shared<BoundingSphere>(extraLife->GetThisPtr(), 4.0f));
		auto sprite = make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim);
		extraLife->SetSprite(sprite);
		extraLife->SetScale(0.1f);

		mGameWorld->AddObject(extraLife);
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////
void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	const char lowerKey = tolower(key);

	// State-independent commands
	switch (lowerKey) {
	case 'm': // Always returns to menu
		if (mCurrentState != GameState::MENU) {
			ReturnToMenu();
		}
		return;

	case 'p': // Start game from menu or instructions
		if (mCurrentState == GameState::INSTRUCTIONS) {
			HideInstructions();
			StartGame();
		}
		else if (mCurrentState == GameState::MENU) {
			HideMenu();
			StartGame();
		}
		return;
	case 'i':
		if (mCurrentState == GameState::MENU) {
			HideMenu();
			ShowInstructions();
		}
		return;
	}

	// State-specific commands
	switch (mCurrentState) {
	case GameState::MENU:
		//if (lowerKey == 'i') HideMenu(); ShowInstructions();
		/*else if (lowerKey == 'h') ShowHighScores();
		else if (lowerKey == 'd') ChangeDifficulty();*/
		break;

	case GameState::IN_GAME:
		if (lowerKey == ' ') mSpaceship->Shoot();
		break;

	case GameState::INSTRUCTIONS:
	case GameState::HIGH_SCORES:
	case GameState::GAME_OVER:
		// These states only handle 'm'
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(20); break;
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
	if (object->GetType() == GameObjectType("ExtraLives")) {
		OutputDebugString("ExtraLives collected! Attempting to increase lives...\n");

		// If mPlayer is a shared_ptr<Player>
		mPlayer.IncreaseLives();  // Give +1 life

		mLivesLabel->SetText("Lives: " + std::to_string(mPlayer.GetLives()));
		return;  // Skip other checks
	}

	if (object->GetType() == GameObjectType("Asteroid")) {
		auto asteroid = dynamic_pointer_cast<Asteroid>(object);

		// Remove from tracking container
		mAsteroids.erase(
			std::remove(mAsteroids.begin(), mAsteroids.end(), asteroid),
			mAsteroids.end()
		);
		mAsteroidCount = mAsteroids.size();

		// Create explosion
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);

		if (mAsteroidCount <= 0) {
			SetTimer(500, START_NEXT_LEVEL);
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
		CreateExtraLives(2 * mLevel);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}

}

void Asteroids::ResetSpaceship()
{
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

void Asteroids::StartGame()
{
	// i want the game to start here instead of start, so i can put the main menu over there
	mCurrentState = GameState::IN_GAME;

	// Force immediate redraw
	glutPostRedisplay();

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	//Create the GUI
	CreateGUI();

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create and add the spaceship
	mGameWorld->AddObject(CreateSpaceship());

	// Create initial asteroids
	CreateAsteroids(10);
	CreateExtraLives(5);

	// Reset game state
	mLevel = 0;
	//mScoreKeeper.ResetScore();

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);
}

void Asteroids::CreateAsteroids(int count) {
	InitializeResources();
	Animation* anim = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
	if (!anim) return;

	for (int i = 0; i < count; i++) {
		auto asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim));
		asteroid->SetScale(0.2f);

		mGameWorld->AddObject(asteroid);
		mAsteroids.push_back(asteroid);  // Add to tracking container
	}
	mAsteroidCount = mAsteroids.size();  // Keep count in sync
}

void Asteroids::DeleteAllAsteroids() {
	// Remove from game world
	for (auto& asteroid : mAsteroids) {
		mGameWorld->RemoveObject(asteroid);
	}

	// Clear container
	mAsteroids.clear();
	mAsteroidCount = 0;

	// Reset game state
	mScoreKeeper.ResetScore();
	mPlayer.ResetLives();
	mLevel = 0;

	// Only update UI if labels exist
	if (mScoreLabel && mLivesLabel) {
		OnScoreChanged(0);
		OnPlayerKilled(mPlayer.GetLives());
	}
}

void Asteroids::SetupInputListeners()
{
	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

}

// my function
void Asteroids::CreateMenu()
{
	InitializeResources();

	mAsteroids.clear();

	//Create Menu asteroids
	CreateAsteroids(12);

	// Create menu labels
	CreateMenuLabels();

	// Force redraw
	glutPostRedisplay();
}

void Asteroids::CreateMenuLabels()
{
	mMenuLabels.clear();

	// Title
	auto title = make_shared<GUILabel>("ASTEROIDS");
	title->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	title->SetColor(GLVector3f(0, 1, 0)); // Green
	title->SetFontSize(48);
	mMenuLabels.push_back(title);

	// Menu options
	vector<string> options = {
		"Press 'P' to start the game",
		"Press 'D' to change the difficulty",
		"Press 'H' to look at the high scores",
		"Press 'I' to look at the instructions"
	};

	for (const auto& text : options) {
		auto label = make_shared<GUILabel>(text);
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetColor(GLVector3f(1, 1, 1)); // White
		mMenuLabels.push_back(label);
	}

	// Position the labels
	UpdateLabelLayout();
}

void Asteroids::ReturnToMenu()
{
	switch (mCurrentState) {
	case GameState::IN_GAME:
	//	StopGame();
		break;
	case GameState::INSTRUCTIONS:
		HideInstructions();
		break;
	case GameState::HIGH_SCORES:
	//	HideHighScores();
		break;
	case GameState::GAME_OVER:
	//	HideGameOver();
		break;
	default:
		break;
	}
	mCurrentState = GameState::MENU;
	ShowMenu();
}

void Asteroids::ShowInstructions()
{
	// Clear any existing menu or game state
	HideMenu();
	mCurrentState = GameState::INSTRUCTIONS;
	mGameWorld->ClearObjects();

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
		"M - Return to Menu",
		"Press P to Play"
		"",
		"Objective:",
		"Destroy all asteroids!",
		"Avoid collisions with them.",
	};

	// Create and position labels
	float yPos = 0.9f; // Start near top (10% from top)
	for (const string& line : instructions) {
		auto label = make_shared<GUILabel>(line);
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetColor(GLVector3f(1, 1, 1)); // White text
		mGameDisplay->GetContainer()->AddComponent(label, GLVector2f(0.5f, yPos));
		mInstructionLabels.push_back(label);
		mInstructionLabels.push_back(label);
		yPos -= 0.07f; // Move down 7% per line
	}

	// Force immediate redraw
	glutPostRedisplay();
}

void Asteroids::HideInstructions() {
	DeleteAllAsteroids();

	for (auto& label : mInstructionLabels) {
		label->SetVisible(false);
	}
}

void Asteroids::UpdateLabelLayout()
{
	float yPos = 0.8f; // Start near top for title
	const float spacing = 0.15f;

	// Title position
	mGameDisplay->GetContainer()->RemoveComponent(static_pointer_cast<GUIComponent>(mMenuLabels[0]));
	mGameDisplay->GetContainer()->AddComponent(
		static_pointer_cast<GUIComponent>(mMenuLabels[0]),
		GLVector2f(0.5f, yPos));

	// Menu options positions
	for (size_t i = 1; i < mMenuLabels.size(); i++) {
		yPos -= spacing;
		mGameDisplay->GetContainer()->RemoveComponent(static_pointer_cast<GUIComponent>(mMenuLabels[i]));
		mGameDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(mMenuLabels[i]),
			GLVector2f(0.5f, yPos));
	}
}


void Asteroids::InitializeResources()
{
	AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	AnimationManager::GetInstance().CreateAnimationFromFile("heart", 80, 80, 80, 80, "Hearts.png");
}

void Asteroids::DrawMenuTitle()
{
	const char* title = "ASTEROIDS";
	glColor3f(0.0f, 1.0f, 0.0f); // Green

	// Calculate title width
	int titleWidth = 0;
	for (const char* c = title; *c; c++) {
		titleWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
	}

	// Center the title above buttons
	glRasterPos2i((mScreenWidth - titleWidth) / 2, mScreenHeight * 0.8f);
	for (const char* c = title; *c; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
}

void Asteroids::HideMenu()
{
	DeleteAllAsteroids();

	// Hide all labels 
	for (auto& label : mMenuLabels) {
		label->SetVisible(false);
	}

	glutPostRedisplay();
}

void Asteroids::ShowMenu()
{
	mCurrentState = GameState::MENU;

	// Create menu elements if they don't exist
	if (mMenuLabels.empty()) {
		CreateMenuLabels();
	}

	// Make all labels visible
	for (auto& label : mMenuLabels) {
		label->SetVisible(true);
	}

	// Show menu asteroids if there is none
	//for (auto& asteroid : mMenuAsteroids) {
	//}

	glutPostRedisplay();
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
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