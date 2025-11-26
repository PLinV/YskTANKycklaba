#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

float tileMap(float dU, char *map)
{

    ClearBackground(RAYWHITE);

    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < H; j++)
        {
            if (map[(j * W + i) * 2] == '1')
            {
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){0, 255, 0, 255});
            }
            else
            {
                DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, (Color){255, i * 10, j * 10, 255});
            }
        }
    }
}

void drawEveryTile(float dU, char *map)
{
    tileMap(dU, map);

    // un obstacle
    DrawRectangle(60 * dU, 60 * dU, 30 * dU, 40 * dU, RED);
}

void drawPlayers(float dU, player *players)
{
    for (int i = 0; i < 2; i++)
    {
        player *p = &players[i];
        Vector2 pos = {(p->posX - p->size / 2), (p->posY - p->size / 2)};

        // body
        DrawRectangle(pos.x * dU, pos.y * dU, p->size * dU, p->size * dU, (Color){i * 255, 0, i * 255, 255});
        // canon
        DrawRectanglePro((Rectangle){(pos.x + p->size / 2) * dU, (pos.y + p->size / 2) * dU, 0.1 * dU, 1 * dU}, (Vector2){0, 0}, p->aimOrientation, (Color){0, 200, 100, 255});
    }
}

void drawProjectiles(float dU, projectileList *pr)
{
    for (int i = 0; i < pr->size; i++) // iterating on each projectile
    {
        projectile proj = pr->data[i];
        Rectangle projShape = {proj.posX * dU, proj.posY * dU, 1 * dU, 1 * dU};
        Color c = {0, 0, proj.playerId * 255, 255};
        DrawRectanglePro(projShape, (Vector2){0, 0}, proj.Orientation, c);
    }
}
