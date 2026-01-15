#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "render.h"

Texture2D *Loadtextures()
{
  // Max 12 blocks (0-11), then entities (12-25)
  Texture2D *textureArray = (Texture2D *)calloc(30, sizeof(Texture2D));
  if (!textureArray)
  {
    printf("ERROR ALLOCATING TEXTURES");
    return NULL;
  }

  // 1. Load Dynamic Blocks from texture_bloc
  FilePathList files = LoadDirectoryFiles("src/Graphisme/assets/texture_bloc");
  for (int i = 0; i < (int)files.count; i++)
  {
    const char *fname = GetFileName(files.paths[i]);
    int id = -1;
    const char *underscore = strchr(fname, '_');
    if (underscore)
    {
      char idStr[16] = {0};
      strncpy(idStr, fname, underscore - fname);
      id = atoi(idStr);
    }

    if (id >= 0 && id < 12)
    {
      textureArray[id] = LoadTexture(files.paths[i]);
    }
  }
  UnloadDirectoryFiles(files);

  // Default if missing (Index 0)
  if (textureArray[0].id == 0)
  {
    Image img = GenImageColor(32, 32, GREEN);
    textureArray[0] = LoadTextureFromImage(img);
    UnloadImage(img);
  }

  // 2. Load Entities (Tank/Bullet) - Indices 12-14
  textureArray[12] = LoadTexture("src/Graphisme/assets/tank_bullet/tank1.png");
  textureArray[13] = LoadTexture("src/Graphisme/assets/tank_bullet/tank2.png");
  textureArray[14] =
      LoadTexture("src/Graphisme/assets/tank_bullet/bullet2.png");

  // 3. Load PowerUps - Indices 15-20
  // Order: Speed, Sniper, MachineGun, Drone, Bomb, Life
  textureArray[15] =
      LoadTexture("src/Graphisme/assets/powerUp/speed.png"); // Index 0 (Speed)
  textureArray[16] =
      LoadTexture("src/Graphisme/assets/powerUp/sniper.png"); // Index 1
  textureArray[17] =
      LoadTexture("src/Graphisme/assets/powerUp/machinegun.png"); // Index 2
  textureArray[18] =
      LoadTexture("src/Graphisme/assets/powerUp/drone.png"); // Index 3
  textureArray[19] =
      LoadTexture("src/Graphisme/assets/powerUp/bomb.png"); // Index 4
  textureArray[20] =
      LoadTexture("src/Graphisme/assets/powerUp/life.png"); // Index 5

  return textureArray;
}

void tileMap(float dU, int *visualMap, int W, int H, Texture2D *textureArray)
{
  ClearBackground(RAYWHITE);

  Texture2D bg = textureArray[0];
  DrawTexturePro(bg, (Rectangle){0, 0, bg.width, bg.height}, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                 (Vector2){0, 0},
                 0.0f,
                 WHITE);
  for (int i = 0; i < W; i++)
  {
    for (int j = 0; j < H; j++)
    {
      int visualID = visualMap[j * W + i];

      // Safety check
      if (visualID < 0 || visualID >= 12)
        visualID = 0;
      if (textureArray[visualID].id == 0)
        visualID = 0;

      Texture2D tex = textureArray[visualID];

      DrawTexturePro(tex,
                     (Rectangle){0, 0, (float)tex.width, (float)tex.height},
                     (Rectangle){(int)(dU * i), (int)(dU * j), dU + 1, dU + 1},
                     (Vector2){0, 0}, 0.0f, WHITE);
    }
  }
}
Color playerColors[4] = {YELLOW, LIGHTGRAY, SKYBLUE, GREEN};

