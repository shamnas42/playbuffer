#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <fstream>

// Game Screen Size
int DISPLAY_WIDTH = 1280;
int DISPLAY_HEIGHT = 720;
int DISPLAY_SCALE = 1;

// Player States 
enum Agent8State
{
    STATE_APPEAR = 0,
    STATE_HALT,
    STATE_PLAY,
    STATE_DEAD,
};

// Different Scenes and Menus
enum GameloopScreen
{
    SLPASH_SCREEN = 0,
    MAIN_MENU,
    GAME_SCENE,
    LEADERBOARD,
    END_SCREEN,
};

// Game Information 
struct GameState
{
    // Timer For Splash Screen 
    float timer = 0;

    // Score For Game 
    int score{ 0 };

    // Menu Choices 
    int playerMenuChoice = 1;
    int playerLeaderboardChoice = 1;
    int playerEndScreenChoice = 1;

    // States for Agent And Scenes 
    Agent8State agentState{ STATE_APPEAR };
    GameloopScreen gameScreen{ SLPASH_SCREEN };
};

// Global Variable For Updating Game
GameState gameState;

// Different Game Objects 
enum GameObjectType
{
    TYPE_NULL = -1,
    TYPE_AGENT8,
    TYPE_FAN,
    TYPE_TOOL,
    TYPE_COIN,
    TYPE_STAR,
    TYPE_LASER,
    TYPE_DESTROYED,
};

// Declarations 
void HandlePlayerControls();
void UpdateFan();
void UpdateTools();
void UpdateCoinsAndStars();
void UpdateLasers();
void UpdateDestroyed();
void UpdateAgent8();
void GameloopSplashScreen(float elapsedTime);
bool GameloopMainMenu();
void GameloopGameScene();
bool GameloopEndScreen();
void HandleGameReset();


// Entry Point 
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
    // Sets Up Intial State of Game 
    Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
    Play::CentreAllSpriteOrigins();
    Play::LoadBackground("Data\\Backgrounds\\background.png");
    Play::StartAudioLoop("music");
    Play::CreateGameObject(TYPE_AGENT8, { 115, 600 }, 50, "agent8");
    int id_fan = Play::CreateGameObject(TYPE_FAN, { 1140, 503 }, 0, "fan");
    Play::GetGameObject(id_fan).velocity = { 0, -3 };
    Play::GetGameObject(id_fan).animSpeed = 1.0f;
}

// Updates Game Every 60 Times Per Second 
bool MainGameUpdate(float elapsedTime)
{
    // Gameloop: Switch To Different Menus And Screens 
    switch (gameState.gameScreen)
    {
        case SLPASH_SCREEN:
        {
            GameloopSplashScreen(elapsedTime);
            break;
        }

        case MAIN_MENU:
        {
            return GameloopMainMenu();
            break;
        }

        case GAME_SCENE:
        {
            GameloopGameScene();
            break;
        }

        case LEADERBOARD:
        {

            break;
        }

        case END_SCREEN:
        {
            return GameloopEndScreen();
            break;
        }

        default:
        {
            // Ends Game If There Is A Bug In Menu
            return true;
        }
    }

    // Returns True To End Game If Escape Is Pressed
    return Play::KeyDown(Play::KEY_ESCAPE);
}

// Quits Game When MainGameUpdate Returns True 
int MainGameExit(void)
{
    Play::DestroyManager();
    return PLAY_OK;
}

// Controls Animations and Movement Of Player 
void HandlePlayerControls()
{
    // Player Reference 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    // Movement Conditions 
    if (Play::KeyDown(Play::KEY_UP))
    {
        // Changes to Climb Animation And Moves Player Up 
        obj_agent8.velocity = { 0, 4 };
        Play::SetSprite(obj_agent8, "agent8_climb", 0.25f);
    }
    else if (Play::KeyDown(Play::KEY_DOWN))
    {
        // Changes to Fall Animation And Moves Player Down 
        obj_agent8.acceleration = { 0, -1 };
        Play::SetSprite(obj_agent8, "agent8_fall", 0);
    }
    else
    {
        // Non Movement 
        if (obj_agent8.velocity.y < -5)
        {
            // Animation for Stopping While Falling 
            gameState.agentState = STATE_HALT;
            Play::SetSprite(obj_agent8, "agent8_halt", 0.333f);
            obj_agent8.acceleration = { 0, 0 };
        }
        else
        {
            // Animation for Stopping While Moving Up  
            Play::SetSprite(obj_agent8, "agent8_hang", 0.02f);
            obj_agent8.velocity *= 0.5f;
            obj_agent8.acceleration = { 0, 0 };
        }
    }
    // Fire Lazer When Space Is Entered
    if (Play::KeyPressed(Play::KEY_SPACE))
    {
        // Spawns And Moves Lazer From Player 
        Vector2D firePos = obj_agent8.pos + Vector2D(155, 75);
        int id = Play::CreateGameObject(TYPE_LASER, firePos, 30, "laser");
        Play::GetGameObject(id).velocity = { 32, 0 };
        Play::PlayAudio("shoot");
    }

    // Updates Player 
    Play::UpdateGameObject(obj_agent8);

    // Keeps Players On Screen 
    if (Play::IsLeavingDisplayArea(obj_agent8))
    {
        obj_agent8.pos = obj_agent8.oldPos;
    }

    // Web Line From Players To Top Of Screen
    Play::DrawLine({ obj_agent8.pos.x, 720 }, obj_agent8.pos, Play::cWhite);
    Play::DrawObjectRotated(obj_agent8);
}

// Updates The Fan 
void UpdateFan()
{
    // Fan Reference 
    GameObject& obj_fan = Play::GetGameObjectByType(TYPE_FAN);

    // 1/50 Chance Of Spawning A Driver 
    if (Play::RandomRoll(50) == 50)
    {
        // Set Driver Objects And Its Movement 
        int id = Play::CreateGameObject(TYPE_TOOL, obj_fan.pos, 50, "driver");
        GameObject& obj_tool = Play::GetGameObject(id);
        obj_tool.velocity = Point2f(-8, Play::RandomRollRange(-1, 1) * 6);

        // 1/2 Chance Of Driver Becoming A Spanner 
        if (Play::RandomRoll(2) == 1)
        {
            // Spanner Animation and Movement 
            Play::SetSprite(obj_tool, "spanner", 0);
            obj_tool.radius = 100;
            obj_tool.velocity.x = -4;
            obj_tool.rotSpeed = 0.1f;
        }
        Play::PlayAudio("tool");
    }

    // 1/150 Chance Of Spawning Coin 
    if (Play::RandomRoll(150) == 1)
    {
        // Sets Coin Sprite And Animation and Movement 
        int id = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 40, "coin");
        GameObject& obj_coin = Play::GetGameObject(id);
        obj_coin.velocity = { -3, 0 };
        obj_coin.rotSpeed = 0.1f;
    }

    // Constantly Updates Fan 
    Play::UpdateGameObject(obj_fan);

    // Keeps Fan On Screen 
    if (Play::IsLeavingDisplayArea(obj_fan))
    {
        obj_fan.pos = obj_fan.oldPos;
        obj_fan.velocity.y *= -1;
    }

    // Draws Fan In New Frame 
    Play::DrawObject(obj_fan);
}

// Updates The Tools That Were Spawned 
void UpdateTools()
{
    // Player Reference and Tool Vector 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);

    // Looks Over Each Tool In Vector 
    for (int id : vTools)
    {
        // Reference For Tool 
        GameObject& obj_tool = Play::GetGameObject(id);

        // If Player Is Still Alive and Gets Hit 
        if (gameState.agentState != STATE_DEAD && Play::IsColliding(obj_tool, obj_agent8))
        {
            // Changes Players State To Dead After Getting Hit 
            Play::StopAudio("music");
            Play::PlayAudio("die");
            gameState.agentState = STATE_DEAD;
        }
        Play::UpdateGameObject(obj_tool);

        // Keeps Tools In Screen By Changing Velociy 
        if (Play::IsLeavingDisplayArea(obj_tool, Play::VERTICAL))
        {
            obj_tool.pos = obj_tool.oldPos;
            obj_tool.velocity.y *= -1;
        }
        Play::DrawObjectRotated(obj_tool);

        // Invisible Object Gets Destroyed 
        if (!Play::IsVisible(obj_tool))
        {
            Play::DestroyGameObject(id);
        }
    }
}

// Updates Coins 
void UpdateCoinsAndStars()
{
    // Player Reference and Coin Vector 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);

    // Looks Over Each Coin In Vector 
    for (int id_coin : vCoins)
    {
        // Reference For Coin Object 
        GameObject& obj_coin = Play::GetGameObject(id_coin);
        bool hasCollided = false;

        // If Player Has Collided With Coin 
        if (Play::IsColliding(obj_coin, obj_agent8))
        {
            // Spawning and Movement Of Stars 
            for (float rad{ 0.25f }; rad < 2.0f; rad += 0.5f)
            {
                int id = Play::CreateGameObject(TYPE_STAR, obj_agent8.pos, 0, "star");
                GameObject& obj_star = Play::GetGameObject(id);
                obj_star.rotSpeed = 0.1f;
                obj_star.acceleration = { 0.0f, -0.5f };
                Play::SetGameObjectDirection(obj_star, 16, rad * PLAY_PI);
            }

            hasCollided = true;
            gameState.score += 500;

            Play::PlayAudio("collect");
        }

        // Updates Coins And Its Animation 
        Play::UpdateGameObject(obj_coin);
        Play::DrawObjectRotated(obj_coin);

        // If Coin Has Been Hit Or Collided Then Desttory It 
        if (!Play::IsVisible(obj_coin) || hasCollided)
        {
            Play::DestroyGameObject(id_coin);
        }
    }

    // Vector For Stars 
    std::vector<int> vStars = Play::CollectGameObjectIDsByType(TYPE_STAR);

    // Looks At Every Star In The Vector 
    for (int id_star : vStars)
    {
        // Each Star 
        GameObject& obj_star = Play::GetGameObject(id_star);

        // Updates Look Of Star 
        Play::UpdateGameObject(obj_star);
        Play::DrawObjectRotated(obj_star);

        // Destroys The Stars When It's Not Visible 
        if (!Play::IsVisible(obj_star))
        {
            Play::DestroyGameObject(id_star);
        }
    }
}

// Updates Lasers Spawned 
void UpdateLasers()
{
    // Vectors For Lazers, Tools, And Coins 
    std::vector<int> vLasers = Play::CollectGameObjectIDsByType(TYPE_LASER);
    std::vector<int> vTools = Play::CollectGameObjectIDsByType(TYPE_TOOL);
    std::vector<int> vCoins = Play::CollectGameObjectIDsByType(TYPE_COIN);

    // Checks Every Lazer In Vector
    for (int id_laser : vLasers)
    {
        // Each Lazer 
        GameObject& obj_laser = Play::GetGameObject(id_laser);
        bool hasCollided = false;
        // Check Every Tool 
        for (int id_tool : vTools)
        {
            // Each Tool 
            GameObject& obj_tool = Play::GetGameObject(id_tool);
            // Checks If Lazer Hits Tool 
            if (Play::IsColliding(obj_laser, obj_tool))
            {
                // Changes Type Of Tool To Destroyed And Increases Score 
                hasCollided = true;
                obj_tool.type = TYPE_DESTROYED;
                gameState.score += 100;
            }
        }

        // Checks Every Coin 
        for (int id_coin : vCoins)
        {
            // Each Coin 
            GameObject& obj_coin = Play::GetGameObject(id_coin);
            // Checks If Lazer Hits Coin 
            if (Play::IsColliding(obj_laser, obj_coin))
            {
                // Changes Type Of Coin To Destroyed And Decreases Score 
                hasCollided = true;
                obj_coin.type = TYPE_DESTROYED;
                Play::PlayAudio("error");
                gameState.score -= 300;
            }
        }

        // Min For Score 
        if (gameState.score < 0)
        {
            gameState.score = 0;
        }

        // Updates The Lazer Each Time
        Play::UpdateGameObject(obj_laser);
        Play::DrawObject(obj_laser);

        // Destroy Lazers When It Is Not Visible Or Has Collided With Another Object 
        if (!Play::IsVisible(obj_laser) || hasCollided)
        {
            Play::DestroyGameObject(id_laser);
        }
    }
}

