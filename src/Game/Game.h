#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#include "render.h"

void mainLoop(player *p, float dU);
void movePlayers(player *p);
int IsThereAnObstacle(Rectangle player, float velocity, bool checkVertical);
void initGame();
