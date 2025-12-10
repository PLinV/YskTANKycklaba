// #include "raylib.h"
// #include <stdio.h>
// #include <math.h>

// int main(void)
// {

//     SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT); // Standard flags
//     // TRY ADDING THIS:
//     SetTraceLogLevel(LOG_ALL); //
//     InitWindow(400, 200, "INPUT SCANNER");
//     SetTargetFPS(60);
//     SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT); // Standard flags
//     // TRY ADDING THIS:
//     SetTraceLogLevel(LOG_ALL); //
//     while (!WindowShouldClose())
//     {
//         BeginDrawing();
//         ClearBackground(BLACK);

//         int inputFound = 0;

//         // Scan all 4 possible gamepad slots (0 to 3)
//         for (int gp = 0; gp < 4; gp++)
//         {
//             if (IsGamepadAvailable(gp))
//             {
//                 DrawText(TextFormat("PAD %d DETECTED: %s", gp, GetGamepadName(gp)), 10, 10 + (gp * 30), 10, GREEN);

//                 // Scan all 6 standard axes
//                 for (int axis = 0; axis < 6; axis++)
//                 {
//                     float val = GetGamepadAxisMovement(gp, axis);

//                     // If we detect ANY movement (ignoring tiny drift)
//                     if (fabs(val) > 0.2f)
//                     {
//                         inputFound = 1;
//                         DrawText(TextFormat(">> PAD %d AXIS %d IS MOVING: %.2f", gp, axis, val), 10, 150, 20, YELLOW);
//                     }
//                 }

//                 // Scan all buttons
//                 for (int btn = 0; btn < 32; btn++)
//                 {
//                     if (IsGamepadButtonDown(gp, btn))
//                     {
//                         inputFound = 1;
//                         DrawText(TextFormat(">> PAD %d BUTTON %d PRESSED", gp, btn), 10, 180, 20, MAGENTA);
//                     }
//                 }
//             }
//         }

//         if (!inputFound)
//         {
//             DrawText("Move sticks to scan...", 10, 130, 20, DARKGRAY);
//         }

//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }