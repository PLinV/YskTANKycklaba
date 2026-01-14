#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "render.h"

void mainLoop(player *p, int playerCount, projectileList *pr, powerUp *pu,
              float dU, char *map, int *visualMap, int underlayId, int W, int H,
              bool *exitGame, Texture2D *textureArray);
void movePlayers(player *players, int pC, char *map, int W, int H);
void actions(player *players, int pC, projectileList *pr);
void updateProjectiles(projectileList *pr, player *p, int pC, char *map,
                       int *visualMap, int underlayId, int W, int H,
                       bool *exitGame);
void checkPlayersGettingHit(projectile *proj, Rectangle ProjHitbox, player *p,
                            int pC, char *map, int W, int H, bool *exitGame);
void randomCoordsWithDistance(int *XY, player *p, char *map, int pC,
                              int distanceMinToPlayer, int W, int H);
void powerUps(powerUp *pu, char *map, player *p, int pC, projectileList *pr,
              int W, int H);
void listRem(int projectileIndex, projectileList *pr);
int RunGame(char *mapPath, int playersCount, User* users[]);
