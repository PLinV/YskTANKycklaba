#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <raylib.h>
#include "Game.h"

void mainLoop(player *p, int playerCount, projectileList *pr, powerUp *pu, float dU, char *map)
{
    // input and logic
    movePlayers(p, playerCount, map);
    actions(p, playerCount, pr);
    updateProjectiles(pr, p, playerCount, map);
    powerUps(pu, map, p, playerCount, pr);

    // render
    drawEveryTile(dU, map);
    drawPlayers(dU, p, playerCount);
    drawPowerUp(dU, *pu);
    drawProjectiles(dU, pr);
}

int IsThereAnObstacle(Rectangle player, float velocity, bool checkHorizontal, char *map, bool isProjectile)
{
    // check collisions in horizontal or vertical
    // player x/y in tile unit
    Rectangle newPos;
    if (checkHorizontal)
        newPos = (Rectangle){player.x + velocity, player.y, player.width, player.height};
    else
        newPos = (Rectangle){player.x, player.y + velocity, player.width, player.height};

    // check all the tiles and see if there is a collision
    for (int j = 0; j < H; j++)
    {
        for (int i = 0; i < W; i++)
        {
            char c = map[(j * W + i) * 2];
            if (c == '1' || c == '2')
            {
                Rectangle tile = {(float)i, (float)j, 1.0f, 1.0f};
                if (CheckCollisionRecs(newPos, tile))
                {
                    if (c == '2' && isProjectile)
                    {
                        map[(j * W + i) * 2] = '0'; // projectiles destroy '2' tiles
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

void movePlayers(player *players, int pC, char *map)
{
    const float DEADZONE = 0.0f;

    for (int i = 0; i < pC; i++)
    {
        player *p = &players[i];
        float x_velocity = 0.0f;
        float y_velocity = 0.0f;

        if (p->move.usesController && p->move.controllerIndex >= 0)
        {
            int g = p->move.controllerIndex;
            float leftStickX = GetGamepadAxisMovement(g, GAMEPAD_AXIS_LEFT_X);
            float leftStickY = GetGamepadAxisMovement(g, GAMEPAD_AXIS_LEFT_Y);
            float rightStickX = GetGamepadAxisMovement(g, GAMEPAD_AXIS_RIGHT_X);
            float rightStickY = GetGamepadAxisMovement(g, GAMEPAD_AXIS_RIGHT_Y);

            // deadzone
            if (fabsf(leftStickX) < DEADZONE)
                leftStickX = 0.0f;
            if (fabsf(leftStickY) < DEADZONE)
                leftStickY = 0.0f;
            if (fabsf(rightStickX) < DEADZONE)
                rightStickX = 0.0f;
            if (fabsf(rightStickY) < DEADZONE)
                rightStickY = 0.0f;

            if (rightStickX != 0.0f || rightStickY != 0.0f)
            {
                float angle = atan2f(-rightStickX, rightStickY) * (180.0f / PI);
                angle += 90.0f;
                if (angle < 0.0f)
                    angle += 360.0f;
                else if (angle >= 360.0f)
                    angle -= 360.0f;
                p->aimOrientation = angle;
            }

            // movement controller
            x_velocity += p->speed * leftStickX;
            y_velocity += p->speed * leftStickY;
        }
        else
        {
            // keyboard movement
            if (IsKeyDown(p->move.keyUp))
                y_velocity -= p->speed;
            if (IsKeyDown(p->move.keyDown))
                y_velocity += p->speed;
            if (IsKeyDown(p->move.keyLeft))
                x_velocity -= p->speed;
            if (IsKeyDown(p->move.keyRight))
                x_velocity += p->speed;

            // keyboard aim
            if (IsKeyDown(p->move.keyTurnRight))
                p->aimOrientation += 3.0f;
            if (IsKeyDown(p->move.keyTurnLeft))
                p->aimOrientation -= 3.0f;
        }

        // aimOrientation
        while (p->aimOrientation >= 360.0f)
            p->aimOrientation -= 360.0f;
        while (p->aimOrientation < 0.0f)
            p->aimOrientation += 360.0f;

        // normalize diagonal speed, check on internet if you don't understand its normal
        if (fabsf(x_velocity) > 0.0f && fabsf(y_velocity) > 0.0f)
        {
            x_velocity /= sqrtf(2.0f);
            y_velocity /= sqrtf(2.0f);
        }

        Rectangle PlayerHitbox = {p->posX - p->size / 2.0f, p->posY - p->size / 2.0f, p->size, p->size};

        if (!IsThereAnObstacle(PlayerHitbox, x_velocity, true, map, false))
            p->posX += x_velocity;
        if (!IsThereAnObstacle(PlayerHitbox, y_velocity, false, map, false))
            p->posY += y_velocity;
    }
}

void checkPlayersGettingHit(projectile *proj, Rectangle ProjHitbox, player *p, int pC)
{
    for (int j = 0; j < pC; j++)
    {
        if (p[j].playerId == proj->playerId)
            continue; // skip its own player

        player *pla = &p[j];
        Rectangle PlayerHitbox = {pla->posX - pla->size / 2.0f, pla->posY - pla->size / 2.0f, pla->size, pla->size};
        if (CheckCollisionRecs(ProjHitbox, PlayerHitbox))
        {
            // basically kills the player
            pla->posX = 1000000.0f;
        }
    }
}

void updateProjectiles(projectileList *pr, player *p, int pC, char *map)
{
    for (int i = pr->size - 1; i >= 0; i--)
    {
        projectile *proj = &pr->data[i];
        Rectangle ProjHitbox = {proj->posX - proj->size / 2.0f, proj->posY - proj->size / 2.0f, proj->size, proj->size};

        // check player collisions
        checkPlayersGettingHit(proj, ProjHitbox, p, pC);

        // horizontal movement and collisions
        if (!IsThereAnObstacle(ProjHitbox, proj->speedX, true, map, true))
        {
            proj->posX += proj->speedX;
        }
        else
        {
            if (proj->bounceLeft > 0)
            {
                proj->speedX = -proj->speedX;
                proj->bounceLeft -= 1;
            }
            else
            {
                listRem(i, pr);
                continue;
            }
        }

        // vertical movement and collisions
        if (!IsThereAnObstacle(ProjHitbox, proj->speedY, false, map, true))
        {
            proj->posY += proj->speedY;
        }
        else
        {
            if (proj->bounceLeft > 0)
            {
                proj->speedY = -proj->speedY;
                proj->bounceLeft -= 1;
            }
            else
            {
                listRem(i, pr);
                continue;
            }
        }
    }
}

void listAdd(projectileList *pr, projectile p)
{
    if (pr->capacity <= pr->size)
    {
        int newCap = pr->capacity + 8;
        projectile *tmp = (projectile *)realloc(pr->data, sizeof(projectile) * newCap);
        if (!tmp)
        {
            fprintf(stderr, "REALLOC FAILED (add)\n");
            return;
        }
        pr->data = tmp;
        pr->capacity = newCap;
    }
    pr->data[pr->size++] = p;
}

void listRem(int projectileIndex, projectileList *pr)
{
    if (projectileIndex < 0 || projectileIndex >= pr->size)
        return;
    if (projectileIndex < pr->size - 1)
    {
        for (int i = projectileIndex; i < pr->size - 1; i++)
            pr->data[i] = pr->data[i + 1];
    }
    pr->size -= 1;

    if (pr->capacity > pr->size + 8)
    {
        int newCap = pr->capacity - 8;
        projectile *tmp = (projectile *)realloc(pr->data, sizeof(projectile) * newCap);
        if (!tmp)
        {
            fprintf(stderr, "REALLOC FAILED (shrink)\n");
            return;
        }
        pr->data = tmp;
        pr->capacity = newCap;
    }
}

void spawnBullet(player p, projectileList *pr, float custom_orientation, float size, int bounce_left, float bullet_speed)
{
    float aimOrientation = (custom_orientation == -1.0f) ? p.aimOrientation : custom_orientation;
    float rad = aimOrientation * PI / 180.0f;
    float speedX = bullet_speed * cosf(rad);
    float speedY = bullet_speed * sinf(rad);

    projectile newProjectile = {
        .playerId = p.playerId,
        .posX = p.posX,
        .posY = p.posY,
        .speedX = speedX,
        .speedY = speedY,
        .Orientation = aimOrientation,
        .size = size,
        .bounceLeft = bounce_left};
    listAdd(pr, newProjectile);
}

void actions(player *players, int pC, projectileList *pr)
{
    for (int i = 0; i < pC; i++)
    {
        player *p = &players[i];
        if (p->shootTimer > 0)
        {
            p->shootTimer -= 1;
            continue;
        }

        bool didShoot = false;
        // controller shoot
        if (p->move.usesController && p->move.controllerIndex >= 0)
        {
            int g = p->move.controllerIndex;
            if (IsGamepadButtonPressed(g, 12))
            {
                float speed = p->powerUp.sniper ? 0.3f : 0.15f;
                spawnBullet(*p, pr, -1.0f, 0.5f, 2, speed);
                p->shootTimer = 15;
                didShoot = true;
            }
        }

        // keyboard shoot
        if (!didShoot && IsKeyPressed(p->move.keyShoot) && !p->powerUp.machineGun)
        {
            float speed = p->powerUp.sniper ? 0.3f : 0.15f;
            spawnBullet(*p, pr, -1.0f, 0.5f, 2, speed);
            p->shootTimer = 15;
            didShoot = true;
        }

        // machine gun powerup (has auto shoot)
        if (p->powerUp.machineGun)
        {
            // random spread
            float random_orientation = p->aimOrientation + (float)(rand() % 40) - 20.0f;
            spawnBullet(*p, pr, random_orientation, 0.3f, 0, 0.15f);
            p->shootTimer = 3;
        }
    }
}

void givePowerUp(player *p, powerUp *pu, projectileList *pr)
{
    switch (pu->index)
    {
    case 0: // increase player speed
        p->speed = 0.15f;
        p->powerUp.timer = 60 * 10;
        break;
    case 1: // increase bullet speed
        p->powerUp.sniper = true;
        p->powerUp.timer = 60 * 10;
        break;
    case 2: // machine gun
        p->powerUp.machineGun = true;
        p->powerUp.timer = 60 * 10;
        break;
    case 3: // spawn dvd enemy (projectile)
    {
        float speed = 0.2f;
        float rad = p->aimOrientation * PI / 180.0f;
        float sx = speed * cosf(rad);
        float sy = speed * sinf(rad);
        projectile dvd = {p->playerId, pu->posX + 1.f, pu->posY + 1.f, sx, sy, p->aimOrientation, 1.5f, 35};
        listAdd(pr, dvd);
        break;
    }
    case 4: // bomb
        for (int i = 0; i < 48; i++)
        {
            float size = (float)(rand() % 9) / 10.0f;
            spawnBullet(*p, pr, i * 7.5f, size, 1, 0.1f);
        }
        break;
    case 5: // life (TODO)
        // implement life logic if you have a life field
        break;
    }
}

void randomCoordsWithDistance(int *XY, player *p, char *map, int pC, int distanceMinToPlayer)
{
    int tries = 0;
    int randomX;
    int randomY;
    bool hasFinished = false;
    while (!hasFinished && tries < 1000)
    {
        randomX = rand() % W;
        randomY = rand() % H;
        Rectangle hitbox = {(float)randomX, (float)randomY, 2.0f, 2.0f};

        if (!IsThereAnObstacle(hitbox, 0.0f, false, map, false))
        {
            bool farEnough = false;
            for (int i = 0; i < pC; i++)
            {
                player pl = p[i];
                float x = randomX - pl.posX;
                float y = randomY - pl.posY;
                farEnough = true;
                float dist = x * x + y * y;
                if (dist < distanceMinToPlayer * distanceMinToPlayer)
                {
                    // if too close
                    farEnough = false;
                    // printf("not far enough : %f, %f, %d\n", sqrt(dist), dist, i);
                    break;
                }
            }
            if (farEnough)
            {
                hasFinished = true;
                break;
            }
        }
        tries++;
    }
    XY[0] = randomX;
    XY[1] = randomY;
    // printf("spawns : %d %d , tries:%d\n", randomX, randomY, tries);
}

void powerUps(powerUp *pu, char *map, player *p, int pC, projectileList *pr)
{
    if (pu->timer <= 0)
    { // spawns power up
        if (!pu->hasSpawnYet)
        {
            int XY[2] = {-1, 0};
            randomCoordsWithDistance(&XY, p, map, pC, 10);
            pu->index = rand() % 6;
            pu->posX = (float)XY[0];
            pu->posY = (float)XY[1];
            pu->hasSpawnYet = true;
            // finds random free spot
        }

        // affecting players
        for (int i = 0; i < pC; i++)
        {
            player *pla = &p[i];
            Rectangle hitboxPlayer = {pla->posX - pla->size / 2.0f, pla->posY - pla->size / 2.0f, pla->size, pla->size};
            Rectangle hitboxPowerUp = {pu->posX, pu->posY, 2.0f, 2.0f};
            if (CheckCollisionRecs(hitboxPlayer, hitboxPowerUp))
            {
                givePowerUp(pla, pu, pr);
                pu->posY = 1000000.0f;
                pu->timer = 60 * 1;
                pu->hasSpawnYet = false;
            }
        }
    }
    else
    {
        pu->timer -= 1;
    }

    // powerup timer to decrease when a player has a powerup
    for (int i = 0; i < pC; i++)
    {
        player *pla = &p[i];
        if (pla->powerUp.timer == 0)
        {
            pla->powerUp.machineGun = false;
            pla->powerUp.sniper = false;
            pla->speed = 0.1f;
            pla->powerUp.timer = -1;
        }
        if (pla->powerUp.timer > 0)
            pla->powerUp.timer -= 1;
    }
}



int StartGame()
{
    const int playersCount = 4;


    powerUpPlayer pupl = {0, false, false};

    // allocate players
    player *p = (player *)malloc(sizeof(player) * playersCount);
    if (!p)
    {
        printf("malloc failed\n");
        return 1;
    }

    // projectile list
    projectile *ptr = (projectile *)malloc(sizeof(projectile) * 8);
    if (!ptr)
    {
        printf("malloc failed\n");
        free(p);
        return 1;
    }
    projectileList prList = {0, ptr, 8};

    powerUp pu = {120, 0.0f, 100000.0f, false, 0};

    // load map
    const char *nom = "./YskTANKycklaba/src/Graphisme/Maps/default.txt";
    char *txt = LoadFileText(nom);
    if (!txt)
    {
        printf("Failed to load map file '%s'\n", nom);
        free(p);
        free(ptr);
        return 1;
    }

    srand(time(NULL)); // makes random random either way random isn't random which is a problem when you want randomness

    // detecting gamepads
    int detectedPadIndices[4];
    int padCount = 0;
    for (int i = 0; i < 4; i++)
    {
        if (IsGamepadAvailable(i))
        {
            detectedPadIndices[padCount++] = i;
            printf("Gamepad %d: %s\n", i, GetGamepadName(i));
        }
    }
    printf("%d gamepads available\n", padCount);

    // keyboard keys for up to 4 players, if gamepads available, then the gamepads 'overwrite' the keyboard inputs
    movement move1 = {KEY_W, KEY_S, KEY_A, KEY_D, KEY_E, KEY_Q, KEY_SPACE, false, -1};
    movement move2 = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_KP_2, KEY_KP_1, KEY_KP_0, false, -1};
    movement move3 = {KEY_I, KEY_K, KEY_J, KEY_L, KEY_O, KEY_U, KEY_P, false, -1};
    movement move4 = {KEY_T, KEY_G, KEY_F, KEY_H, KEY_Y, KEY_R, KEY_V, false, -1};
    movement moves[4] = {move1, move2, move3, move4};

    // initialize players and give the detected gamepads
    int padAssignCursor = 0;
    for (int i = 0; i < playersCount; i++)
    {
        p[i] = (player){
            .posX = (float)((i + 1) * 2),
            .posY = 2.0f,
            .size = 0.8f,
            .speed = 0.1f,
            .aimOrientation = 0.0f,
            .playerId = i,
            .shootTimer = 0,
            .powerUp = pupl,
            .move = moves[i]};

        if (padAssignCursor < padCount)
        {
            p[i].move.usesController = true;
            p[i].move.controllerIndex = detectedPadIndices[padAssignCursor++];
        }
        else
        {
            p[i].move.usesController = false;
            p[i].move.controllerIndex = -1;
        }
    }

    // main loop
    while (1)
    {
        BeginDrawing();
        //---------------recompute dynamic dU when window resizes
        float dUX = GetScreenWidth() / (float)W;
        float dUY = GetScreenHeight() / (float)H;
        float dU = dUX < dUY ? dUX : dUY;
        //---------------
        mainLoop(p, playersCount, &prList, &pu, dU, txt);
        EndDrawing();
    }

    // free memory and no segfaults
    free(txt);
    free(p);
    free(ptr);
    CloseWindow();
    return 0;
}



void RunGame(char *mapPath)
{
    // SetWindowTitle("YskTANKycklaba - Game");

    // Default map if none provided
    if (mapPath == NULL || strlen(mapPath) == 0)
    {
        mapPath = "./YskTANKycklaba/src/Graphisme/Maps/default.txt";
    }
    const char *nom = "./YskTANKycklaba/src/Graphisme/Maps/default.txt";
    char *text = LoadFileText(nom);
    // 1. Load Map Data
    // char *text = LoadFileText(mapPath);
    if (!text)
    {
        TraceLog(LOG_ERROR, "Failed to load map: %s", mapPath);
        return;
    }
    int width = 0, height = 0;
    int nbSpawnP1 = 0, nbSpawnP2 = 0;

    // Read dimensions and spawn counts from first line
    // Format: Width,Height,NbSpawnP1,NbSpawnP2
    char *line = strtok(text, "\n");
    int args =
        sscanf(line, "%d,%d,%d,%d", &width, &height, &nbSpawnP1, &nbSpawnP2);
    if (args < 2)
    {
        TraceLog(LOG_ERROR, "Invalid map format: %s", mapPath);
        UnloadFileText(text);
        return;
    }
    // Parse collision map and collect spawns
    char *cleanMap = (char *)calloc(width * height, sizeof(char));

    // Temporary storage for spawn points
    Vector2 *spawnsP1 = (Vector2 *)malloc(sizeof(Vector2) * (width * height));
    Vector2 *spawnsP2 = (Vector2 *)malloc(sizeof(Vector2) * (width * height));
    int countP1 = 0;
    int countP2 = 0;

    int index = 0;
    char *token = strtok(NULL, ",\n\r");
    while (token != NULL && index < width * height)
    {
        int val = atoi(token);
        cleanMap[index] = val;

        // Collect Spawns
        if (val == 2)
        { // Spawn P1
            spawnsP1[countP1++] =
                (Vector2){(float)(index % width), (float)(index / width)};
        }
        else if (val == 3)
        { // Spawn P2
            spawnsP2[countP2++] =
                (Vector2){(float)(index % width), (float)(index / width)};
        }

        index++;
        token = strtok(NULL, ",\n\r");
    }
    UnloadFileText(text);

    // 2. Load Map Image
    char imagePath[256];
    strncpy(imagePath, mapPath, 255);
    char *dot = strrchr(imagePath, '.');
    if (dot)
        strcpy(dot, ".png");
    else
        strcat(imagePath, ".png");

    // Texture2D bgTexture = LoadTexture(imagePath);

    // if (bgTexture.id == 0)
    // {
    //     TraceLog(LOG_WARNING, "Failed to load map image: %s. Using fallback.",
    //              imagePath);
    // }

    // 3. Init Players & Projectiles
    // Randomize Spawns if available
    float p1x = 1.5f, p1y = 1.5f;                  // Default
    float p2x = width - 2.5f, p2y = height - 2.5f; // Default

    if (countP1 > 0)
    {
        int r = GetRandomValue(0, countP1 - 1);
        p1x = spawnsP1[r].x + 0.5f; // Center in tile
        p1y = spawnsP1[r].y + 0.5f;
    }
    if (countP2 > 0)
    {
        int r = GetRandomValue(0, countP2 - 1);
        p2x = spawnsP2[r].x + 0.5f;
        p2y = spawnsP2[r].y + 0.5f;
    }

    // Free spawn lists
    free(spawnsP1);
    free(spawnsP2);

    // Player size set to 0.7 (70% of tile)
    player p1 = {p1x, p1y, 0.7f, 0.1f, 10};
    player p2 = {p2x, p2y, 0.7f, 0.3f, 10};
    player p[2] = {p1, p2};

    powerUp pu = {120, 0.0f, 100000.0f, false, 0};

    projectile *ptr = (projectile *)(malloc(sizeof(projectile) * 8));
    projectileList prList = {0, ptr, 8};

    // 4. Game Loop
    bool exitGame = false;
    while (!WindowShouldClose() && !exitGame)
    {

        if (IsKeyPressed(KEY_BACKSPACE))
            exitGame = true;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        float dUX = GetScreenWidth() / (float)width;
        float dUY = GetScreenHeight() / (float)height;
        float dU = dUX < dUY ? dUX : dUY;

        mainLoop(p, 4, &prList, &pu, dU, cleanMap);

        DrawText("Press BACKSPACE to return to Menu", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    // Cleanup
    free(ptr);
    free(cleanMap);
    // if (bgTexture.id != 0)
    //     UnloadTexture(bgTexture);
}