#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <time.h>
// pr lancer : gcc YskTANKycklaba/src/Game/*.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -I./raylib/src -L./raylib/src && ./a.out
#include "Game.h"

void mainLoop(player *p, int playerCount, projectileList *pr, powerUp *pu, float dU, char *map)
{
    // get inputs and move player (in tiles unit)
    movePlayers(p, playerCount, map);
    actions(p, playerCount, pr);
    updateProjectiles(pr, p, map, playerCount);
    powerUps(pu, map, p, playerCount, pr);
    // render everything (scale unit)
    drawEveryTile(dU, map);
    drawPlayers(dU, p, playerCount);
    drawPowerUp(dU, *pu);
    drawProjectiles(dU, pr);
}

int IsThereAnObstacle(Rectangle player, float velocity, bool checkHorizontal, char *map, bool isPlayer)
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
                        if (map[(j * W + i) * 2] == '2' && !isPlayer)
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
                if (map[(j * W + i) * 2] == '1' || map[(j * W + i) * 2] == '2')
                {
                    Rectangle newPlayerPos = {player.x, player.y + velocity, player.width, player.height};
                    Rectangle obstacle = {i, j, 1, 1};
                    if (CheckCollisionRecs(newPlayerPos, obstacle))
                    {
                        if (map[(j * W + i) * 2] == '2' && !isPlayer)
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
}
void movePlayers(player *players, int pC, char *map)
{
    const float DEADZONE = 0.2f;

    for (int i = 0; i < pC; i++)
    {
        float x_velocity = 0.0f;
        float y_velocity = 0.0f;
        player *p = &players[i];

        if (p->move.usesController && p->move.controllerIndex >= 0)
        {
            int g = p->move.controllerIndex;

            float leftStickX = GetGamepadAxisMovement(g, GAMEPAD_AXIS_LEFT_X);
            float leftStickY = GetGamepadAxisMovement(g, GAMEPAD_AXIS_LEFT_Y);
            float rightStickX = GetGamepadAxisMovement(g, GAMEPAD_AXIS_RIGHT_X);
            float rightStickY = GetGamepadAxisMovement(g, GAMEPAD_AXIS_RIGHT_Y);

            // apply deadzone
            if (fabs(leftStickX) < DEADZONE)
                leftStickX = 0.0f;
            if (fabs(leftStickY) < DEADZONE)
                leftStickY = 0.0f;
            if (fabs(rightStickX) < DEADZONE)
                rightStickX = 0.0f;
            if (fabs(rightStickY) < DEADZONE)
                rightStickY = 0.0f;

            // aim: your custom system where 0=down, 90=left, 180=up, 270=right, then +90° fix
            if (rightStickX != 0.0f || rightStickY != 0.0f)
            {
                float angle = atan2f(-rightStickX, rightStickY) * (180.0f / PI);
                angle += 90.0f;
                if (angle < 0)
                    angle += 360.0f;
                else if (angle >= 360.0f)
                    angle -= 360.0f;
                p->aimOrientation = angle;
            }

            x_velocity += p->speed * leftStickX;
            y_velocity += p->speed * leftStickY;
        }
        else
        {
            if (IsKeyDown(p->move.keyUp))
                y_velocity -= p->speed;
            if (IsKeyDown(p->move.keyDown))
                y_velocity += p->speed;
            if (IsKeyDown(p->move.keyLeft))
                x_velocity -= p->speed;
            if (IsKeyDown(p->move.keyRight))
                x_velocity += p->speed;
            if (IsKeyDown(p->move.keyTurnRight))
                p->aimOrientation += 3.0f;
            if (IsKeyDown(p->move.keyTurnLeft))
                p->aimOrientation -= 3.0f;
        }

        // wrap aimOrientation properly
        while (p->aimOrientation >= 360.0f)
            p->aimOrientation -= 360.0f;
        while (p->aimOrientation < 0.0f)
            p->aimOrientation += 360.0f;

        if (fabs(x_velocity) > 0.0f && fabs(y_velocity) > 0.0f)
        {
            x_velocity /= sqrtf(2.0f);
            y_velocity /= sqrtf(2.0f);
        }

        Rectangle PlayerHitbox = {p->posX - p->size / 2.0f, p->posY - p->size / 2.0f, p->size, p->size};
        if (!IsThereAnObstacle(PlayerHitbox, x_velocity, true, map, true))
            p->posX += x_velocity;
        if (!IsThereAnObstacle(PlayerHitbox, y_velocity, false, map, true))
            p->posY += y_velocity;
    }
}
void checkPlayersGettingHit(projectile *proj, Rectangle ProjHitbox, player *p, int pC)
{
    for (int j = 0; j < pC; j++)
    {
        if (p[j].playerId == proj->playerId)
            continue; // skip owner by id

        player *pla = &p[j];
        Rectangle PlayerHitbox = {pla->posX - pla->size / 2, pla->posY - pla->size / 2, pla->size, pla->size};
        if (CheckCollisionRecs(ProjHitbox, PlayerHitbox))
        {
            pla->posX = 1000000; // kill
        }
    }
}

void updateProjectiles(projectileList *pr, player *p, char *map, int pC)
{
    for (int i = pr->size - 1; i >= 0; i--) // iterating on each projectile
    {
        projectile *proj = &pr->data[i];
        Rectangle ProjHitbox = {proj->posX - proj->size / 2, proj->posY - proj->size / 2, proj->size, proj->size};
        // player hits
        checkPlayersGettingHit(proj, ProjHitbox, p, pC);

        // wall hits
        if (!IsThereAnObstacle(ProjHitbox, proj->speedX, true, map, false))
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
        if (!IsThereAnObstacle(ProjHitbox, proj->speedY, false, map, false))
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

void actions(player *players, int pC, projectileList *pr)
{
    for (int i = 0; i < pC; i++) // iterating on every player
    {

        player *p = &players[i];
        if (p->shootTimer > 0)
        {
            p->shootTimer -= 1;
            continue;
        }
        else

        {
            if (IsKeyPressed(p->move.keyShoot) && !p->powerUp.machineGun)
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

void powerUps(powerUp *pu, char *map, player *p, int pC, projectileList *pr)
{

    if (pu->timer <= 0)
    { // spawn a power up
        while (!pu->hasSpawnYet)
        {
            int randomX = rand() % W;
            int randomY = rand() % H;
            Rectangle hitbox = {randomX, randomY, 2, 2};

            if (!IsThereAnObstacle(hitbox, 0, false, map, false))
            {
                int randomIndex = rand() % 6;
                pu->index = randomIndex;
                pu->posX = (float)randomX;
                pu->posY = (float)randomY;
                pu->hasSpawnYet = true;
            }
        }
        // check if a player takes it
        for (int i = 0; i < pC; i++)
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
    for (int i = 0; i < pC; i++)
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
    int playersCount = 4;

    initGame();
    player t;
    powerUpPlayer pupl = {0, 0, 0};

    player *p = (player *)malloc(sizeof(player) * playersCount);

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

    int gamepadsCount = 0;
    for (int i = 0; i < 4; i++) // GLFW supports up to 4 gamepads reliably
    {
        if (IsGamepadAvailable(i))
        {

            printf("Gamepad %d: %s\n", i, GetGamepadName(i));
            gamepadsCount++;
        }
    }
    printf("%d gamepads available\n", gamepadsCount);

    int keyboardCount = playersCount - gamepadsCount;
    keyboardCount = (keyboardCount > 2) ? 2 : keyboardCount;
    printf("%d\n", keyboardCount);

    movement move1 = {KEY_W, KEY_S, KEY_A, KEY_D, KEY_E, KEY_Q, KEY_SPACE, false, -1};
    movement move2 = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_KP_2, KEY_KP_1, KEY_KP_0, false, -1};
    movement move3 = {0, 0, 0, 0, 0, 0, 0, false, -1};
    movement move4 = {0, 0, 0, 0, 0, 0, 0, false, -1};

    movement moves[4] = {move1, move2, move3, move4};
    for (int i = 0; i < playersCount; i++)
    {
        p[i] = (player){
            .posX = (float)((i + 1) * 2),
            .posY = 2.0f,
            .size = 0.8f,
            .speed = 0.1f,
            .aimOrientation = 0.0f,
            .playerId = i, // assign unique player ids
            .shootTimer = 0,
            .powerUp = pupl,
            .move = moves[i] // safe, stays in-bounds now
        };

        // controller assignment: only mark usesController for available gamepads
        if (i < gamepadsCount)
        {
            p[i].move.usesController = true;
            p[i].move.controllerIndex = i; // assign the actual gamepad index
        }
        else
        {
            p[i].move.usesController = false;
            p[i].move.controllerIndex = -1;
        }
    }

    while (!WindowShouldClose())
    {
        BeginDrawing();
        //---------------- DESSINER ICI POUR CHAQUE FRAME
        float dUX = GetScreenWidth() / (float)W;
        float dUY = GetScreenHeight() / (float)H;
        float dU = dUX < dUY ? dUX : dUY; // dynamic resolution rendering
        mainLoop(p, playersCount, &prList, &pu, dU, txt);
        //----------------

        EndDrawing();
    }
    free(txt);
    free(p);
    free(ptr);
    CloseWindow();
    return 0;
}