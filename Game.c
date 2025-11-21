#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define STANDARD_SIZE_CHARACTER
#define 

typedef struct rect_infos{
    int left;
    int right;
    int top;
    int bottom;
    Vector2 center;
} rect_infos;


typedef struct {};


void initR(Rectangle* R, int x, int y, int width, int height){
    R->x = x;
    R->y = y;
    R->width = width;
    R->height = height;
}

void event(Rectangle* R){

    if(IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT))
        R->x -= 50;
    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        R->x += 50;
    if(IsKeyDown(KEY_Z) || IsKeyDown(KEY_UP))
        R->y -= 50;
    if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        R->y += 50;
}

int main(){
    InitWindow(1500, 1500, "Projet C - DaMS");

    SetTargetFPS(30);
    SetMousePosition(750, 750);


    Rectangle* R = malloc(sizeof(Rectangle));
    initR(R, 750, 750, 100, 100);
    
    while (!WindowShouldClose())
    {
        ClearBackground(WHITE);
        BeginDrawing();

        event(R);
        DrawRectangleRec(*R, RED);

        EndDrawing();
      
    }
    CloseWindow();

    return 0;
}