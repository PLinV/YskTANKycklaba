#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

void mainLoop(player *p, int playerCount, projectileList *pr, powerUp *pu, float dU, char *map, int W, int H, bool *exitGame);
void movePlayers(player *players, int pC, char *map, int W, int H);
void actions(player *players, int pC, projectileList *pr);
void updateProjectiles(projectileList *pr, player *p, int pC, char *map, int W, int H, bool *exitGame);
void checkPlayersGettingHit(projectile *proj, Rectangle ProjHitbox, player *p, int pC, char *map, int W, int H, bool *exitGame);
void randomCoordsWithDistance(int *XY, player *p, char *map, int pC, int distanceMinToPlayer, int W, int H);
void powerUps(powerUp *pu, char *map, player *p, int pC, projectileList *pr, int W, int H);
void listRem(int projectileIndex, projectileList *pr);
void drawEveryTile(float dU, char *map, int W, int H);
void drawPlayers(float dU, player *p, int playerCount);
void drawProjectiles(float dU, projectileList *pr);
void drawPowerUp(float dU, powerUp pu);
void RunGame(char *mapPath, int playersCount);
