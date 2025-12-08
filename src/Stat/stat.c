#include "stat.h"
#include "raylib.h"

void RunStats(void) {
  // Ensure window size is correct
  // SetWindowSize(1000, 640); // Removed for dynamic resizing
  SetWindowTitle("YskTANKycklaba - Statistics");

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_BACKSPACE))
      break; // Return to menu

    BeginDrawing();
    ClearBackground(RAYWHITE);

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const char *title = "STATISTICS";
    const char *wip = "Work in Progress...";
    const char *back = "Press BACKSPACE to return to Menu";

    DrawText(title, sw / 2 - MeasureText(title, 50) / 2, sh * 0.3, 50,
             DARKGRAY);
    DrawText(wip, sw / 2 - MeasureText(wip, 30) / 2, sh * 0.5, 30, GRAY);
    DrawText(back, sw / 2 - MeasureText(back, 20) / 2, sh * 0.8, 20, LIGHTGRAY);

    EndDrawing();
  }
}
