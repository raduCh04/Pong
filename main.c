#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define LOG(x)            printf("%s\n", x)
#define LOG_ERROR(x)      fprintf(stderr, "Error: %s\n", x)
#define LOG_SDL_ERROR(x)  fprintf(stderr, "%s: %s\n", x, SDL_GetError())
#define global_variable   static
#define internal_function static
#define SCREEN_WIDTH      640
#define SCREEN_HEIGHT     480
#define BALL_SIZE         10
#define PADDLE_SIZE       20
#define MAX_SCORE         3

typedef const char* string;
typedef float realNum;

typedef struct Ball
{
    realNum xPos;
    realNum yPos;
    realNum xSpeed;
    realNum ySpeed;
    int size;
} Ball;

typedef struct Player
{
    realNum yPos;
    int score;
    int xPos;
    int width;
    int height;
    int speed;
} Player;

bool flipCoin(void);
bool checkForIntersection(Player *, Ball *);
void updateScore(void);

bool initGame(void);
void update(realNum);
void handleEvent(SDL_Event);
void renderGame(void);
void shutdownGame(void);

Ball makeBall(void);
void renderBall(Ball *);
void updateBall(Ball *, realNum);

Player makePlayer(realNum);
void renderPlayer(Player *);
void updatePlayer(realNum);


bool served = false;
global_variable bool Running;
global_variable SDL_Window *g_pWindow;
global_variable SDL_Renderer *g_pRenderer;
Ball *g_pBall       = NULL;
Player *g_pPlayer1  = NULL;
Player *g_pPlayer2  = NULL;

int
main(int argc, char const *argv[])
{
    srand((unsigned int)time(NULL));

    atexit(shutdownGame);

    if (!initGame())
    {
        LOG_ERROR("Failed Initialization");
        exit(1);
    }
    else
    {
        Running = true;
        int fps = 330;
        int desiredDelta = 1000/fps;

        SDL_Event event;

        while (Running)
        {

            renderGame();
            update(desiredDelta);
            handleEvent(event);
            SDL_Delay(rand() % 30);
        }
    }
    
    return (EXIT_SUCCESS);
}

bool
flipCoin()
{
    return (rand() % 2 == 1 ? true : false);
}
bool
checkForIntersection(Player *player, Ball *ball)
{
    SDL_Rect ballRect = {
        .h = ball->size,
        .w = ball->size,
        .x = ball->xPos,
        .y = ball->yPos
    };

    SDL_Rect paddleRect = {
        .h = player->height,
        .w = player->width,
        .x = player->xPos,
        .y = player->yPos
    };

    if (SDL_HasIntersection(&ballRect, &paddleRect))
    {
        return (true);
    }
    return (false);
}

void updateScore()
{
    if (g_pPlayer1->score == MAX_SCORE || g_pPlayer2->score == MAX_SCORE)
    {
       g_pPlayer1->score = 0;
       g_pPlayer2->score = 0;
    }
    string fmt = "Player1: %d | Player2: %d";
    int len = snprintf(NULL, 0, fmt, g_pPlayer1->score, g_pPlayer2->score);
    char buf[len +1];
    snprintf(buf, len + 1, fmt, g_pPlayer1->score, g_pPlayer2->score);
    SDL_SetWindowTitle(g_pWindow, buf);
    }

bool
initGame()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        LOG_SDL_ERROR("Failed initialization: ");

        return (false);
    }
    else
    {
        g_pWindow   = 
        SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        g_pRenderer = 
        SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        g_pBall     = (Ball*)malloc(sizeof(Ball));
        g_pPlayer1  = (Player*)malloc(sizeof(Player));
        g_pPlayer2  = (Player*)malloc(sizeof(Player));

        *g_pBall    =  makeBall();
        *g_pPlayer1 = makePlayer(15);
        *g_pPlayer2 = makePlayer(SCREEN_WIDTH - 25);
        

        if ((g_pBall || g_pPlayer1 || g_pPlayer2 || g_pRenderer || g_pWindow) == NULL)
        {
            return (false);
        }

        g_pWindow   == NULL ?  LOG_ERROR("Window failed")   : LOG("Succes Window");
        g_pRenderer == NULL ?  LOG_ERROR("Renderer failed") : LOG("Succes Renderer");
        g_pBall     == NULL ?  LOG_ERROR("Ball failed")     : LOG("Succes Ball");
        g_pPlayer1  == NULL ?  LOG_ERROR("Player1 failed")  : LOG("Succes Player1");
        g_pPlayer2  == NULL ?  LOG_ERROR("Player2 failed")  : LOG("Succes Player2");

        LOG("Game initialized");
    }

    return (true);
}

