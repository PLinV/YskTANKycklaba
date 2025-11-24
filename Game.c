#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define ZONE_JEU_WIDTH 1200
#define ZONE_JEU_HEIGHT 720

typedef struct rect_infos
{
    int left;
    int right;
    int top;
    int bottom;
    Vector2 center;
} rect_infos;

typedef struct player
{
    int posX;
    int posY;
    int speed;
} player;

void mainLoop(player *p)
{

    drawEveryObstacle();
    movePlayers(p);
}
void drawEveryObstacle()
{
    ClearBackground(RAYWHITE);

    // zone de jeu = 400*400
    int a = (int)((GetScreenWidth() - ZONE_JEU_WIDTH) / 2);
    int b = (int)((GetScreenHeight() - ZONE_JEU_HEIGHT) / 2);
    Color t = {100, 199, 100, 100};
    DrawRectangle(a, b, ZONE_JEU_WIDTH, ZONE_JEU_HEIGHT, t);

    // un obstacle
    DrawRectangle(600, 400, 200, 200, RED);
}
bool IsThereAnObstacle(int X, int Y)
{
    if (X > 600 && X < 800) //&& Y >= 400 && Y <= 600
    {
        return 1;
    }
    return 0;
}

void movePlayers(player *p)
{
    printf("\n\n%d, %d\n\n", p->posX, p->posY);

    // movement
    int x_velocity = 0;
    int y_velocity = 0;

    if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT))
        x_velocity -= p->speed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        x_velocity += p->speed;
    if (IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP))
        y_velocity -= p->speed;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        y_velocity += p->speed;

    if (abs(x_velocity) > 0 && abs(y_velocity) > 0)
    {
        x_velocity /= sqrt(2);
        y_velocity /= sqrt(2);
    }

    // update player position

    if (!IsThereAnObstacle(p->posX + x_velocity + 20, p->posY))
    {
        p->posX += x_velocity;
    }
    if (!IsThereAnObstacle(p->posX, p->posY + y_velocity))
    {
        p->posY += y_velocity;
    }

    x_velocity, y_velocity = 0, 0;

    DrawRectangle(p->posX - 20, p->posY - 20, 40, 40, YELLOW);
}

player initGame()
{
    player p1 = {200, 200, 5};
    return p1;
}

int main(void)
{
    char *title = "YskTANKycklaba";
    int b = GetMonitorWidth(0);
    int a = GetMonitorHeight(0);

    InitWindow(a, b, title);
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(ZONE_JEU_WIDTH, ZONE_JEU_HEIGHT);

    player p = initGame();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        //---------------- DESSINER ICI POUR CHAQUE FRAME
        mainLoop(&p);

        //----------------
        EndDrawing();
    }

    CloseWindow();
    return 0;
}