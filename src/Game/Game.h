#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "render.h"

void mainLoop(player *p, projectileList *pr, float dU, char *map, int width,
              int height, Texture2D bg);
void movePlayers(player *p, char *map, int width, int height);
void listAdd(projectileList *pr, projectile p);
void actions(player *players, projectileList *pr);
int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical,
                      char *map, int width, int height);
void initGame();
void RunGame(char *mapPath);
