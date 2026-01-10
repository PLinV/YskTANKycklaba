#include "./Game/Game.h"

void initWindow()
{
    const char *title = "YskTANKycklaba";
    InitWindow(1000, 640, title);
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
}

int main()
{
    initWindow();

    // main loop
    StartGame();
}