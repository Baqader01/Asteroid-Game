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

#include <Windows.h>

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

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

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

	DeleteLabels();

	//Create the GUI
	CreateGUI();

	// Create a spaceship and add it to the world
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
	else
	{
		switch (lowerKey) {
		case 'm':
			CreateMenu();
			return;
		case 'p':
			StartGame();
			return;
		case 'i':
			CreateInstructions();
			return;
		case 'h':
			CreateHighScore();
			return;
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
	const std::vector<std::pair<std::string, GLVector3f>> menuItems = {
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
	// Then clear the vector

	glutPostRedisplay();
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
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




