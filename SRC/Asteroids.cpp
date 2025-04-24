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
#include "Button.h"
#include "GUILabel.h"
#include <Windows.h>
#include "Explosion.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv) 
{
	mLevel = 0;
	mAsteroidCount = 0;
	mInMenu = true;
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

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////
void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	// Handle return to menu in any state
	if (key == 'p' || key == 'P') { // 27 = ESC key
		if (mShowingInstructions) {
			HideInstructions();
			//this is the problem
			DeleteMenu();
			glutPostRedisplay();
			StartGame();
		}
		else if (!mInMenu) {
			// Return from game to menu
			ReturnToMenu();
		}
		return;
	}

	if (key == 'm' || key == 'M') { // 27 = ESC key
		DeleteMenu();
		glutPostRedisplay();
		StartGame();
		return;
	}

	switch (key)
	{
	case ' ':
		mSpaceship->Shoot();
		break;

	default:
		break;
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
	if (object->GetType() == GameObjectType("Asteroid")) {
		auto asteroid = dynamic_pointer_cast<Asteroid>(object);
		if (asteroid && !asteroid->IsMenuAsteroid()) {
			// Only process game asteroids
			shared_ptr<GameObject> explosion = CreateExplosion();
			explosion->SetPosition(object->GetPosition());
			explosion->SetRotation(object->GetRotation());
			mGameWorld->AddObject(explosion);
			mAsteroidCount--;

			if (mAsteroidCount <= 0) {
				SetTimer(500, START_NEXT_LEVEL);
			}
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
		CreateAsteroids(num_asteroids, false);
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
	HideMenu();
	mGameWorld->ClearObjects();
	// Force immediate redraw
	glutPostRedisplay();

	DeleteAllAsteroids();

	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

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
	CreateAsteroids(10, false);

	// Reset game state
	mLevel = 0;
	//mScoreKeeper.ResetScore();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Change to playing state
	mGameWorld->SetState(::GameState::PLAYING);


}

void Asteroids::CreateAsteroids(int count, bool forMenu)
{
	InitializeResources();
	Animation* anim = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
	if (!anim) return;

	for (int i = 0; i < count; i++) {
		auto asteroid = make_shared<Asteroid>(forMenu);
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		auto sprite = make_shared<Sprite>(anim->GetWidth(), anim->GetHeight(), anim);
		asteroid->SetSprite(sprite);
		asteroid->SetScale(0.2f);

		mGameWorld->AddObject(asteroid);

		if (forMenu) {
			mMenuAsteroids.push_back(asteroid); // Add to menu container
		}
		else {
			mGameWorld->AddObject(asteroid);    // Add to game world
			mAsteroidCount++;
		}
	}
}

void Asteroids::DeleteAllAsteroids()
{
	// Clear all menu asteroids
	for (auto& asteroid : mMenuAsteroids) {
		mGameWorld->RemoveObject(asteroid);
	}
	mMenuAsteroids.clear();
}

void Asteroids::SetupInputListeners()
{
	// Create shared_ptr to this 
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Store the shared_ptr to prevent deletion
	mSelfPtr = thisPtr;
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

void Asteroids::DrawMenuBackground()
{
	// Starfield background
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < 100; i++) {
		glVertex2f(rand() % GLUT_SCREEN_WIDTH, rand() % GLUT_SCREEN_HEIGHT);
	}
	glEnd();

	// Asteroids logo text (big blocky letters)
	const char* title = "ASTEROIDS";
	int titleWidth = 0;
	for (const char* c = title; *c; c++) {
		titleWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c) * 2;
	}

	glColor3f(0.0f, 1.0f, 0.0f); // Green
	glRasterPos2i((GLUT_SCREEN_WIDTH - titleWidth) / 2, GLUT_SCREEN_HEIGHT - 100);
	for (const char* c = title; *c; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
		// Extra spacing for retro look
		//glRasterPos2i(glutGet(GLUT_CURRENT_RASTER_POSITION_X) + 20,
		//	glutGet(GLUT_CURRENT_RASTER_POSITION_Y));
	}
}

bool Asteroids::HandleEscapeKey()
{
	if (mInMenu) {
		return false; // Let default handler exit
	}
	else {
		ReturnToMenu();
		return true; // We handled it
	}
}

// my function
void Asteroids::CreateMenu()
{
	// Create menu asteroids
	CreateAsteroids(12, true);

	// Create buttons
	CreateMenuButtons();

	// Add resize handler (using lambda directly)
	mGameWindow->AddResizeCallback([this](int w, int h) {
		mScreenWidth = w;
		mScreenHeight = h;
		UpdateButtonLayout();
		});
}

void Asteroids::DeleteMenu()
{
	// Clear existing buttons
	for (auto& button : mButtons) {
		mGameWindow->RemoveMouseListener(button);
		button->SetActive(false);
		button->SetVisible(false);
	}
	mButtons.clear();
	mMenuAsteroids.clear();
	
}

void Asteroids::CreateMenuButtons()
{
	mButtons.clear();

	// Create and configure buttons
	auto startBtn = make_shared<Button>("Start Game");
	auto difficultyBtn = make_shared<Button>("Difficulty");
	auto instructionsBtn = make_shared<Button>("Instructions");
	auto highscoresBtn = make_shared<Button>("High Scores");

	// Set button click handlers
	startBtn->SetClickListener([this]() { 			
		HideMenu();
		glutPostRedisplay();
		StartGame(); });
	//difficultyBtn->SetClickListener([this]() { ShowDifficultyOptions(); });
	instructionsBtn->SetClickListener([this]() { ShowInstructions(); });
	//highscoresBtn->SetClickListener([this]() { ShowHighScores(); });

	// Store buttons
	mButtons = { startBtn, difficultyBtn, instructionsBtn, highscoresBtn };

	// Set initial layout
	UpdateButtonLayout();

	// Register mouse listeners for all buttons
	for (auto& btn : mButtons) {
		mGameWindow->AddMouseListener(btn);
	}
}

void Asteroids::ReturnToMenu()
{
	// Clear game state
	mGameWorld->ClearObjects();
	mInstructionLabels.clear();
	mInMenu = true;

	// Reset game state
	mLevel = 0;
	mAsteroidCount = 0;

	// Recreate menu
	CreateMenu();
	ShowMenu();

	// Reset GUI
	mScoreLabel->SetText("Score: 0");
	mLivesLabel->SetText("Lives: 3");
	mGameOverLabel->SetVisible(false);
}

void Asteroids::ShowInstructions()
{
	// Clear any existing menu or game state
	HideMenu();
	mGameWorld->ClearObjects();

	// Set flag to indicate we're showing instructions
	mShowingInstructions = true;
	mInMenu = false;

	// Create a black background panel that covers the entire screen
	shared_ptr<GUIComponent> background = make_shared<GUIComponent>();
	background->SetColor(GLVector3f(0, 0, 0)); // Black
	background->SetPosition(GLVector2i(0, 0));
	background->SetSize(GLVector2i(mScreenWidth, mScreenHeight));
	mGameDisplay->GetContainer()->AddComponent(background, GLVector2f(0, 0));

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
		"M/ESC - Return to Menu",
		"",
		"Objective:",
		"Destroy all asteroids!",
		"Avoid collisions with them.",
		"",
		"Press P or ESC to return"
	};

	// Create and position labels
	float yPos = 0.9f; // Start near top (10% from top)
	for (const string& line : instructions) {
		auto label = make_shared<GUILabel>(line);
		label->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		label->SetColor(GLVector3f(1, 1, 1)); // White text
		mGameDisplay->GetContainer()->AddComponent(label, GLVector2f(0.5f, yPos));
		mInstructionLabels.push_back(label);
		yPos -= 0.07f; // Move down 7% per line
	}

	// Force immediate redraw
	glutPostRedisplay();
}

void Asteroids::HideInstructions() {
	mShowingInstructions = false;
	for (auto& label : mInstructionLabels) {
		label->SetVisible(false);
	}
}

void Asteroids::UpdateButtonLayout()
{
	const float baseWidth = 800.0f;  // Original design width
	const float baseHeight = 600.0f; // Original design height

	// Calculate scale factors
	float scaleX = mScreenWidth / baseWidth;
	float scaleY = mScreenHeight / baseHeight;

	// Calculate button size (300x50 at base size)
	int buttonWidth = static_cast<int>(300 * scaleX);
	int buttonHeight = static_cast<int>(50 * scaleY);

	// Position buttons (starting at 70% down, spaced by 12%)
	float yPos = 0.7f;
	const float buttonSpacing = 0.12f;
	const float xPos = ((baseWidth - 50) / 3) / baseWidth; // 35% from left

	for (auto& btn : mButtons) {
		btn->SetSize(GLVector2i(buttonWidth, buttonHeight));

		// Update position
		mGameDisplay->GetContainer()->RemoveComponent(static_pointer_cast<GUIComponent>(btn));
		mGameDisplay->GetContainer()->AddComponent(
			static_pointer_cast<GUIComponent>(btn),
			GLVector2f(xPos, yPos));

		yPos -= buttonSpacing;

		// Refresh mouse listener
		mGameWindow->RemoveMouseListener(btn);
		mGameWindow->AddMouseListener(btn);
		mGameWindow->AddMouseListener(btn);
	}
}


void Asteroids::InitializeResources()
{
	// Load animations if they haven't been loaded yet
	static bool resourcesInitialized = false;
	if (!resourcesInitialized) {
		AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
		AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
		AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
		resourcesInitialized = true;
	}
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
	mInMenu = false;

	if (!mButtons.empty()) {
		for (auto& button : mButtons) {
			button->SetActive(false);
			button->SetVisible(false);
		}
	}
}

void Asteroids::ShowMenu()
{
	mInMenu = true;

	// Clear existing buttons
	for (auto& button : mButtons) {
		mGameWindow->RemoveMouseListener(button);
	}

	// Recreate all menu buttons
	CreateMenuButtons();  // Extract button creation to a separate function

	// Force redraw
	glutPostRedisplay();
}

void Asteroids::OnButtonClick(Button* button)
{
	if (button == mStartButton.get()) {
		StartGame();
	}
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