// Updates Destroyed Objects 
void UpdateDestroyed()
{
    // Vector For Destroyed Objects 
    std::vector<int> vDead = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);
    // Checks Each Destroyed Objects 
    for (int id_dead : vDead)
    {
        // Each Destroyed Object
        GameObject& obj_dead = Play::GetGameObject(id_dead);
        // Changes Animation, Updates Object, "Blinks", and Destorys object 
        obj_dead.animSpeed = 0.2f;
        Play::UpdateGameObject(obj_dead);
        if (obj_dead.frame % 2)
        {
            Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);
        }
        if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)
        {
            Play::DestroyGameObject(id_dead);
        }
    }
}

// Updates Player 
void UpdateAgent8()
{
    // Reference To Player 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    // Switch Statesment For Each Player State 
    switch (gameState.agentState)
    {
        case STATE_APPEAR:
        {
            // Player's Placement And Look In the Beginning Of The Game 
            obj_agent8.velocity = { 0, -12 };
            obj_agent8.acceleration = { 0, -0.5f };
            Play::SetSprite(obj_agent8, "agent8_fall", 0);
            obj_agent8.rotation = 0;
            if (obj_agent8.pos.y <= DISPLAY_HEIGHT * 0.66f)
            {
                gameState.agentState = STATE_PLAY;
            }
            break;
        }

        case STATE_HALT:
        {
            // Player Stopping 
            obj_agent8.velocity *= 0.9f;
            if (Play::IsAnimationComplete(obj_agent8))
            {
                gameState.agentState = STATE_PLAY;
            }
            break;
        }

        case STATE_PLAY:
        {


            // During Play, Looks for Player Input 
            HandlePlayerControls();
            break;
        }

        case STATE_DEAD:
        {
            // Animation When Getting Hit 
            obj_agent8.acceleration = { -0.3f , 0.5f };
            obj_agent8.rotation += 0.25f;

            // Goes To End Screen When Dead 
            gameState.gameScreen = END_SCREEN;

            break;
        }
    } 

    // Updates The Player Each Time
    Play::UpdateGameObject(obj_agent8);

    // Keep Player In Screen 
    if (Play::IsLeavingDisplayArea(obj_agent8) && gameState.agentState != STATE_DEAD)
    {
        obj_agent8.pos = obj_agent8.oldPos;
    }

    // Web For Player 
    Play::DrawLine({ obj_agent8.pos.x, 720 }, obj_agent8.pos, Play::cWhite);
    Play::DrawObjectRotated(obj_agent8);
}

// Gameloop For Slpash Screen 
void GameloopSplashScreen(float elapsedTime)
{
    gameState.timer += elapsedTime;

    Play::ClearDrawingBuffer(Play::cBlack);
    Play::DrawSprite(0, { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, gameState.timer);
    // Updates Text 
    Play::PresentDrawingBuffer();

    if (gameState.timer > 4)
    {
        gameState.gameScreen = MAIN_MENU;
    }
}

// Gameloop For Main Menu 
bool GameloopMainMenu()
{
    // Black Background
    Play::ClearDrawingBuffer(Play::cBlack);

    // Menu Options 
    Play::DrawFontText("72px", "Menu",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 6 * 5 }, Play::CENTRE);
    
    Play::DrawFontText("64px", "Play",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 6 * 4 }, Play::CENTRE);

    Play::DrawFontText("64px", "Leaderboard",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 6 * 3 }, Play::CENTRE);

    Play::DrawFontText("64px", "Quit",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 6 * 2 }, Play::CENTRE);

    Play::DrawFontText("32px", "Press Enter To Select",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 6 }, Play::CENTRE);
    
    // Input For Choosing Option 
    if (Play::KeyPressed(Play::KEY_DOWN) && gameState.playerMenuChoice < 3)
    {
        gameState.playerMenuChoice++;
    }
    else if (Play::KeyPressed(Play::KEY_UP) && gameState.playerMenuChoice > 1)
    {
        gameState.playerMenuChoice--;
    }

    // Results for Chooing Option 
    switch (gameState.playerMenuChoice) {
        case 1:
        {
            // Play 
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 6 * 4 }, 0);
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                gameState.gameScreen = GAME_SCENE;
            }
            break;
        }
        case 2:
        {
            // Leaderboard 
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 6 * 3 }, 0);

            break;
        }
        case 3:
        {
            // Quit
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 6 * 2 }, 0);
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                return true; 
            }
            break;
        }
        default:
        {
            // Ends Game If There Is A Bug In Menu
            return true;
        }
    }

    // Updates Text 
    Play::PresentDrawingBuffer();
    return false; 
}

