#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

#include "Game.h"

void mainLoop(player *p, float dU)
{
    // get inputs and move player (in tiles unit)
    movePlayers(p);

    // render everything (scale unit)
    drawEveryTile(dU);
    drawPlayers(dU, p);
}

int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical)
{

    // checks if hitbox collides in a direction (either vertical or horizontal)
    if (checkVertical)
    {
        Rectangle obstacle = {60, 60, 30, 40};
        Rectangle newPlayerPos = {player.x + velocity, player.y, player.width, player.height};
        return CheckCollisionRecs(newPlayerPos, obstacle);
    }
    else
    {
        Rectangle obstacle = {60, 60, 30, 40};
        Rectangle newPlayerPos = {player.x, player.y + velocity, player.width, player.height};
        return CheckCollisionRecs(newPlayerPos, obstacle);
    }
}

void movePlayers(player *p)
{
    // printf("\n\n%d, %d\n\n", p->posX, p->posY);

    // movement
    float x_velocity = 0.0f;
    float y_velocity = 0.0f;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) // careful raylib bug thinking input while there are none (press 3keys same time)
        x_velocity -= p->speed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        x_velocity += p->speed;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        y_velocity -= p->speed;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        y_velocity += p->speed;

    if (fabs(x_velocity) > 0 && fabs(y_velocity) > 0)
    {
        // normalize velocity, si vous comprennez pas cherchez sur internet c normal
        x_velocity /= sqrt(2);
        y_velocity /= sqrt(2);
    }

    // update player position
    Rectangle PlayerHitbox = {p->posX - p->size / 2, p->posY - p->size / 2, p->size, p->size};
    // DrawRectangle(PlayerHitbox.x * 5, PlayerHitbox.y * 5, PlayerHitbox.width * 5, PlayerHitbox.height * 5, RED);

    // checker si collision, si oui, return space left
    if (!IsThereAnObstacle(PlayerHitbox, x_velocity, true))
    {
        p->posX += x_velocity;
    }
    if (!IsThereAnObstacle(PlayerHitbox, y_velocity, false))
    {
        p->posY += y_velocity;
    }
}

void initGame()
{
    char *title = "YskTANKycklaba";

    InitWindow(1000, 640, title);
    int b = GetMonitorWidth(0);
    int a = GetMonitorHeight(0);

    // ToggleBorderlessWindowed();
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    // SetWindowMinSize(1000, 700);
}

int main(void)
{

    initGame();
    player p = {60, 0, 10, 1};
    float dUX = GetScreenWidth() / (float)W;
    float dUY = GetScreenHeight() / (float)H;
    float dU = dUX < dUY ? dUX : dUY;
    printf("%d", W);
    printf("\n\n%f\n\n", dU);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        //---------------- DESSINER ICI POUR CHAQUE FRAME
        float dUX = GetScreenWidth() / (float)W;
        float dUY = GetScreenHeight() / (float)H;
        float dU = dUX < dUY ? dUX : dUY;
        mainLoop(&p, dU);
        //----------------
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
