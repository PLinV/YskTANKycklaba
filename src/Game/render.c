#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

float tileMap(float dU)
{

    ClearBackground(RAYWHITE);

    const char *nom = "./YskTANKycklaba/src/Game/map.txt";
    char *txt = LoadFileText(nom);
    printf("%s\n", txt);

    for (int i = 0; i < W + 1; i++)
    {
        for (int j = 0; j < H + 1; j++)
        {
            DrawRectangle((int)(dU * i), (int)(dU * j), 40, 40, (Color){1, i * 10, j * 10, 100});
            if(txt[i*W+j*H]=='0'){
                DrawRectangle((int)(dU * i), (int)(dU * j), 40, 40, (Color){1, i * 10, j * 10, 100});
            }
        }
    }
    DrawLine(1 * dU, 1 * dU, ((W - 1)) * dU, 1 * dU, (Color){200, 0, 0, 255});
    DrawLine((W - 1) * dU, 1 * dU, (W - 1) * dU, (H - 1) * dU, (Color){200, 0, 0, 255});
    DrawLine((W - 1) * dU, (H - 1) * dU, 1 * dU, (H - 1) * dU, (Color){200, 0, 0, 255});
    DrawLine(1 * dU, (H - 1) * dU, 1 * dU, 1 * dU, (Color){200, 0, 0, 255});
}

void drawEveryTile(float dU)
{
    tileMap(dU);

    // un obstacle
    DrawRectangle(60 * dU, 60 * dU, 30 * dU, 40 * dU, RED);
}

void drawPlayers(float dU, player *p)
{
    DrawRectangle((p->posX - p->size / 2) * dU, (p->posY - p->size / 2) * dU, p->size * dU, p->size * dU, YELLOW);
}
