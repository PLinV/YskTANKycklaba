#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

float tileMap(float dU, char *map, int W, int H)
{

    ClearBackground(RAYWHITE);

    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < H; j++)
        {
            if (map[j * W + i] == 1)
            {
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){100, 150, 100, 255});
            }
            else if (map[j * W + i] == 2)
            {
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){255, 100, 100, 255});
            }
            else if (map[j * W + i] == 0)
            {
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){200, 200, 200, 255});
            }
            else
            {
                // default color for error
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){150, 150, 200, 255});
            }
        }
    }
}

void drawEveryTile(float dU, char *map, int W, int H)
{
    tileMap(dU, map, W, H);

    // un obstacle
    DrawRectangle(60 * dU, 60 * dU, 30 * dU, 40 * dU, RED);
}

void drawPlayers(float dU, player *players, int playerCount)
{
    for (int i = 0; i < playerCount; i++)
    {
        player *p = &players[i];
        Vector2 pos = {(p->posX - p->size / 2), (p->posY - p->size / 2)};

        // body
        DrawRectangle(pos.x * dU, pos.y * dU, p->size * dU, p->size * dU, (Color){i * 60, i * 100, i * 60, 255 * (120 - p->respawnTimer) / 120});
        // canon
        float canon_size = p->powerUp.sniper ? 1.5 : 1.0;
        DrawRectanglePro((Rectangle){(pos.x + p->size / 2) * dU, (pos.y + p->size / 2) * dU, canon_size * dU, 0.1 * dU}, (Vector2){0, 0}, p->aimOrientation, (Color){0, 200, 100, 255 * (120 - p->respawnTimer) / 120});
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
