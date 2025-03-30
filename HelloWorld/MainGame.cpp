#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include <fstream>
#include <algorithm>

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
    SPLASH_SCREEN = 0,
    MAIN_MENU,
    GAME_SCENE,
    LEADERBOARD,
    END_SCREEN,
};

struct MenuDisplay
{
    const char* fontSize = "32px";
    const std::string menuContent = "Not Set";
    const int menuXLocation = 0;
    const int menuYLocation = 0;
};

// Game Information 
struct GameState
{
    // Timer For Splash Screen 
    float timer = 0.0f;

    // Score For Game 
    int score{ 0 };

    // Coin Frenzy Mode 
    float coinFrenzyTimer = 0.0f;
    bool bIsCoinFrenzy = false;

    // Sheild 
    bool bIsSheildOn = false; 

    // Score Multiplier 
    float scoreMultiplier = 1.0f;
    int toolAttackStreak = 0;

    // Menu Choices 
    bool bShouldSoundBeOn = false;
    bool bIsMusicOn = false; 

    // Menu Choices
    int playerMenuChoice = 1;
    int playerLeaderboardChoice = 1;
    int playerEndScreenChoice = 1;

    //Leaderboard Information 
    std::vector<int> leaderboardTopScore;

    // States for Agent And Scenes 
    Agent8State agentState{ STATE_APPEAR };
    GameloopScreen gameScreen{ SPLASH_SCREEN };
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
    TYPE_COIN_POWER,
    TYPE_SHIELD,
    TYPE_STAR,
    TYPE_LASER,
    TYPE_DESTROYED,
};

// Declarations 
void HandlePlayerControls();
void UpdateFan();
void UpdateTools();
void UpdateCoinsAndStars();
void UpdateCoinPower();
void UpdateShield();
void UpdateLasers();
void UpdateDestroyed();
void UpdateAgent8();
void GameloopSplashScreen();
void DisplayMainMenu();
bool ChooseMainMenu();
void GameloopGameScene(const float elapsedTime);
void ResetGameScreen();
void DisplayEndScreen();
bool ChooseEndScreen();
void DisplayLeaderBoardMenu();
void DisplayLeaderBoardScores();
bool ChooseLeaderBoard();
void SetUpLeaderBoard();
void UploadScore();
void SetUpSound();
void UpdateSound();

// Entry Point 
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE) 
{
    // Sets Up Intial State of Game 
    Play::CreateManager(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE);
    Play::CentreAllSpriteOrigins();
    Play::LoadBackground("Data\\Backgrounds\\background.png");
    SetUpSound();
    if (gameState.bShouldSoundBeOn)
    {
        Play::StartAudioLoop("music");
    }
    Play::CreateGameObject(TYPE_AGENT8, { 115, 600 }, 50, "agent8");
    int id_fan = Play::CreateGameObject(TYPE_FAN, { 1140, 503 }, 0, "fan");
    Play::GetGameObject(id_fan).velocity = { 0, -3 };
    Play::GetGameObject(id_fan).animSpeed = 1.0f;
    SetUpLeaderBoard();
}

