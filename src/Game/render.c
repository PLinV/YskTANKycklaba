#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

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
void drawPlayers(float dU, player *players, int playerCount, Texture2D *textureArray)
{
    Texture2D bodyTex = textureArray[3];
    Texture2D canonTex = textureArray[4];
    Color playerColors[4] = {YELLOW, LIGHTGRAY, SKYBLUE, GREEN};

    for (int i = 0; i < playerCount; i++)
    {
        player *p = &players[i];

        Vector2 tankCenterPixels = {(p->posX) * dU, p->posY * dU};

        float bodyDrawSize = p->size * dU * 1.2f;

        float canonWidth = bodyDrawSize * (p->powerUp.sniper ? 2.5f : 1.8f);
        float canonHeight = bodyDrawSize * 1.0f;

        // body
        DrawTexturePro(
            bodyTex,
            (Rectangle){0, 0, (float)bodyTex.width, (float)bodyTex.height},
            (Rectangle){tankCenterPixels.x - 0.2 * dU, tankCenterPixels.y, bodyDrawSize + 0.4 * dU, bodyDrawSize},
            (Vector2){bodyDrawSize / 2, bodyDrawSize / 2},
            0.0f,
            playerColors[i]);

        // canon

        DrawTexturePro(
            canonTex,
            (Rectangle){0, 0, (float)canonTex.width, (float)canonTex.height},
            (Rectangle){tankCenterPixels.x, tankCenterPixels.y, canonWidth, canonHeight},
            (Vector2){canonWidth / 2 + 0.5 * dU, canonHeight / 2},
            p->aimOrientation + 180,
            playerColors[i]);

        DrawRectangle(tankCenterPixels.x - bodyDrawSize / 2, tankCenterPixels.y - bodyDrawSize / 2 - 10, bodyDrawSize * (p->livesLeft / 3.0f), 5, RED);
    }
}

void drawProjectiles(float dU, projectileList *pr)
{
    for (int i = 0; i < pr->size; i++) // iterating on each projectile
    {
        projectile proj = pr->data[i];
        Rectangle projShape = {(proj.posX - proj.size / 2) * dU, (proj.posY - proj.size / 2) * dU, proj.size * dU, proj.size * dU};
        Color c = {proj.playerId * 60, proj.playerId * 100, proj.playerId * 60, 255};
        DrawRectangleRec(projShape, c);
    }
}

void drawPowerUp(float dU, powerUp pu)
{
    Color c = {pu.index * 40, pu.index * 20, 255 - 40 * pu.index, 255};
    Rectangle puShape = {pu.posX * dU, pu.posY * dU, 2 * dU, 2 * dU};
    DrawRectangleRec(puShape, c);
}