void
renderGame()
{
    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(g_pRenderer);
    renderBall(g_pBall);
    renderPlayer(g_pPlayer1);
    renderPlayer(g_pPlayer2);
    SDL_RenderPresent(g_pRenderer);
}

void
update(realNum elapsed)
{
    updateBall(g_pBall, elapsed);
    updatePlayer(elapsed);
    updateScore();
}

void
handleEvent(SDL_Event event)
{
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
    {
        Running = false;
    } break;

    default:
        break;
    }
}

void
shutdownGame()
{
    if (g_pWindow)
    {
        SDL_DestroyWindow(g_pWindow);
    }

    if (g_pRenderer)
    {
        SDL_DestroyRenderer(g_pRenderer);
    }

    if (g_pBall)
    {
        free(g_pBall);
    }

    if (g_pPlayer1)
    {
        free(g_pPlayer1);
    }

    if (g_pPlayer2)
    {
        free(g_pPlayer2);
    }

    SDL_Quit();

    LOG("Game shutdowned");
}

Ball
makeBall()
{
    const int speed = 1;
    Ball ball = {
        .size   = BALL_SIZE,
        .xSpeed = speed * (flipCoin() ? 1 : -1),
        .ySpeed = speed * (flipCoin() ? 1 : -1),
        .yPos   = SCREEN_HEIGHT / 2 - BALL_SIZE / 2,
        .xPos   = SCREEN_WIDTH / 2 - BALL_SIZE / 2};
    return (ball);
}

void
renderBall(Ball *ball)
{
    SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 255, 255);
    SDL_Rect ballRect = {
        .h = ball->size,
        .w = ball->size,
        .x = ball->xPos,
        .y = ball->yPos};
    SDL_RenderFillRect(g_pRenderer, &ballRect);
}

void
updateBall(Ball *ball, realNum elapsed)
{

    if (!served)
    {
        ball->xPos = SCREEN_WIDTH / 2;
        ball->yPos = SCREEN_HEIGHT / 2;
        g_pPlayer1->yPos = SCREEN_HEIGHT / 2 - PADDLE_SIZE * 2;
        g_pPlayer2->yPos = SCREEN_HEIGHT / 2 - PADDLE_SIZE * 2;
        return;
    }

    ball->xPos += ball->xSpeed * elapsed;
    ball->yPos += ball->ySpeed * elapsed;

    if (ball->xPos < 0)
    {
        g_pPlayer2->score++;
        served = false;
    }
    if (checkForIntersection(g_pPlayer1, g_pBall))
    {
        ball->xSpeed = fabs(ball->xSpeed);
    }
    if (ball->xPos >= SCREEN_WIDTH - ball->size)
    {
        g_pPlayer1->score++;
        served = false;
    }
     if (checkForIntersection(g_pPlayer2, g_pBall))
    {
        ball->xSpeed = -fabs(ball->xSpeed);
    }
    if (ball->yPos >= SCREEN_HEIGHT - ball->size)
    {
        ball->ySpeed = -fabs(ball->ySpeed);
    }
    if (ball->yPos < 0)
    {
        ball->ySpeed = fabs(ball->ySpeed);
    }
}

Player
makePlayer(realNum x)
{
    const int speed = 1;
    Player player = {
        .xPos   = x,
        .yPos   = SCREEN_HEIGHT / 2 - PADDLE_SIZE * 2,
        .score  = 0,
        .height = PADDLE_SIZE * 4,
        .width  = PADDLE_SIZE / 2,
        .speed  = speed
    };

    return (player);
}

void
renderPlayer(Player* player)
{
    SDL_Rect paddleRect = {
        .h = player->height,
        .w = player->width,
        .x = player->xPos,
        .y = player->yPos
    };
    SDL_RenderFillRect(g_pRenderer, &paddleRect);
}

void
updatePlayer(realNum elapsed)
{
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_SPACE])
    {
        served = true;
    }
    if (keyboardState[SDL_SCANCODE_W] && g_pPlayer1->yPos > 0) 
    {
        g_pPlayer1->yPos -= g_pPlayer1->speed * elapsed;
    }
    if (keyboardState[SDL_SCANCODE_S] && g_pPlayer1->yPos < (SCREEN_HEIGHT - PADDLE_SIZE * 4 ))
    {
        g_pPlayer1->yPos += g_pPlayer1->speed * elapsed;
    }
    if (keyboardState[SDL_SCANCODE_UP] && g_pPlayer2->yPos > 0)
    {
        g_pPlayer2->yPos -= g_pPlayer2->speed * elapsed;
    }
    if (keyboardState[SDL_SCANCODE_DOWN] && g_pPlayer2->yPos < (SCREEN_HEIGHT - PADDLE_SIZE * 4 ))
    {
        g_pPlayer2->yPos += g_pPlayer2->speed * elapsed;
    }
}