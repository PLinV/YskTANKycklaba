#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

void movePlayers(player *players, int pC, char *map);
void actions(player *players, int pC, projectileList *pr);
void updateProjectiles(projectileList *pr, player *p, int pC, char *map);
void powerUps(powerUp *pu, char *map, player *p, int pC, projectileList *pr);
void drawEveryTile(float dU, char *map);
void drawPlayers(float dU, player *p, int playerCount);
void drawProjectiles(float dU, projectileList *pr);
void drawPowerUp(float dU, powerUp pu);