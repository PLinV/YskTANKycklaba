
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct powerUpPlayer // the logic object (sub type) in player
{
    int timer;
    bool machineGun;
    bool sniper;

} powerUpPlayer;

typedef struct movement
{
    int keyUp;
    int keyDown;
    int keyLeft;
    int keyRight;
    int keyTurnRight;
    int keyTurnLeft;
    int keyShoot;
    bool usesController;
    int controllerIndex;

} movement;

typedef struct player
{
    float posX; // top left corner hitbox, in tiles
    float posY;
    float size;
    float speed;          // in tiles
    float aimOrientation; // 0-
    int playerId;
    int shootTimer;
    powerUpPlayer powerUp;
    movement move;

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

#define W 50 // tile map, number of tiles
#define H 32

float tileMap(float dU, char *map);
void drawEveryTile(float dU, char *map);
void drawPlayers(float dU, player *p, int playerCount);
void drawProjectiles(float dU, projectileList *pr);
void drawPowerUp(float dU, powerUp pu);
