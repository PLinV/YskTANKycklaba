#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct rect_infos
{
    int left;
    int right;
    int top;
    int bottom;
    Vector2 center;
} rect_infos;

#define SCREEN_WIDTH 700
#define SCREEN_LENGTH 700

void initR(Rectangle *R, int x, int y, int width, int height)
{
    R->x = x;
    R->y = y;
    R->width = width;
    R->height = height;
}

void event(Rectangle *R)
{

    if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT))
        R->x -= 50;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        R->x += 50;
    if (IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP))
        R->y -= 50;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        R->y += 50;
}

int main(void)
{
    // Desired window size as a percentage of the monitor
    float scale = 0.2f; // 80%

    // Get primary monitor size
    int monitorWidth = GetMonitorWidth(0);
    int monitorHeight = GetMonitorHeight(0);

    // Scale the window size
    int windowWidth = (int)(monitorWidth * scale);
    int windowHeight = (int)(monitorHeight * scale);

    // Center the window manually (optional)
    int posX = (monitorWidth - windowWidth) / 2;
    int posY = (monitorHeight - windowHeight) / 2;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Optional: make it resizable
    InitWindow(windowWidth, windowHeight, "Percentage Window");

    // Center the window (on Linux/Windows this works)
    SetWindowPosition(posX, posY);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Window is 80% of screen", 50, 50, 30, MAROON);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}