void drawPlayers(float dU, player *players, int playerCount,
                 Texture2D *textureArray, User *users[])
{
  Texture2D bodyTex = textureArray[12];
  Texture2D canonTex = textureArray[13];

  for (int i = 0; i < playerCount; i++)
  {
    player *p = &players[i];

    Vector2 tankCenterPixels = {(p->posX) * dU, p->posY * dU};

    float bodyDrawSize = p->size * dU * 1.2f;

    float canonWidth = bodyDrawSize * (p->powerUp.sniper ? 3.0f : 1.8f);
    float canonHeight = bodyDrawSize * 1.0f;

    // body
    DrawTexturePro(
        bodyTex, (Rectangle){0, 0, (float)bodyTex.width, (float)bodyTex.height},
        (Rectangle){tankCenterPixels.x - 0.2 * dU, tankCenterPixels.y,
                    bodyDrawSize + 0.4 * dU, bodyDrawSize},
        (Vector2){bodyDrawSize / 2 + 0.2 * dU, bodyDrawSize / 2},
        p->lastRenderOrientation,
        Fade(playerColors[i], 1 - (float)(p->respawnTimer) / 120));

    // canon

    DrawTexturePro(
        canonTex,
        (Rectangle){0, 0, (float)canonTex.width, (float)canonTex.height},
        (Rectangle){tankCenterPixels.x, tankCenterPixels.y, canonWidth,
                    canonHeight},
        (Vector2){canonWidth / 2 + 0.5 * dU, canonHeight / 2},
        p->aimOrientation + 180,
        Fade(playerColors[i], 1 - (float)(p->respawnTimer) / 120));

    // life bar
    DrawRectangle(tankCenterPixels.x - bodyDrawSize / 2,
                  tankCenterPixels.y - bodyDrawSize / 2 - 10, bodyDrawSize, 5,
                  WHITE);
    // Draw Username
    Color healthColor = RED;
    if (users && users[i])
    {
      // 0=BLUE, 1=RED, 2=GREEN, 3=ORANGE (GameSetup definitions)
      // Standard Raylib colors: BLUE, RED, GREEN, ORANGE
      switch (users[i]->currentSkinID)
      {
      case 0:
        healthColor = BLUE;
        break;
      case 1:
        healthColor = RED;
        break;
      case 2:
        healthColor = GREEN;
        break;
      case 3:
        healthColor = ORANGE;
        break;
      default:
        healthColor = RED;
        break;
      }

      const char *name = users[i]->username;
      int txtW = MeasureText(name, 10);
      DrawText(name, tankCenterPixels.x - txtW / 2,
               tankCenterPixels.y - bodyDrawSize / 2 - 22, 10, WHITE);
    }

    DrawRectangle(tankCenterPixels.x - bodyDrawSize / 2,
                  tankCenterPixels.y - bodyDrawSize / 2 - 10,
                  bodyDrawSize * (p->livesLeft / 3.0f), 5, healthColor);
  }
}
void drawProjectiles(float dU, projectileList *pr, Texture2D *textureArray)
{
  Texture2D bulletTex = textureArray[14];

  for (int i = 0; i < pr->size; i++)
  {
    projectile proj = pr->data[i];

    Vector2 bulletCenter = {proj.posX * dU, proj.posY * dU};

    float bWidth = proj.size * 1.5f * dU;
    float bHeight = proj.size * 0.8f * dU;

    DrawTexturePro(
        bulletTex,
        (Rectangle){0, 0, (float)bulletTex.width, (float)bulletTex.height},
        (Rectangle){bulletCenter.x, bulletCenter.y, bWidth, bHeight},
        (Vector2){bWidth / 2, bHeight / 2}, proj.Orientation + 180.0f, WHITE);

    // hitbox for debug
    // DrawRectangleLinesEx((Rectangle){(proj.posX - proj.size/2)*dU, (proj.posY
    // - proj.size/2)*dU, proj.size*dU, proj.size*dU}, 1, RED);
  }
}

void drawPowerUp(float dU, powerUp pu, Texture2D *textureArray)
{
  Texture2D bodyTex = textureArray[pu.index + 15];
  Rectangle puShape = {pu.posX * dU, pu.posY * dU, 2 * dU, 2 * dU};
  DrawTexturePro(bodyTex,
                 (Rectangle){0, 0, (float)bodyTex.width, (float)bodyTex.height},
                 puShape, (Vector2){0, 0}, 0.0f, WHITE);
}
