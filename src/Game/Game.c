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
    updateProjectiles(pr, map);
    // render everything (scale unit)
    drawEveryTile(dU, map);
    drawPlayers(dU, p);
    drawProjectiles(dU, pr);
}

int IsThereAnObstacle(Rectangle player, float velocity, bool checkHorizontal, char *map)
{ // arg called player but applies as well to projectiles

    // checks if hitbox collides in a direction (either vertical or horizontal)
    if (checkHorizontal)
    {
        bool res = false;
        for (int i = 0; i < W; i++)
        { // yes it's not fucking optimized but it's starts lagging at 15000 projectiles so ...
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
                p->aimOrientation -= 3.0f;
            if (IsKeyDown(KEY_E))
                p->aimOrientation += 3.0f;
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
                p->aimOrientation += 3.0f;
            if (IsKeyDown(KEY_KP_2))
                p->aimOrientation -= 3.0f;
            break;
        }
        if (p->aimOrientation > 360)
        {
            p->aimOrientation = 0;
        }
        if (p->aimOrientation < 0)
        {
            p->aimOrientation = 360;
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

void updateProjectiles(projectileList *pr, char *map)
{
    for (int i = pr->size - 1; i >= 0; i--) // iterating on each projectile
    {
        projectile *proj = &pr->data[i];
        Rectangle ProjHitbox = {proj->posX - proj->size / 2, proj->posY - proj->size / 2, proj->size, proj->size};
        if (!IsThereAnObstacle(ProjHitbox, proj->speedX, true, map))
        {
            proj->posX += proj->speedX;
        }
        else
        {
            // horizontal wall hit
            if (proj->bounceLeft > 0)
            {
                proj->speedX = -proj->speedX;
                proj->bounceLeft -= 1;
            }
            else
            {
                listRem(i, pr);
                continue; // magic line preventing the skip of an index
                //  because listRem is changing the very list we're iterating on
            }
        }
        if (!IsThereAnObstacle(ProjHitbox, proj->speedY, false, map))
        {
            proj->posY += proj->speedY;
        }
        else
        {
            // vertical wall hit
            if (proj->bounceLeft > 0)
            {
                proj->speedY = -proj->speedY;
                proj->bounceLeft -= 1;
            }
            else
            {
                listRem(i, pr);
                continue; // magic line preventing the skip of an index
                //  because listRem is changing the very list we're iterating on
            }
        }
    }
}

void listAdd(projectileList *pr, projectile p)
{

    if (pr->capacity <= pr->size)
    {
        printf("realloc \n");
        projectile *tmp = (projectile *)realloc(pr->data, sizeof(projectile) * (pr->capacity + 8));
        if (tmp == NULL)
        {
            printf("REALLOC FAILED");
            return;
        }

        pr->data = tmp;
        pr->capacity += 8;
    }
    pr->data[pr->size] = p;
    pr->size += 1;
}
void listRem(int projectileIndex, projectileList *pr)
{
    if (projectileIndex < 0 || projectileIndex >= pr->size)
    {
        printf("erreur index");
        return;
    }
    if (projectileIndex < pr->size - 1)
    {
        projectile *ptr = pr->data;

        for (int i = projectileIndex; i < pr->size - 1; i++) // move in the list each projectile after the one selected by the index by overwriting
        {
            ptr[i] = ptr[i + 1];
        }
    }
    pr->size -= 1;
    
    // change capacity
    if (pr->capacity <= pr->size)
    {
        printf("realloc \n");
        projectile *tmp = (projectile *)realloc(pr->data, sizeof(projectile) * (pr->capacity + 8));
        if (tmp == NULL)
        {
            printf("REALLOC FAILED");
            return;
        }

        pr->data = tmp;
        pr->capacity += 8;
    }
}

void spawnBullet(player p, projectileList *pr)
{
    float speedX = 0.1 * cosf(p.aimOrientation * 2 * PI / 360);
    float speedY = 0.1 * sinf(p.aimOrientation * 2 * PI / 360);
    projectile newProjectile = {p.playerId, p.posX, p.posY, speedX, speedY, p.aimOrientation, 1, 2};
    listAdd(pr, newProjectile);
}
void actions(player *players, projectileList *pr)
{
    for (int i = 0; i < 2; i++) // iterating on every player
    {
        player p;

        if (IsKeyPressed(KEY_SPACE))
        {
            p = players[0];
            spawnBullet(p, pr);
        }
        if (IsKeyPressed(KEY_KP_0))
        {
            p = players[1];
            spawnBullet(p, pr);
        }
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

    player p1 = {2, 2, 0.8, 0.1, 10, 0};
    player p2 = {10, 2, 0.8, 0.1, 10, 1};
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
    free(txt);
    CloseWindow();
    return 0;
}