// Updates Game Every 60 Times Per Second 
bool MainGameUpdate(float elapsedTime)
{
    gameState.timer += elapsedTime;

    // Gameloop: Switch To Different Menus And Screens 
    switch (gameState.gameScreen)
    {
        case SPLASH_SCREEN:
        {
            GameloopSplashScreen();
            break;
        }
        case MAIN_MENU:
        {
            DisplayMainMenu();
            return ChooseMainMenu();
            break;
        }
        case GAME_SCENE:
        {
            GameloopGameScene(elapsedTime);
            break;
        }
        case LEADERBOARD:
        {
            DisplayLeaderBoardMenu();
            DisplayLeaderBoardScores();
            return ChooseLeaderBoard();
            break;
        }
        case END_SCREEN:
        {
            DisplayEndScreen();
            return ChooseEndScreen();
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
        if (gameState.bShouldSoundBeOn)
        {
            Play::PlayAudio("shoot");
        }
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
    if (Play::RandomRoll(50) == 50 && !gameState.bIsCoinFrenzy)
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
        if (gameState.bShouldSoundBeOn)
        {
            Play::PlayAudio("tool");
        }
    }

    // 1/150 Chance Of Spawning Coin and 1/50 if Coin Frenzy Is On 
    int coinChance = 150; 

    if (gameState.bIsCoinFrenzy)
    {
        coinChance = 20;
    }

    if (Play::RandomRoll(coinChance) == 1)
    {
        
        if (Play::RandomRoll(5) == 1 && !gameState.bIsCoinFrenzy)
        {
            int id = Play::CreateGameObject(TYPE_COIN_POWER, obj_fan.pos, 25, "coin_power");
            GameObject& obj_coin = Play::GetGameObject(id);
            obj_coin.velocity = { -3, 0 };
        }
        else
        {
            // Sets Coin Sprite And Animation and Movement 
            int id = Play::CreateGameObject(TYPE_COIN, obj_fan.pos, 40, "coin");
            GameObject& obj_coin = Play::GetGameObject(id);
            obj_coin.velocity = { -3, 0 };
            obj_coin.rotSpeed = 0.1f;
        }
    }

    // Roll A Shield 
    if (Play::RandomRoll(2000) == 1)
    {

        // Sets Coin Sprite And Animation and Movement 
        int id = Play::CreateGameObject(TYPE_SHIELD, obj_fan.pos, 30, "shield");
        GameObject& obj_sheild = Play::GetGameObject(id);
        obj_sheild.velocity = { -3, 0 };
        
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
            if (!gameState.bIsSheildOn)
            {
                
                // Changes Players State To Dead After Getting Hit 
                if (gameState.bShouldSoundBeOn)
                {
                    Play::StopAudio("music");
                    Play::PlayAudio("die");
                    gameState.bIsMusicOn = false;
                }
                gameState.agentState = STATE_DEAD;
            }
            else
            {
                ResetGameScreen();
            }
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
            gameState.score += (int)(500 * gameState.scoreMultiplier);
            if (gameState.bShouldSoundBeOn)
            {
                Play::PlayAudio("collect");
            }
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

void UpdateCoinPower()
{
    // Player Reference and Coin Vector 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vCoinPowers = Play::CollectGameObjectIDsByType(TYPE_COIN_POWER);

    // Looks Over Each Coin In Vector 
    for (int id_coin : vCoinPowers)
    {
        // Reference For Coin Object 
        GameObject& obj_coin = Play::GetGameObject(id_coin);
        bool hasCollided = false;

        // If Player Has Collided With Coin Power Up 
        if (Play::IsColliding(obj_coin, obj_agent8))
        {
            // More Points and Turn Frency On And Reset Timer
            hasCollided = true;
            gameState.score += (int)(1000 * gameState.scoreMultiplier);
            gameState.bIsCoinFrenzy = true;
            gameState.coinFrenzyTimer = 0.0f;
            if (gameState.bShouldSoundBeOn)
            {
                Play::PlayAudio("collect");
            }
        }

        // Updates Coin Power Up 
        Play::UpdateGameObject(obj_coin);

        // Rotate Coin Power Up 
        int frame = (int)gameState.timer * 11; 
        Play::DrawSprite("coin_power", obj_coin.pos, frame % 11);
 
        // If Coin Has Been Hit Or Collided Then Desttory It 
        if (!Play::IsVisible(obj_coin) || hasCollided)
        {
            Play::DestroyGameObject(id_coin);
        }
    }
}

void UpdateShield()
{
    // Player Reference and Coin Vector 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);
    std::vector<int> vShield = Play::CollectGameObjectIDsByType(TYPE_SHIELD);

    // Looks Over Each Shield In Vector 
    for (int id_shield : vShield)
    {
        // Reference For Shield Object 
        GameObject& obj_shield = Play::GetGameObject(id_shield);
        bool hasCollided = false;

        // If Player Has Collided With Sheild Power Up 
        if (Play::IsColliding(obj_shield, obj_agent8))
        {
            // Turn On Sheild When Colliding 
            hasCollided = true;
            gameState.score += (int)(500 * gameState.scoreMultiplier);
            gameState.bIsSheildOn = true; 

            if (gameState.bShouldSoundBeOn)
            {
                Play::PlayAudio("collect");
            }
        }

        // Updates Coin Power Up 
        Play::UpdateGameObject(obj_shield);

        // Rotate Coin Power Up 
        Play::DrawSprite("shield", obj_shield.pos, 0);

        // If Coin Has Been Hit Or Collided Then Desttory It 
        if (!Play::IsVisible(obj_shield) || hasCollided)
        {
            Play::DestroyGameObject(id_shield);
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
    std::vector<int> vCoinPowers = Play::CollectGameObjectIDsByType(TYPE_COIN_POWER);
    std::vector<int> vShields = Play::CollectGameObjectIDsByType(TYPE_SHIELD);

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
                gameState.score += (int)(100 * gameState.scoreMultiplier);
                gameState.toolAttackStreak++; 

                // Increase Score Multiplier For Every 5 Attacks
                if (gameState.toolAttackStreak % 5 == 0)
                {
                    gameState.scoreMultiplier += .5;
                }
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
                if (gameState.bShouldSoundBeOn)
                {
                    Play::PlayAudio("error");
                }
                gameState.score -= 300;

                // Resets Streak 
                gameState.toolAttackStreak = 0;
                gameState.scoreMultiplier = 1;
            }
        }

        for (int id_coin_power : vCoinPowers)
        {
            // Each Coin 
            GameObject& obj_coin_power = Play::GetGameObject(id_coin_power);
            // Checks If Lazer Hits Coin 
            if (Play::IsColliding(obj_laser, obj_coin_power))
            {
                // Changes Type Of Coin To Destroyed And Decreases Score 
                hasCollided = true;
                obj_coin_power.type = TYPE_DESTROYED;
                if (gameState.bShouldSoundBeOn)
                {
                    Play::PlayAudio("error");
                }
                gameState.score -= 1000;

                // Resets Streak 
                gameState.toolAttackStreak = 0;
                gameState.scoreMultiplier = 1;
            }
        }

        for (int id_shield : vShields)
        {
            // Each Shield 
            GameObject& obj_shield = Play::GetGameObject(id_shield);
            // Checks If Lazer Hits Shield 
            if (Play::IsColliding(obj_laser, obj_shield))
            {
                // Changes Type Of Shield To Destroyed
                hasCollided = true;
                obj_shield.type = TYPE_DESTROYED;

                if (gameState.bShouldSoundBeOn)
                {
                    Play::PlayAudio("error");
                }
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

            // Updates Score
            UploadScore();

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
void GameloopSplashScreen()
{
    // Look Of Splash Screen 
    Play::ClearDrawingBuffer(Play::cBlack);
    Play::DrawSprite("playbuffer_title", {DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2}, 0); 
    Play::PresentDrawingBuffer();

  

    // 4 Seconds In Slpash Screen
    if (gameState.timer > 4)
    {
        gameState.gameScreen = MAIN_MENU;
    }
}

void DisplayMainMenu()
{
    Play::ClearDrawingBuffer(Play::cBlack);

    const int menuOptionNumber = 6;
    const int menuX = DISPLAY_WIDTH / 2;
    const int menuY = DISPLAY_HEIGHT / 7;

    MenuDisplay mainMenuDisplayArray[menuOptionNumber] =
    {
        {"72px", "Menu", menuX, menuY},
        {"64px", "Play", menuX, menuY},
        {"64px", "Leaderboard", menuX, menuY},
        {"64px", "Turn Sound ", menuX, menuY},
        {"64px", "Quit", menuX, menuY},
        {"32px", "Press Enter To Select", menuX, menuY}
    };

    for (int i = 0; i < menuOptionNumber; i++)
    {

        if (i != 3)
        {
            Play::DrawFontText(mainMenuDisplayArray[i].fontSize, mainMenuDisplayArray[i].menuContent,
                { mainMenuDisplayArray[i].menuXLocation, mainMenuDisplayArray[i].menuYLocation * (menuOptionNumber - i)}, Play::CENTRE);
        }
        else
        {
            if (gameState.bShouldSoundBeOn)
            {
                Play::DrawFontText(mainMenuDisplayArray[i].fontSize, mainMenuDisplayArray[i].menuContent + "Off",
                    { mainMenuDisplayArray[i].menuXLocation, mainMenuDisplayArray[i].menuYLocation * (menuOptionNumber - i) }, Play::CENTRE);
            }
            else
            {
                Play::DrawFontText(mainMenuDisplayArray[i].fontSize, mainMenuDisplayArray[i].menuContent + "On",
                    { mainMenuDisplayArray[i].menuXLocation, mainMenuDisplayArray[i].menuYLocation * (menuOptionNumber - i) }, Play::CENTRE);
            }
        }
    }
}

// Gameloop For Main Menu 
bool ChooseMainMenu()
{

    // Input For Choosing Option 
    if (Play::KeyPressed(Play::KEY_DOWN) && gameState.playerMenuChoice < 4)
    {
        gameState.playerMenuChoice++;
    }
    else if (Play::KeyPressed(Play::KEY_UP) && gameState.playerMenuChoice > 1)
    {
        gameState.playerMenuChoice--;
    }

    // Lazer Pointer 
    Play::DrawSprite("laser", { DISPLAY_WIDTH / 2 - 300, DISPLAY_HEIGHT / 7 * (6 - gameState.playerMenuChoice)}, 0);

    // Results for Chooing Option 
    switch (gameState.playerMenuChoice) 
    {
        case 1:
        {
            // Play 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                gameState.gameScreen = GAME_SCENE;
            }
            break;
        }
        case 2:
        {
            // Leaderboard 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                gameState.gameScreen = LEADERBOARD;
            }
            break;
        }
        case 3:
        {
            // Music 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                if (gameState.bShouldSoundBeOn)
                {
                    Play::StopAudio("music");
                    gameState.bShouldSoundBeOn = false;
                    gameState.bIsMusicOn = false; 
                }
                else
                {
                    gameState.bShouldSoundBeOn = true;
                    Play::StartAudioLoop("music");
                    gameState.bIsMusicOn = true;
                }
                UpdateSound();
            }
            break;
        }
        case 4:
        {
            // Quit
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
void GameloopGameScene(const float elapsedTime)
{
    Play::DrawBackground();
    UpdateAgent8();
    UpdateFan();
    UpdateTools();
    UpdateCoinsAndStars();
    UpdateCoinPower();
    UpdateShield();
    UpdateLasers();
    UpdateDestroyed();
    Play::DrawFontText("32px", "ARROW KEYS TO MOVE UP AND DOWN AND SPACE TO FIRE",
        { DISPLAY_WIDTH / 2, 40 }, Play::CENTRE);
    Play::DrawFontText("72px", "SCORE: " + std::to_string(gameState.score),
        { DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 80 }, Play::CENTRE);
    Play::DrawFontText("32px", "Multi: x" + std::to_string(std::round(gameState.scoreMultiplier * 10.0f) / 10.0f).substr(0,4),
        { DISPLAY_WIDTH / 2 + 500, DISPLAY_HEIGHT - 80 }, Play::CENTRE);

    if (gameState.bIsSheildOn)
    {
        Play::DrawFontText("32px", "Shield: ON",
            { DISPLAY_WIDTH / 2 + 500, DISPLAY_HEIGHT - 120 }, Play::CENTRE);
    }
    else 
    {
        Play::DrawFontText("32px", "Shield: OFF",
            { DISPLAY_WIDTH / 2 + 500, DISPLAY_HEIGHT - 120 }, Play::CENTRE);
    }


    // Display and Turn Off Coin Frenzy After 10 
    if (gameState.bIsCoinFrenzy)
    {
        gameState.coinFrenzyTimer += elapsedTime;
        if (gameState.coinFrenzyTimer > 5)
        {
            gameState.bIsCoinFrenzy = false;
        }
    }

    // Updates Text 
    Play::PresentDrawingBuffer();
}

// Resets Everything On Screen To Start Game Again 
void ResetGameScreen()
{
    // Player Reference 
    GameObject& obj_agent8 = Play::GetGameObjectByType(TYPE_AGENT8);

    // Sets Player's States For Beginning 
    gameState.agentState = STATE_APPEAR;

    // Set Players Settings
    obj_agent8.pos = { 115, 600 };
    obj_agent8.velocity = { 0, 0 };
    obj_agent8.frame = 0;

    // Turn On Music When Reset
    if (gameState.bShouldSoundBeOn && !gameState.bIsMusicOn)
    {
        Play::StartAudioLoop("music");
        gameState.bIsMusicOn = true; 
    }

    // Doesn't Reset Score Is Shield Is Active 
    if (!gameState.bIsSheildOn)
    {
        gameState.score = 0;
    }

    // Reset Muliplier, Streak, Coin Frenzy, And Shield When Hit 
    gameState.bIsCoinFrenzy = false;
    gameState.scoreMultiplier = 1;
    gameState.toolAttackStreak = 0;


    gameState.bIsSheildOn = false; 

    // Destoryed all Coins, Coin Power Ups, Shields, and Tools 
    for (int id_obj : Play::CollectGameObjectIDsByType(TYPE_TOOL))
    {
        Play::GetGameObject(id_obj).type = TYPE_DESTROYED;
    }

    for (int id_obj : Play::CollectGameObjectIDsByType(TYPE_COIN))
    {
        Play::GetGameObject(id_obj).type = TYPE_DESTROYED;
    }

    for (int id_obj : Play::CollectGameObjectIDsByType(TYPE_COIN_POWER))
    {
        Play::GetGameObject(id_obj).type = TYPE_DESTROYED;
    }

    for (int id_obj : Play::CollectGameObjectIDsByType(TYPE_SHIELD))
    {
        Play::GetGameObject(id_obj).type = TYPE_DESTROYED;
    }
}


void DisplayEndScreen()
{
    // Reset Screen 
    Play::ClearDrawingBuffer(Play::cBlack);

    // Menu Settings 
    const int menuOptionNumber = 7;
    const int menuX = DISPLAY_WIDTH / 2;
    const int menuY = DISPLAY_HEIGHT / 8;

    MenuDisplay endMenuDisplayArray[menuOptionNumber] =
    {
        {"72px", "You Have Died", menuX, menuY},
        {"64px", "Your Score Is: " + std::to_string(gameState.score), menuX, menuY},
        {"64px", "Retry", menuX, menuY},
        {"64px", "Leaderboard", menuX, menuY},
        {"64px", "Menu", menuX, menuY},
        {"64px", "Quit", menuX, menuY},
        {"32px", "Press Enter To Select", menuX, menuY}
    };

    // Shows Displays 
    for (int i = 0; i < menuOptionNumber; i++)
    {
        Play::DrawFontText(endMenuDisplayArray[i].fontSize, endMenuDisplayArray[i].menuContent,
            { endMenuDisplayArray[i].menuXLocation, endMenuDisplayArray[i].menuYLocation * (menuOptionNumber - i) }, Play::CENTRE);
    }
}

// Gameloop For End Screen
bool ChooseEndScreen()
{
    // Input For Choosing Option 
    if (Play::KeyPressed(Play::KEY_DOWN) && gameState.playerEndScreenChoice < 4)
    {
        gameState.playerEndScreenChoice++;
    }
    else if (Play::KeyPressed(Play::KEY_UP) && gameState.playerEndScreenChoice > 1)
    {
        gameState.playerEndScreenChoice--;
    }

    // Lazer Pointer 
    Play::DrawSprite("laser", { DISPLAY_WIDTH / 2 - 200, DISPLAY_HEIGHT / 8 * (6 - gameState.playerEndScreenChoice)}, 0);

    // Results for Chooing Option 
    switch (gameState.playerEndScreenChoice) {
        case 1:
        {
            // Retry 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                ResetGameScreen();
                gameState.gameScreen = GAME_SCENE;
            }
            break;
        }
        case 2:
        {
            // Leaderboard 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                gameState.gameScreen = LEADERBOARD;
            }

            break;
        }
        case 3:
        {
            // Menu 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                ResetGameScreen();
                gameState.gameScreen = MAIN_MENU;
            }
            break;
        }
        case 4:
        {
            // Quit 
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

// Display Leaderboard Options 
void DisplayLeaderBoardMenu()
{
    // Resets Screen
    Play::ClearDrawingBuffer(Play::cBlack);

    // Settings for menu 
    const int menuOptionNumber = 3;
    const int menuX = DISPLAY_WIDTH / 2 + 400;
    const int menuY = DISPLAY_HEIGHT / 4;

    MenuDisplay leaderMenuDisplayArray[menuOptionNumber] =
    {
        {"32px", "Menu", menuX, menuY},
        {"32px", "Reset Leaderboard", menuX, menuY},
        {"32px", "Quit", menuX, menuY}
    };

    // Print Each Setting Display 
    for (int i = 0; i < menuOptionNumber; i++)
    {
        Play::DrawFontText(leaderMenuDisplayArray[i].fontSize, leaderMenuDisplayArray[i].menuContent,
            { leaderMenuDisplayArray[i].menuXLocation,leaderMenuDisplayArray[i].menuYLocation * (menuOptionNumber - i) }, Play::CENTRE);
    }
}

// Display Leaderboard scores 
void DisplayLeaderBoardScores()
{
    const int scoreAmount = 8;
    const int menuX = DISPLAY_WIDTH / 2 - 400;
    const int menuY = DISPLAY_HEIGHT / 10;

    // Title 

    Play::DrawFontText("64px", "Leaderboard Top 8",
        { menuX, menuY * 9 }, Play::CENTRE);

    // To Prevent Crash From Less Than 8 Scores 
    if (gameState.leaderboardTopScore.size() < 8)
    {
        gameState.leaderboardTopScore.resize(gameState.leaderboardTopScore.size() + 8 - gameState.leaderboardTopScore.size(), 0);
    }

    // Score Display 
    for (int i = 0; i < scoreAmount; i++)
    {
        Play::DrawFontText("32px", std::to_string(i + 1) + ". " + std::to_string(gameState.leaderboardTopScore[i]),
            { menuX,menuY * (scoreAmount - i) }, Play::CENTRE);
    }
}

// Chosing Leaderboard Scores 
bool ChooseLeaderBoard()
{

    // Input For Choosing Option 
    if (Play::KeyPressed(Play::KEY_DOWN) && gameState.playerLeaderboardChoice < 3)
    {
        gameState.playerLeaderboardChoice++;
    }
    else if (Play::KeyPressed(Play::KEY_UP) && gameState.playerLeaderboardChoice > 1)
    {
        gameState.playerLeaderboardChoice--;
    }

    // Lazer Pointer
    Play::DrawSprite("laser", { DISPLAY_WIDTH / 2 + 200, DISPLAY_HEIGHT / 4 * (4 - gameState.playerLeaderboardChoice)}, 0);

    // Results for Chooing Option 
    switch (gameState.playerLeaderboardChoice) {
        case 1:
        {
            // Menu 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                ResetGameScreen();
                gameState.gameScreen = MAIN_MENU;

            }
            break;
        }
        case 2:
        {
            // Reset Leaderboard 
            if (Play::KeyPressed(Play::KEY_ENTER))
            {
                gameState.leaderboardTopScore.assign(8, 0);
                std::ofstream LeaderboardFile("Leaderboard.txt", std::ios::trunc);
            }
            break;
        }
        case 3:
        {
            // Quit
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

// Set Up Leaderboard From Previous Games 
void SetUpLeaderBoard()
{
    int scoreFromLeaderboard = 0;

    std::ifstream LeaderboardRead("Leaderboard.txt");
    if (LeaderboardRead.is_open())
    {
        while (LeaderboardRead >> scoreFromLeaderboard)
        {
            // Add All Scores To Current Vector 
            gameState.leaderboardTopScore.push_back(scoreFromLeaderboard);
        }
        LeaderboardRead.close();
    }
    std::sort(gameState.leaderboardTopScore.begin(), gameState.leaderboardTopScore.end(), std::greater<>());
}

// Updates New Score 
void UploadScore()
{
    // Upload To File For Future Games 
    std::ofstream LeaderboardWrite("Leaderboard.txt", std::ios::app);
    if (LeaderboardWrite.is_open()) {

        LeaderboardWrite << std::endl << gameState.score;

        LeaderboardWrite.close();
    }
    std::ifstream LeaderboardRead("Leaderboard.txt");

    // In Game Upload
    gameState.leaderboardTopScore.push_back(gameState.score);
    std::sort(gameState.leaderboardTopScore.begin(), gameState.leaderboardTopScore.end(), std::greater<>());
}

// Sets Up Sound From Previous Game 
void SetUpSound()
{
    int soundSetting = 0;

    std::ifstream SettingRead("Settings.txt");
    if (SettingRead.is_open())
    {
        while (SettingRead >> soundSetting)
        {
            gameState.bShouldSoundBeOn = soundSetting;
        }
        SettingRead.close();
    }

    // Sets Sound For Current Game 
    if (soundSetting == 0)
    {
        gameState.bIsMusicOn = false;
    }
    else if (soundSetting == 1)
    {
        gameState.bIsMusicOn = true;

    }
}

// Update Sound For Future And Current Game 
void UpdateSound()
{
    int soundSetting = 0; 
    if (gameState.bShouldSoundBeOn)
    {
        soundSetting = 1;
        gameState.bIsMusicOn = true;
    }
    else
    {
        soundSetting = 0;
        gameState.bIsMusicOn = false;
    }

    // Changes TXT file 
    std::ofstream SettingWrite("Settings.txt");
    if (SettingWrite.is_open())
    {

        SettingWrite << soundSetting;
 
        SettingWrite.close();
    }
}