#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct powerUpPlayer // the logic object (sub type) in player
{
    int timer;
    bool bomb;
    bool machineGun;
    bool sniper;
    bool speed;

} powerUpPlayer;

typedef struct player
{
    float posX; // top left corner hitbox, in tiles
    float posY;
    float size;
    float speed;          // in tiles
    float aimOrientation; // 0-
    int playerId;
    powerUpPlayer powerUp;

} player;

typedef struct projectile
{
    int playerId;
    float posX; // top left corner hitbox, in tiles
    float posY;
    float speedX; // in tiles unit
    float speedY;
    float Orientation; // angle in degrees
    float size;        // in tile unit
    int bounceLeft;

} projectile;

typedef struct projectileList
{
    int size; // in tiles
    projectile *data;
    int capacity;

} projectileList;

typedef struct powerUp // the physical object
{
    int timer;
    float posX;
    float posY;
    bool hasSpawnYet;
    int index; // (0-5) depending on what effect
} powerUp;

#define W 25 // tile map, number of tiles
#define H 16

float tileMap(float dU, char *map);
void drawEveryTile(float dU, char *map);
void drawPlayers(float dU, player *p);
void drawProjectiles(float dU, projectileList *pr);
void drawPowerUp(float dU, powerUp pu);
