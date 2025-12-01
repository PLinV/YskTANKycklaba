#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

void mainLoop(player *p, projectileList *pr, float dU, char *map);
void movePlayers(player *p, char *map);
void listAdd(projectileList *pr, projectile p);
void listRem(int projectileIndex, projectileList *pr);
void actions(player *players, projectileList *pr);
void spawnBullet(player p, projectileList *pr);
void updateProjectiles(projectileList *pr, char *map);
int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical, char *map);
void initGame();