// Gameloop For Game Itself 
void GameloopGameScene()
{
    Play::DrawBackground();
    UpdateAgent8();
    UpdateFan();
    UpdateTools();
    UpdateCoinsAndStars();
    UpdateLasers();
    UpdateDestroyed();
    Play::DrawFontText("32px", "ARROW KEYS TO MOVE UP AND DOWN AND SPACE TO FIRE",
        { DISPLAY_WIDTH / 2, 40 }, Play::CENTRE);
    Play::DrawFontText("72px", "SCORE: " + std::to_string(gameState.score),
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);
    Play::DrawFontText("32px", std::to_string(gameState.timer),
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2 }, Play::CENTRE);

    // Updates Text 
    Play::PresentDrawingBuffer();
}

// Gameloop For End Screen
bool GameloopEndScreen()
{
    // Black Background
    Play::ClearDrawingBuffer(Play::cBlack);

    // Menu Options 
    Play::DrawFontText("72px", "You Have Died",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 * 7 }, Play::CENTRE);
    Play::DrawFontText("64px", "Your Score Is: " + std::to_string(gameState.score),
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 * 6 }, Play::CENTRE);
    Play::DrawFontText("64px", "Retry",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 * 5 }, Play::CENTRE);
    Play::DrawFontText("64px", "Leaderboard",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 * 4 }, Play::CENTRE);
    Play::DrawFontText("64px", "Menu",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 * 3 }, Play::CENTRE);
    Play::DrawFontText("64px", "Quit",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 * 2 }, Play::CENTRE);
    Play::DrawFontText("32px", "Press Enter To Select",
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 8 }, Play::CENTRE);

    // Input For Choosing Option 
    if (Play::KeyPressed(Play::KEY_DOWN) && gameState.playerEndScreenChoice < 4)
    {
        gameState.playerEndScreenChoice++;
    }
    else if (Play::KeyPressed(Play::KEY_UP) && gameState.playerEndScreenChoice > 1)
    {
        gameState.playerEndScreenChoice--;
    }

    // Results for Chooing Option 
    switch (gameState.playerEndScreenChoice) {
        case 1:
        {
            // Retry 
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 8 * 5 }, 0);

            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                HandleGameReset();
                gameState.gameScreen = GAME_SCENE;
            }
            break;
        }
        case 2:
        {
            // Leaderboard 
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 8 * 4 }, 0);


            break;
        }
        case 3:
        {
            // Menu 
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 8 * 3 }, 0);

            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                HandleGameReset();
                gameState.gameScreen = MAIN_MENU;
                
            }
            break;
        }
        case 4:
        {
            // Quit 
            Play::DrawSprite(12, { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 8 * 2 }, 0);

            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                return true;
            }
            break;
        }
        default:
        {
            // Ends Game If There Is A Bug In Menu
            return true; 
        }
    }

    // Updates Text 
    Play::PresentDrawingBuffer();

    return false; 
}

// Resets Everything On Screen To Start Game Again 
void HandleGameReset()
{
    // Player Reference 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    // Sets Player's States For Beginning 
    gameState.agentState = STATE_APPEAR;

    // Set Players Settings
    obj_agent8.pos = { 115, 600 };
    obj_agent8.velocity = { 0, 0 };
    obj_agent8.frame = 0;
    Play::StartAudioLoop("music");
    gameState.score = 0;

    // Destoryed all Coins and Tools 
    for (int id_obj : Play::CollectGameObjectIDsByType(TYPE_TOOL))
        Play::GetGameObject(id_obj).type = TYPE_DESTROYED;

    for (int id_obj : Play::CollectGameObjectIDsByType(TYPE_COIN))
        Play::GetGameObject(id_obj).type = TYPE_DESTROYED;
}

