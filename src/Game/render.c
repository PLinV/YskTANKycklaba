#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

Texture2D *Loadtextures()
{
    Texture2D *textureArray = (Texture2D *)malloc(sizeof(Texture2D) * 12);

    if (textureArray == NULL)
    {
        printf("ERROR LOADING TEXTURES");
    }
    textureArray[0] = LoadTexture("src/Graphisme/assets/ground4.jpg");
    textureArray[1] = LoadTexture("src/Graphisme/assets/wall3.jpg");
    textureArray[2] = LoadTexture("src/Graphisme/assets/box2.jpg");
    textureArray[3] = LoadTexture("src/Graphisme/assets/tank1.png");
    textureArray[4] = LoadTexture("src/Graphisme/assets/tank2.png");
    textureArray[5] = LoadTexture("src/Graphisme/assets/bullet2.png");
    textureArray[6] = LoadTexture("src/Graphisme/assets/speed.png");
    textureArray[7] = LoadTexture("src/Graphisme/assets/sniper.png");
    textureArray[8] = LoadTexture("src/Graphisme/assets/machinegun.png");
    textureArray[9] = LoadTexture("src/Graphisme/assets/drone.png");
    textureArray[10] = LoadTexture("src/Graphisme/assets/bomb.png");
    textureArray[11] = LoadTexture("src/Graphisme/assets/life.png");
    return textureArray;
}

float tileMap(float dU, char *map, int W, int H, Texture2D *textureArray)
{
    Texture2D ground = textureArray[0];
    Texture2D wall = textureArray[1];
    Texture2D box = textureArray[2];

    ClearBackground(RAYWHITE);

    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < H; j++)
        {
            if (map[j * W + i] == 1)
            {
                DrawTexturePro(
                    wall,
                    (Rectangle){0, 0, (float)wall.width, (float)wall.height},
                    (Rectangle){(int)(dU * i), (int)(dU * j), dU + 1, dU + 1},
                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
            else if (map[j * W + i] == 2)
            {
                DrawTexturePro(
                    box,
                    (Rectangle){0, 0, (float)box.width, (float)box.height},
                    (Rectangle){(int)(dU * i), (int)(dU * j), dU + 1, dU + 1},
                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
            else if (map[j * W + i] == 0)
            {

                DrawTexturePro(
                    ground,
                    (Rectangle){0, 0, (float)ground.width, (float)ground.height},
                    (Rectangle){(int)(dU * i), (int)(dU * j), dU + 1, dU + 1},
                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
            else
            {
                // default color for error
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){255, 0, 0, 255});
            }
        }
    }
}
Color playerColors[4] = {YELLOW, LIGHTGRAY, SKYBLUE, GREEN};

void drawPlayers(float dU, player *players, int playerCount, Texture2D *textureArray)
{
    Texture2D bodyTex = textureArray[3];
    Texture2D canonTex = textureArray[4];

    for (int i = 0; i < playerCount; i++)
    {
        player *p = &players[i];

        Vector2 tankCenterPixels = {(p->posX) * dU, p->posY * dU};

        float bodyDrawSize = p->size * dU * 1.2f;

        float canonWidth = bodyDrawSize * (p->powerUp.sniper ? 3.0f : 1.8f);
        float canonHeight = bodyDrawSize * 1.0f;

        // body
        DrawTexturePro(
            bodyTex,
            (Rectangle){0, 0, (float)bodyTex.width, (float)bodyTex.height},
            (Rectangle){tankCenterPixels.x - 0.2 * dU, tankCenterPixels.y, bodyDrawSize + 0.4 * dU, bodyDrawSize},
            (Vector2){bodyDrawSize / 2 + 0.2 * dU, bodyDrawSize / 2},
            p->lastRenderOrientation,
            Fade(playerColors[i], 1 - (float)(p->respawnTimer) / 120));

        // canon

        DrawTexturePro(
            canonTex,
            (Rectangle){0, 0, (float)canonTex.width, (float)canonTex.height},
            (Rectangle){tankCenterPixels.x, tankCenterPixels.y, canonWidth, canonHeight},
            (Vector2){canonWidth / 2 + 0.5 * dU, canonHeight / 2},
            p->aimOrientation + 180,
            Fade(playerColors[i], 1 - (float)(p->respawnTimer) / 120));

        // life bar
        DrawRectangle(tankCenterPixels.x - bodyDrawSize / 2, tankCenterPixels.y - bodyDrawSize / 2 - 10, bodyDrawSize, 5, WHITE);
        DrawRectangle(tankCenterPixels.x - bodyDrawSize / 2, tankCenterPixels.y - bodyDrawSize / 2 - 10, bodyDrawSize * (p->livesLeft / 3.0f), 5, RED);
    }
}
void drawProjectiles(float dU, projectileList *pr, Texture2D *textureArray)
{
    Texture2D bulletTex = textureArray[5];

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
            (Vector2){bWidth / 2, bHeight / 2},
            proj.Orientation + 180.0f,
            WHITE);

        // hitbox for debug
        // DrawRectangleLinesEx((Rectangle){(proj.posX - proj.size/2)*dU, (proj.posY - proj.size/2)*dU, proj.size*dU, proj.size*dU}, 1, RED);
    }
}

void drawPowerUp(float dU, powerUp pu, Texture2D *textureArray)
{
    Texture2D bodyTex = textureArray[pu.index + 6];
    Rectangle puShape = {pu.posX * dU, pu.posY * dU, 2 * dU, 2 * dU};
    DrawTexturePro(
        bodyTex,
        (Rectangle){0, 0, (float)bodyTex.width, (float)bodyTex.height},
        puShape,
        (Vector2){0, 0},
        0.0f,
        WHITE);
}
