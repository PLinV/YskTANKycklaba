#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// pr lancer : gcc YskTANKycklaba/src/Game/*.c -lraylib -lGL -lm -lpthread -ldl
// -lrt -lX11 -I./raylib/src -L./raylib/src && ./a.out
#include "Game.h"

void mainLoop(player *p, projectileList *pr, float dU, char *map, int width,
              int height, Texture2D bg) {
  // get inputs and move player (in tiles unit)
  movePlayers(p, map, width, height);
  actions(p, pr);
  // render everything (scale unit)
  drawEveryTile(dU, map, width, height, bg);
  drawPlayers(dU, p);
  drawProjectiles(dU, pr);
}

int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical,
                      char *map, int width, int height) {

  // checks if hitbox collides in a direction (either vertical or horizontal)
  if (checkVertical) {
    bool res = false;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        if (map[j * width + i] ==
            1) { // 1 means obstacle. 2 and 3 are spawns (walkable)
          Rectangle newPlayerPos = {player.x + velocity, player.y, player.width,
                                    player.height};
          Rectangle obstacle = {i, j, 1, 1};
          if (CheckCollisionRecs(newPlayerPos, obstacle)) {
            res = true;
            break;
          }
        }
      }
      if (res)
        break;
    }

    return res;
  } else {
    bool res = false;
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        if (map[j * width + i] == 1) { // 1 means obstacle
          Rectangle newPlayerPos = {player.x, player.y + velocity, player.width,
                                    player.height};
          Rectangle obstacle = {i, j, 1, 1};
          if (CheckCollisionRecs(newPlayerPos, obstacle)) {
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

void movePlayers(player *players, char *map, int width, int height) {
  for (int i = 0; i < 2; i++) {
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

    if (fabs(x_velocity) > 0 && fabs(y_velocity) > 0) {
      // normalize velocity, si vous comprennez pas cherchez sur internet c
      // normal
      x_velocity /= sqrt(2);
      y_velocity /= sqrt(2);
    }

    // update player position
    Rectangle PlayerHitbox = {p->posX - p->size / 2, p->posY - p->size / 2,
                              p->size, p->size};
    // DrawRectangle(PlayerHitbox.x * 5, PlayerHitbox.y * 5, PlayerHitbox.width
    // * 5, PlayerHitbox.height * 5, RED);

    // checker si collision, si oui
    if (!IsThereAnObstacle(PlayerHitbox, x_velocity, true, map, width,
                           height)) {
      p->posX += x_velocity;
    }
    if (!IsThereAnObstacle(PlayerHitbox, y_velocity, false, map, width,
                           height)) {
      p->posY += y_velocity;
    }
  }
}

void listAdd(projectileList *pr, projectile p) {
  if (pr->capacity <= pr->size) {
    pr->data = (projectile *)realloc(pr->data,
                                     sizeof(projectile) * (pr->capacity + 8));
    pr->capacity += 8;
  }
  pr->data[pr->size] = p;
  pr->size += 1;
  // printf("taille : %d, capa : %d\n", pr->size, pr->capacity);
  // for (int i = 0; i < pr->size; i++)
  // {
  //     projectile c = pr->data[i];
  //     printf("x: %f, y:%f, 1:%f, 2:%f\n", c.posX, c.posY, c.speedX,
  //     c.speedY);
  // }
}
void listRem() {}

void actions(player *players, projectileList *pr) {
  for (int i = 0; i < 2; i++) // iterating on every player
  {
    player p = players[i];
    switch (i) {
    case 0:
      if (IsKeyPressed(KEY_SPACE)) {
        projectile newProjectile = {i, p.posX, p.posY, 3, 4, p.aimOrientation};
        listAdd(pr, newProjectile);
      }
      break;
    case 1:
      if (IsKeyPressed(KEY_KP_0)) {
        projectile newProjectile = {i, p.posX, p.posY, 3, 4, p.aimOrientation};
        listAdd(pr, newProjectile);
      }
      break;
    }
  }
}

void initGame() {
  char *title = "YskTANKycklaba";

  InitWindow(1000, 640, title);
  int b = GetMonitorWidth(0);
  int a = GetMonitorHeight(0);

  // ToggleBorderlessWindowed();
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  // SetWindowMinSize(1000, 700);
}

void RunGame(char *mapPath) {
  SetWindowTitle("YskTANKycklaba - Game");

  // Default map if none provided
  if (mapPath == NULL || strlen(mapPath) == 0) {
    mapPath = "src/Graphisme/Maps/default.txt";
  }

  // 1. Load Map Data
  char *text = LoadFileText(mapPath);
  if (!text) {
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
  if (args < 2) {
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
  while (token != NULL && index < width * height) {
    int val = atoi(token);
    cleanMap[index] = val;

    // Collect Spawns
    if (val == 2) { // Spawn P1
      spawnsP1[countP1++] =
          (Vector2){(float)(index % width), (float)(index / width)};
    } else if (val == 3) { // Spawn P2
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

  Texture2D bgTexture = LoadTexture(imagePath);
  if (bgTexture.id == 0) {
    TraceLog(LOG_WARNING, "Failed to load map image: %s. Using fallback.",
             imagePath);
  }

  // 3. Init Players & Projectiles
  // Randomize Spawns if available
  float p1x = 1.5f, p1y = 1.5f;                  // Default
  float p2x = width - 2.5f, p2y = height - 2.5f; // Default

  if (countP1 > 0) {
    int r = GetRandomValue(0, countP1 - 1);
    p1x = spawnsP1[r].x + 0.5f; // Center in tile
    p1y = spawnsP1[r].y + 0.5f;
  }
  if (countP2 > 0) {
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

  projectile *ptr = (projectile *)(malloc(sizeof(projectile) * 8));
  projectileList prList = {0, ptr, 8};

  // 4. Game Loop
  bool exitGame = false;
  while (!WindowShouldClose() && !exitGame) {
    if (IsKeyPressed(KEY_BACKSPACE))
      exitGame = true;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    float dUX = GetScreenWidth() / (float)width;
    float dUY = GetScreenHeight() / (float)height;
    float dU = dUX < dUY ? dUX : dUY;

    mainLoop(p, &prList, dU, cleanMap, width, height, bgTexture);

    DrawText("Press BACKSPACE to return to Menu", 10, 10, 20, DARKGRAY);
    EndDrawing();
  }

  // Cleanup
  free(ptr);
  free(cleanMap);
  if (bgTexture.id != 0)
    UnloadTexture(bgTexture);
}
