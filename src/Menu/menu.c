#include "menu.h"
#include "../Game/Game.h"
#include "../Graphisme/graphisme.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Helper function to draw a button
bool DrawButton(Rectangle bounds, const char *text) {
  bool pressed = false;
  Vector2 mousePoint = GetMousePosition();
  bool isHover = CheckCollisionPointRec(mousePoint, bounds);

  if (isHover) {
    DrawRectangleRec(bounds, LIGHTGRAY);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      pressed = true;
    }
  } else {
    DrawRectangleRec(bounds, RAYWHITE);
  }

  DrawRectangleLinesEx(bounds, 2, DARKGRAY);

  // Center text
  int textWidth = MeasureText(text, 20);
  DrawText(text, bounds.x + (bounds.width - textWidth) / 2,
           bounds.y + (bounds.height - 20) / 2, 20, DARKGRAY);

  return pressed;
}

// --- NOUVELLE FONCTION POUR LE BOUTON IMAGE ---
bool DrawImageButton(Rectangle bounds, Texture2D texture) {
    bool pressed = false;
    Vector2 mousePoint = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mousePoint, bounds);
    
    // Changement de teinte au survol pour donner un effet visuel
    // WHITE = couleur normale, LIGHTGRAY = un peu plus sombre quand on passe la souris
    Color tint = isHover ? LIGHTGRAY : WHITE; 

    if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        pressed = true;
    }

    // On dessine l'image en l'étirant pour qu'elle remplisse le rectangle "bounds"
    DrawTexturePro(
        texture,
        (Rectangle){0, 0, (float)texture.width, (float)texture.height}, // Source (toute l'image)
        bounds, // Destination (taille du bouton définie dans le main)
        (Vector2){0, 0},
        0.0f,
        tint
    );

    return pressed;
}

void SelectMap(char *selectedMapPath) {
  FilePathList files = LoadDirectoryFiles("src/Graphisme/Maps");
  int selectedIndex = -1;
  bool mapSelected = false;

  while (!WindowShouldClose() && !mapSelected) {
    if (IsKeyPressed(KEY_BACKSPACE)) {
      selectedMapPath[0] = '\0';
      break;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("SELECT A MAP", 10, 10, 20, DARKGRAY);
    DrawText("Press BACKSPACE to cancel", 10, 40, 10, GRAY);

    int y = 70;
    for (int i = 0; i < (int)files.count; i++) {
      if (IsFileExtension(files.paths[i], ".txt")) {
        Rectangle btnBounds = {20, y, 300, 30};

        // Highlight selected
        if (selectedIndex == i)
          DrawRectangleRec(btnBounds, SKYBLUE);

        if (CheckCollisionPointRec(GetMousePosition(), btnBounds)) {
          if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            selectedIndex = i;
          }
        }

        DrawText(GetFileName(files.paths[i]), 25, y + 5, 20, BLACK);
        y += 35;
      }
    }

    if (selectedIndex != -1) {
      if (DrawButton((Rectangle){400, 100, 200, 50}, "START GAME")) {
        strcpy(selectedMapPath, files.paths[selectedIndex]);
        mapSelected = true;
      }
    }

    EndDrawing();
  }
  UnloadDirectoryFiles(files);
}

int main() {
  InitWindow(1000, 640, "YskTANKycklaba - Menu");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  Texture2D background = LoadTexture("src/Menu/images/background_start.jpg");
  // TEST POUR VOIR SI L'IMAGE CHARGE BIEN
  if (background.id == 0) { 
    printf("Failed to load background image.\n");
  }

  Texture2D btnPlayImg = LoadTexture("src/Menu/images/bouton_jouer.jpg");
  if (btnPlayImg.id == 0) { 
    printf("Failed to load image bouton jouer.\n");
  }

  // --- 2. BOUCLE PRINCIPALE ---
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTexturePro(
      background, 
      (Rectangle){ 0, 0, (float)background.width, (float)background.height }, // Source (toute l'image)
      (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, // Destination (tout l'écran)
      (Vector2){ 0, 0 }, 
      0.0f, 
      WHITE // Teinte (WHITE = couleur originale)
    );

    int centerX = GetScreenWidth() / 2 - 100;
    int startY = GetScreenHeight() / 2 - 100;
    int buttonWidth = 200;
    int buttonHeight = 50;
    int spacing = 20;


    // 1. Jouer
    if (DrawImageButton((Rectangle){centerX, startY, buttonWidth, buttonHeight}, btnPlayImg)) {
      char mapPath[512] = {0};
      SelectMap(mapPath);
      if (mapPath[0] != '\0') {
        RunGame(mapPath, 4); //4 is the player count, you can set it between (1-4)
        SetWindowTitle("YskTANKycklaba - Menu");
      }
    }

    // 2. Statistiques
    if (DrawButton((Rectangle){centerX, startY + buttonHeight + spacing,
                               buttonWidth, buttonHeight},
                   "STATISTIQUES")) {
      RunStats();
      SetWindowTitle("YskTANKycklaba - Menu");
    }

    // 3. Map Editor
    if (DrawButton((Rectangle){centerX, startY + 2 * (buttonHeight + spacing),
                               buttonWidth, buttonHeight},
                   "MAP EDITOR")) {
      RunEditor();
      SetWindowTitle("YskTANKycklaba - Menu");
    }

    // 4. Quitter
    if (DrawButton((Rectangle){centerX, startY + 3 * (buttonHeight + spacing),
                               buttonWidth, buttonHeight},
                   "QUITTER")) {
      break;
    }

    EndDrawing();
  }

  // --- 3. DÉCHARGEMENT DE LA MÉMOIRE ---
  UnloadTexture(background);

  CloseWindow();
  return 0;
}
