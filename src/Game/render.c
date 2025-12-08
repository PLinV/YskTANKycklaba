#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "render.h"

void tileMap(float dU, char *map, int width, int height, Texture2D bg) {
  ClearBackground(RAYWHITE);

  // Draw the background image scaled to the screen
  // The image corresponds to the full map
  // dU is the size of one tile.
  // Total width in pixels = width * dU
  // Total height in pixels = height * dU

  if (bg.id != 0) {
    Rectangle source = {0, 0, bg.width, bg.height};
    // Note: bg.height might need to be negative if flipped, but usually
    // LoadTexture is fine. However, the editor flipped it vertically for
    // OpenGL. Let's see. Raylib LoadTexture usually gives correct orientation.

    Rectangle dest = {0, 0, width * dU, height * dU};
    DrawTexturePro(bg, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
  } else {
    // Fallback if no texture: draw collision map
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        DrawRectangle((int)(dU * i), (int)(dU * j), dU + 1, dU + 1, GRAY);
      }
    }
  }
}

void drawEveryTile(float dU, char *map, int width, int height, Texture2D bg) {
  tileMap(dU, map, width, height, bg);

  // un obstacle (Legacy hardcoded test object?)
  // DrawRectangle(60 * dU, 60 * dU, 30 * dU, 40 * dU, RED);
}

void drawPlayers(float dU, player *players) {
  for (int i = 0; i < 2; i++) {
    player *p = &players[i];
    Vector2 pos = {(p->posX - p->size / 2), (p->posY - p->size / 2)};

    // body
    DrawRectangle(pos.x * dU, pos.y * dU, p->size * dU, p->size * dU,
                  (Color){i * 255, 0, i * 255, 255});
    // canon
    DrawRectanglePro((Rectangle){(pos.x + p->size / 2) * dU,
                                 (pos.y + p->size / 2) * dU, 0.1 * dU, 1 * dU},
                     (Vector2){0, 0}, p->aimOrientation,
                     (Color){0, 200, 100, 255});
  }
}

void drawProjectiles(float dU, projectileList *pr) {
  for (int i = 0; i < pr->size; i++) // iterating on each projectile
  {
    projectile proj = pr->data[i];
    Rectangle projShape = {proj.posX * dU, proj.posY * dU, 0.1 * dU, 1 * dU};
    Color c = {0, 0, proj.playerId * 255, 255};
    DrawRectanglePro(projShape, (Vector2){0, 0}, proj.Orientation, c);
  }
}
