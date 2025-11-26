#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
// pr lancer : gcc YskTANKycklaba/src/Game/*.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -I./raylib/src -L./raylib/src && ./a.out
#include "Game.h"

void mainLoop(player *p, projectileList *pr, float dU, char *map)
{
    // get inputs and move player (in tiles unit)
    movePlayers(p, map);
    actions(p, pr);
    // render everything (scale unit)
    drawEveryTile(dU, map);
    drawPlayers(dU, p);
}

int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical, char *map)
{

    // checks if hitbox collides in a direction (either vertical or horizontal)
    if (checkVertical)
    {
        bool res = false;
        for (int i = 0; i < W; i++)
        {
            for (int j = 0; j < H; j++)
            {
                if (map[(j * W + i) * 2] == '1')
                {
                    Rectangle newPlayerPos = {player.x + velocity, player.y, player.width, player.height};
                    Rectangle obstacle = {i, j, 1, 1};
                    if (CheckCollisionRecs(newPlayerPos, obstacle))
                    {
                        res = true;
                        break;
                    }
                }
            }
            if (res)
                break;
        }

        return res;
    }
    else
    {
        bool res = false;
        for (int i = 0; i < W; i++)
        {
            for (int j = 0; j < H; j++)
            {
                if (map[(j * W + i) * 2] == '1')
                {
                    Rectangle newPlayerPos = {player.x, player.y + velocity, player.width, player.height};
                    Rectangle obstacle = {i, j, 1, 1};
                    if (CheckCollisionRecs(newPlayerPos, obstacle))
                    {
                        res = true;
                        break;
                    }
                }
            }
            if (res)
                break;
        }

        return res;
    }
}

void movePlayers(player *players, char *map)
{
    for (int i = 0; i < 2; i++)
    {
        player *p = &players[i];
        // movement
        float x_velocity = 0.0f;
        float y_velocity = 0.0f;

        switch (i) // mouvement pour chaque joueur
        {
        case 0:
            if (IsKeyDown(KEY_A))
                x_velocity -= p->speed;
            if (IsKeyDown(KEY_D))
                x_velocity += p->speed;
            if (IsKeyDown(KEY_W))
                y_velocity -= p->speed;
            if (IsKeyDown(KEY_S))
                y_velocity += p->speed;
            if (IsKeyDown(KEY_Q))
                p->aimOrientation -= 5.0f;
            if (IsKeyDown(KEY_E))
                p->aimOrientation += 5.0f;
            break;

        case 1:
            if (IsKeyDown(KEY_LEFT))
                x_velocity -= p->speed;
            if (IsKeyDown(KEY_RIGHT))
                x_velocity += p->speed;
            if (IsKeyDown(KEY_UP))
                y_velocity -= p->speed;
            if (IsKeyDown(KEY_DOWN))
                y_velocity += p->speed;
            if (IsKeyDown(KEY_KP_1))
                p->aimOrientation += 5.0f;
            if (IsKeyDown(KEY_KP_2))
                p->aimOrientation -= 5.0f;
            break;
        }

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
        if (!IsThereAnObstacle(PlayerHitbox, x_velocity, true, map))
        {
            p->posX += x_velocity;
        }
        if (!IsThereAnObstacle(PlayerHitbox, y_velocity, false, map))
        {
            p->posY += y_velocity;
        }
    }
}

void listAdd(projectileList *pr, projectile p)
{
    if (pr->capacity <= pr->size)
    {
        pr->data = (projectile *)realloc(pr->data, sizeof(projectile) * (pr->capacity + 8));
        pr->capacity += 8;
    }
    pr->data[pr->size] = p;
    pr->size += 1;
    printf("taille : %d, capa : %d\n", pr->size, pr->capacity);
    for (int i=0;i<pr->size;i++){
        projectile c = pr->data[i];

    }
}
void listRem() {}

void actions(player *players, projectileList *pr)
{
    // printf("%d\n", pr->size);
    if (IsKeyPressed(KEY_KP_0))
    {
        projectile newProjectile = {0, 0, 0, 0};
        listAdd(pr, newProjectile);
    }
    // printf("%f\n", pr->data[0].posX);
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

    player p1 = {2, 2, 0.8, 0.3, 10};
    player p2 = {10, 2, 0.8, 0.3, 10};
    player p[2] = {p1, p2};
    projectile *ptr = (projectile *)(malloc(sizeof(projectile) * 8)); // initializing projectile list
    projectileList prList = {0, ptr, 8};

    float dUX = GetScreenWidth() / (float)W;
    float dUY = GetScreenHeight() / (float)H;
    float dU = dUX < dUY ? dUX : dUY;

    const char *nom = "./YskTANKycklaba/src/Game/map.txt";
    char *txt = LoadFileText(nom);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        //---------------- DESSINER ICI POUR CHAQUE FRAME
        float dUX = GetScreenWidth() / (float)W;
        float dUY = GetScreenHeight() / (float)H;
        float dU = dUX < dUY ? dUX : dUY; // dynamic resolution rendering
        mainLoop(p, &prList, dU, txt);
        //----------------
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
