#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

void mainLoop(player *p, int pC,projectileList *pr, powerUp *pu, float dU, char *map);
void movePlayers(player *p,int pC, char *map);
void listAdd(projectileList *pr, projectile p);
void listRem(int projectileIndex, projectileList *pr);
void actions(player *players, int pC, projectileList *pr);
void spawnBullet(player p, projectileList *pr, float custom_orientation, float size, int bounce_left, float speed);
void powerUps(powerUp *pu, char *map, player *p,int pC, projectileList *pr);
void updateProjectiles(projectileList *pr, player *p, char *map, int pC);
int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical, char *map, bool isPlayer);
void initGame();
