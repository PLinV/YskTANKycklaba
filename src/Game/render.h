#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct player
{
    float posX; // top left corner hitbox, in tiles
    float posY;
    int size;
    float speed; // in tiles

} player;

#define W 25 // tile map, number of tiles
#define H 16

float tileMap(float dU);
void drawEveryTile(float dU);
void drawPlayers(float dU, player *p);
