#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
// pr lancer : gcc YskTANKycklaba/src/Game/*.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -I./raylib/src -L./raylib/src && ./a.out
#include "Game.h"

void mainLoop(player *p, projectileList *pr, powerUp *pu, float dU, char *map)
{
    // get inputs and move player (in tiles unit)
    movePlayers(p, map);
    actions(p, pr);
    updateProjectiles(pr, p, map);
    powerUps(pu, map, p, pr);
    // render everything (scale unit)
    drawEveryTile(dU, map);
    drawPlayers(dU, p);
    drawPowerUp(dU, *pu);
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
                if (map[(j * W + i) * 2] == '1' || map[(j * W + i) * 2] == '2')
                {
                    Rectangle newPlayerPos = {player.x + velocity, player.y, player.width, player.height};
                    Rectangle obstacle = {i, j, 1, 1};
                    if (CheckCollisionRecs(newPlayerPos, obstacle))
                    {
                        if (map[(j * W + i) * 2] == '2')
                        {
                            map[(j * W + i) * 2] = '0';
                        }
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

void checkPlayersGettingHit(projectile *proj, Rectangle ProjHitbox, player *p)
{
    for (int j = 0; j < 2; j++)
    {
        if (j == proj->playerId)
        {
            continue;
            // skips for its own player
        }
        else
        {
            player pla = p[j];
            Rectangle PlayerHitbox = {pla.posX - pla.size / 2, pla.posY - pla.size / 2, pla.size, pla.size};
            if (CheckCollisionRecs(ProjHitbox, PlayerHitbox))
            {
                player *tmp = &p[j];
                tmp->posX = 1000000;
                // basically kills it bruh
            }
        }
    }
}

void updateProjectiles(projectileList *pr, player *p, char *map)
{
    for (int i = pr->size - 1; i >= 0; i--) // iterating on each projectile
    {
        projectile *proj = &pr->data[i];
        Rectangle ProjHitbox = {proj->posX - proj->size / 2, proj->posY - proj->size / 2, proj->size, proj->size};
        // player hits
        checkPlayersGettingHit(proj, ProjHitbox, p);

        // wall hits
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
    if (pr->capacity > pr->size + 8)
    {
        projectile *tmp = (projectile *)realloc(pr->data, sizeof(projectile) * (pr->capacity - 8));
        if (tmp == NULL)
        {
            printf("REALLOC FAILED");
            return;
        }

        pr->data = tmp;
        pr->capacity -= 8;
    }
}

void spawnBullet(player p, projectileList *pr, float custom_orientation, float size, int bounce_left, float bullet_speed)
{
    float aimOrientation = custom_orientation == -1 ? p.aimOrientation : custom_orientation;
    float speedX = bullet_speed * cosf(aimOrientation * 2 * PI / 360);
    float speedY = bullet_speed * sinf(aimOrientation * 2 * PI / 360);
    projectile newProjectile = {p.playerId, p.posX, p.posY, speedX, speedY, p.aimOrientation, size, bounce_left};
    listAdd(pr, newProjectile);
}

void actions(player *players, projectileList *pr)
{
    for (int i = 0; i < 2; i++) // iterating on every player
    {

        player *p = &players[i];
        int single_shot_key;
        if (i == 0)
        {
            single_shot_key = KEY_SPACE;
        }
        else
        { // i == 1
            single_shot_key = KEY_KP_0;
        }
        if (p->shootTimer > 0)
        {
            p->shootTimer -= 1;
            continue;
        }
        else

        {
            if (IsKeyPressed(single_shot_key) && !p->powerUp.machineGun)
            {

                float speed = p->powerUp.sniper ? 0.3 : 0.15;
                spawnBullet(*p, pr, -1.0, 0.5, 1, speed);
                p->shootTimer = 15;
            }
            if (p->powerUp.machineGun)
            {

                float random_orientation = (p->aimOrientation + rand() % 40) - 20;
                spawnBullet(*p, pr, random_orientation, 0.3, 0, 0.15);
                p->shootTimer = 3;
            }
        }
    }
}

void givePowerUp(player *p, powerUp *pu, projectileList *pr)
{
    switch (pu->index)
    {
    case 0: // increase player speed
        p->speed = 0.15;
        p->powerUp.timer = 60 * 5;
        break;
    case 1: // increase bullet speed
        p->powerUp.sniper = true;
        p->powerUp.timer = 60 * 5;
        break;
    case 2: // auto shoot and 'noise' in the shots
        p->powerUp.machineGun = true;
        p->powerUp.timer = 60 * 5;
        break;
    case 3: // summons dvd enemy
        float speed = 0.1;
        float speedX = speed * cosf(p->aimOrientation * 2 * PI / 360);
        float speedY = speed * sinf(p->aimOrientation * 2 * PI / 360);
        projectile dvd = {p->playerId, pu->posX + 1, pu->posY + 1, speedX, speedY, p->aimOrientation, 1.5, 15};
        listAdd(pr, dvd);
        break;
    case 4: // bomb sends bullets
        for (int i = 0; i < 48; i++)
        {
            float size = (float)(rand() % 9) / 10;
            spawnBullet(*p, pr, i * 7.5, size, 0, 0.1);
        }
        break;
    case 5: // life + 1
        printf("life + 1 a ajouter");
        break;
    }
}

void powerUps(powerUp *pu, char *map, player *p, projectileList *pr)
{

    if (pu->timer <= 0)
    { // spawn a power up
        while (!pu->hasSpawnYet)
        {
            int randomX = rand() % W;
            int randomY = rand() % H;
            Rectangle hitbox = {randomX, randomY, 2, 2};

            if (!IsThereAnObstacle(hitbox, 0, false, map))
            {
                int randomIndex = rand() % 6;
                pu->index = randomIndex;
                pu->posX = (float)randomX;
                pu->posY = (float)randomY;
                pu->hasSpawnYet = true;
            }
        }
        // check if a player takes it
        for (int i = 0; i < 2; i++)
        {
            player *pla = &p[i];
            Rectangle hitboxPlayer = {pla->posX - pla->size / 2, pla->posY - pla->size / 2, pla->size, pla->size};
            Rectangle hitboxPowerUp = {pu->posX, pu->posY, 2, 2};
            if (CheckCollisionRecs(hitboxPlayer, hitboxPowerUp))
            { // effects of the power up
                givePowerUp(pla, pu, pr);
                pu->posY = 1000000; // disappears from the map
                pu->timer = 60 * 1;
                pu->hasSpawnYet = false;
            }
        }
    }
    else
    {
        pu->timer -= 1;
    }
    for (int i = 0; i < 2; i++)
    {
        player *pla = &p[i];

        if (pla->powerUp.timer == 0)
        {
            pla->powerUp.machineGun = false;
            pla->powerUp.sniper = false;
            pla->speed = 0.1;
            pla->powerUp.timer = -1;
        }
        if (pla->powerUp.timer > 0)
        {
            pla->powerUp.timer -= 1;
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
    player t;
    powerUpPlayer pupl = {0, 0, 0};
    player p1 = {2, 2, 0.8, 0.1, 10, 0, 0, pupl};
    player p2 = {20, 13, 0.8, 0.1, 10, 1, 0, pupl};
    player p[2] = {p1, p2};
    projectile *ptr = (projectile *)(malloc(sizeof(projectile) * 8)); // initializing projectile list
    projectileList prList = {0, ptr, 8};
    powerUp pu = {120, 0, 100000, false, 0}; // the object, spawns out of the map

    float dUX = GetScreenWidth() / (float)W;
    float dUY = GetScreenHeight() / (float)H;
    float dU = dUX < dUY ? dUX : dUY;

    const char *nom = "./YskTANKycklaba/src/Game/map.txt";
    char *txt = LoadFileText(nom);

    // gives a random seed to make random random or random isn't random
    srand(time(NULL));

    const char *caca = GetGamepadName(0);
    printf("Detected gamepad name: %s\n", GetGamepadName(0));
    if (IsGamepadAvailable(0))
    {
        printf("Gamepad connected\n");
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        //---------------- DESSINER ICI POUR CHAQUE FRAME
        float dUX = GetScreenWidth() / (float)W;
        float dUY = GetScreenHeight() / (float)H;
        float dU = dUX < dUY ? dUX : dUY; // dynamic resolution rendering
        mainLoop(p, &prList, &pu, dU, txt);
        //----------------
        EndDrawing();
    }
    free(txt);
    CloseWindow();
    return 0;
}
