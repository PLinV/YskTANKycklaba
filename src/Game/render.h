#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct player
{
    float posX; // top left corner hitbox, in tiles
    float posY;
    float size;
    float speed;          // in tiles
    float aimOrientation; // 0-

} player;

typedef struct projectile
{
    float posX; // top left corner hitbox, in tiles
    float posY;
    float speedX; // in tiles unit
    float speedY;

} projectile;

typedef struct projectileList
{
    int size; // in tiles
    projectile *data;
    int capacity;

} projectileList;

#define W 25 // tile map, number of tiles
#define H 16

float tileMap(float dU, char *map);
void drawEveryTile(float dU, char *map);
void drawPlayers(float dU, player *p);
