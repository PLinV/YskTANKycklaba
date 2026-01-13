#include "GameSetup.h"
#include "../dataCode/user.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> // for free? no, LoadDirectoryFiles uses Raylib allocator

typedef struct {
  char pseudo[16];
  int skinID; // 0=BLUE, 1=RED, 2=GREEN, 3=ORANGE
  Rectangle bounds;
} PlayerConfig;

// Structure to hold pre-loaded map info
typedef struct {
  char name[128];
  char path[512];
  Texture2D texture;
  bool hasTexture;
} MapUI;

// Initialize defaults
PlayerConfig setupJoueurs[4] = {
    {"J1", 0, {0}}, {"J2", 0, {0}}, {"J3", 0, {0}}, {"J4", 0, {0}}};
int totalPlayers = 2; // Par défaut

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

// 3D Cyclic Carousel Logic
void DisplayMap(MapUI *maps, int count, int *mapIndex) {
  if (count == 0)
    return;

  // Navigation (Cyclic)
  if (IsKeyPressed(KEY_RIGHT))
    *mapIndex = (*mapIndex + 1) % count;
  if (IsKeyPressed(KEY_LEFT))
    *mapIndex = (*mapIndex - 1 + count) % count;

  DrawText("Choisir une map:",
           GetScreenWidth() / 2 - MeasureText("Choisir une map:", 20) / 2,
           GetScreenHeight() * 0.60, 20, DARKGRAY);

  int current = *mapIndex;
  int prev = (current - 1 + count) % count;
  int next = (current + 1) % count;

  // CAROUSEL
  float centerX = GetScreenWidth() / 2;
  float centerY = GetScreenHeight() * 0.75;

  // Size relative to screen
  float minDimension = (GetScreenWidth() < GetScreenHeight())
                           ? GetScreenWidth()
                           : GetScreenHeight();
  float size = minDimension * 0.25;
  float x = centerX - size / 2;
  float y = centerY - size / 2;

  // CURRENT
  int aimTextWidth = MeasureText(maps[current].name, 20);
  DrawText(maps[current].name, x + size / 2 - aimTextWidth / 2, y - 30, 20,
           DARKGRAY);
  DrawRectangle(x - 5, y - 5, size + 10, size + 10, GOLD);
  DrawRectangle(x, y, size, size, RAYWHITE);

  if (maps[current].hasTexture) {
    DrawTexturePro(maps[current].texture,
                   (Rectangle){0, 0, maps[current].texture.width,
                               maps[current].texture.height},
                   (Rectangle){x, y, size, size}, (Vector2){0, 0}, 0.0f, WHITE);
  } else {
    DrawText("No Image", x + size * 0.3, y + size / 2, 20, LIGHTGRAY);
  }

  // PREVIOUS
  if (count > 1) {
    float sideSize = size * 0.66;
    float gap = size * 0.2;
    float sideX = x - gap - sideSize;
    float sideY = centerY - sideSize / 2;

    int w = MeasureText(maps[prev].name, 20);
    DrawText(maps[prev].name, sideX + sideSize / 2 - w / 2, sideY - 30, 20,
             DARKGRAY);
    DrawRectangle(sideX, sideY, sideSize, sideSize, LIGHTGRAY);
    if (maps[prev].hasTexture) {
      DrawTexturePro(maps[prev].texture,
                     (Rectangle){0, 0, maps[prev].texture.width,
                                 maps[prev].texture.height},
                     (Rectangle){sideX, sideY, sideSize, sideSize},
                     (Vector2){0, 0}, 0.0f, WHITE);
    }
  }

  // NEXT
  if (count > 1) {
    float sideSize = size * 0.66;
    float gap = size * 0.2;
    float sideX = x + size + gap;
    float sideY = centerY - sideSize / 2;

    int w = MeasureText(maps[next].name, 20);
    DrawText(maps[next].name, sideX + sideSize / 2 - w / 2, sideY - 30, 20,
             DARKGRAY);
    DrawRectangle(sideX, sideY, sideSize, sideSize, LIGHTGRAY);
    if (maps[next].hasTexture) {
      DrawTexturePro(maps[next].texture,
                     (Rectangle){0, 0, maps[next].texture.width,
                                 maps[next].texture.height},
                     (Rectangle){sideX, sideY, sideSize, sideSize},
                     (Vector2){0, 0}, 0.0f, WHITE);
    }
  }
}

void RunGameSetup(char *selectedMapPath, int *finalPlayerCount) {
  FilePathList files = LoadDirectoryFiles("src/Graphisme/Maps");

  int editingPlayer = -1;

  // Autocomplete Data
  char **allUsernames = NULL;
  int userCount = GetAllUsers(&allUsernames);
  // We need a list to store current matching results
  int maxMatches = 10;
  char **matches = malloc(maxMatches * sizeof(char *));
  int matchCount = 0;

  int mapIndex = 0;
  bool mapSelected = false;

  // Load Map Headers and Textures
  int mapCount = 0;
  // First count .txt files
  for (int i = 0; i < (int)files.count; i++) {
    if (IsFileExtension(files.paths[i], ".txt"))
      mapCount++;
  }

  MapUI *maps = malloc(mapCount * sizeof(MapUI));
  int currentIdx = 0;

  for (int i = 0; i < (int)files.count; i++) {
    if (IsFileExtension(files.paths[i], ".txt")) {
      strcpy(maps[currentIdx].path, files.paths[i]);
      const char *name = GetFileNameWithoutExt(files.paths[i]);
      strcpy(maps[currentIdx].name, name);

      // Try load texture .png
      char imgPath[512];
      strcpy(imgPath, files.paths[i]);
      // Replace .txt with .png
      char *ext = strrchr(imgPath, '.');
      if (ext)
        strcpy(ext, ".png");

      if (FileExists(imgPath)) {
        maps[currentIdx].texture = LoadTexture(imgPath);
        maps[currentIdx].hasTexture = true;
      } else
        maps[currentIdx].hasTexture = false;
      currentIdx++;
    }
  }

  while (!WindowShouldClose() && !mapSelected) {
    // --- LOGIQUE ---
    if (IsKeyPressed(KEY_BACKSPACE)) {
      if (editingPlayer != -1) {
        int len = strlen(setupJoueurs[editingPlayer].pseudo);
        if (len > 0)
          setupJoueurs[editingPlayer].pseudo[len - 1] = '\0';
      } else {
        // Only exit if not editing
        selectedMapPath[0] = '\0';
        break;
      }
    }

    // Input Handling for Pseudo
    if (editingPlayer != -1) {
      int key = GetCharPressed();
      while (key > 0) {
        int len = strlen(setupJoueurs[editingPlayer].pseudo);
        if ((key >= 32) && (key <= 125) && (len < 15)) {
          setupJoueurs[editingPlayer].pseudo[len] = (char)key;
          setupJoueurs[editingPlayer].pseudo[len + 1] = '\0';
        }
        key = GetCharPressed();
      }

      // Update Matches
      matchCount = 0;
      char *current = setupJoueurs[editingPlayer].pseudo;
      int currentLen = strlen(current);
      if (currentLen > 0) {
        for (int u = 0; u < userCount && matchCount < maxMatches; u++) {
          if (strncmp(allUsernames[u], current, currentLen) == 0) {
            matches[matchCount++] = allUsernames[u];
          }
        }
      } else {
        // Show first 10 if empty
        for (int u = 0; u < userCount && matchCount < maxMatches; u++) {
          matches[matchCount++] = allUsernames[u];
        }
      }
    } else {
      // Clear matches if not editing
      matchCount = 0;
    }

    // Mouse click outside to stop editing
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && editingPlayer != -1) {
      // Check if we clicked on a match handled in drawing, else close
      // Simple logic: we'll check collision in drawing loop.
      // If not clicked on valid UI, reset.
      // Doing this check at end of drawing might be better or flag consumption.
      // For now, let's assume if we click a new box, it changes.
    }

    // --- DESSIN ---
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw Background (White/Gray as requested by removal of image)
    // if (setupBackground.id != 0) { ... } REMOVED

    DrawText("CONFIGURATION DE LA PARTIE", 10, 10, 20, DARKGRAY);

    // 1. CHOIX DU NOMBRE DE JOUEURS (En haut)
    DrawText("Nombre de joueurs :", 350, 40, 20, BLACK);
    for (int i = 2; i <= 4; i++) {
      Rectangle r = {550 + (i - 2) * 40, 35, 30, 30};
      if (CheckCollisionPointRec(GetMousePosition(), r) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        totalPlayers = i;
      DrawRectangleRec(r, (totalPlayers == i) ? SKYBLUE : LIGHTGRAY);
      DrawText(TextFormat("%d", i), r.x + 10, r.y + 5, 20, BLACK);
    }

    // 2. ZONE JOUEURS (Pseudos et Skins)
    float sectorWidth = (float)GetScreenWidth() / totalPlayers;
    for (int i = 0; i < totalPlayers; i++) {
      float startX = i * sectorWidth;

      // Ligne de séparation
      if (i > 0)
        DrawLine(startX, 80, startX, 280, LIGHTGRAY);

      // Pseudo
      DrawText(TextFormat("Pseudo J%d:", i + 1), startX + 20, 100, 18,
               DARKGRAY);
      Rectangle pseudoRect = {startX + 20, 125, 150, 30};

      if (CheckCollisionPointRec(GetMousePosition(), pseudoRect) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        editingPlayer = i;
      }

      DrawRectangleRec(pseudoRect, (editingPlayer == i) ? SKYBLUE : LIGHTGRAY);
      DrawText(setupJoueurs[i].pseudo, pseudoRect.x + 5, pseudoRect.y + 5, 18,
               BLACK);

      // AUTOCOMPLETE LIST
      if (editingPlayer == i) {
        float listY = pseudoRect.y + pseudoRect.height;
        for (int m = 0; m < matchCount; m++) {
          Rectangle matchRect = {pseudoRect.x, listY + (m * 25),
                                 pseudoRect.width, 25};
          DrawRectangleRec(matchRect, SKYBLUE);
          DrawRectangleLinesEx(matchRect, 1, DARKBLUE);
          DrawText(matches[m], matchRect.x + 5, matchRect.y + 5, 15, BLACK);

          if (CheckCollisionPointRec(GetMousePosition(), matchRect) &&
              IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            strcpy(setupJoueurs[i].pseudo, matches[m]);
            editingPlayer = -1; // Stop editing
          }
        }

        // Button to create/confirm if not in list
        // Logic: If user typed something that isn't exactly a match (or just
        // wants to confirm new) We show a small specific button to
        // valid/create? User asked: "si c'est un nouveau pseudo on affiche sur
        // la droite de la case d'input un bouton pour confirmer" We check if
        // current pseudo exists in matches
        bool exists = false;
        for (int u = 0; u < userCount; u++) {
          if (strcmp(allUsernames[u], setupJoueurs[i].pseudo) == 0) {
            exists = true;
            break;
          }
        }
        if (!exists && strlen(setupJoueurs[i].pseudo) > 0) {
          Rectangle createBtn = {pseudoRect.x + pseudoRect.width + 10,
                                 pseudoRect.y, 80, 30};
          if (DrawButton(createBtn, "Créer")) {
            // Create user logic -> getOrCreateUser does it.
            editingPlayer = -1;
          }
        }
      }

      // BACKGROUND FOR LIST (Fix for "mélange les pseudos")
      if (editingPlayer == i) {
        // Draw a background behind the list to hide underlying UI
        float listY = pseudoRect.y + pseudoRect.height;
        // Calculate total height of list
        float listHeight = matchCount * 25.0f;
        if (listHeight > 0) {
          DrawRectangle(pseudoRect.x, listY, pseudoRect.width + 100,
                        listHeight + 10, SKYBLUE);
        }

        for (int m = 0; m < matchCount; m++) {
          Rectangle matchRect = {pseudoRect.x, listY + (m * 25),
                                 pseudoRect.width, 25};
          DrawRectangleRec(matchRect, SKYBLUE);
          DrawRectangleLinesEx(matchRect, 1, DARKBLUE);
          DrawText(matches[m], matchRect.x + 5, matchRect.y + 5, 15, BLACK);

          if (CheckCollisionPointRec(GetMousePosition(), matchRect) &&
              IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            strcpy(setupJoueurs[i].pseudo, matches[m]);
            editingPlayer = -1; // Stop editing
          }
        }
      }

      // Couleur (Classic logic updated label)
      DrawText(TextFormat("Couleur J%d:", i + 1), startX + 20, 170, 18,
               DARKGRAY);

      // Map valid skinIDs to Colors correctly
      // Ensure skinID stays in 0-3 range
      if (setupJoueurs[i].skinID < 0)
        setupJoueurs[i].skinID = 3;
      if (setupJoueurs[i].skinID > 3)
        setupJoueurs[i].skinID = 0;

      Color skinColor;
      switch (setupJoueurs[i].skinID) {
      case 0:
        skinColor = BLUE;
        break;
      case 1:
        skinColor = RED;
        break;
      case 2:
        skinColor = GREEN;
        break;
      case 3:
        skinColor = ORANGE;
        break;
      default:
        skinColor = BLACK;
        break;
      }

      DrawCircle(startX + sectorWidth / 2, 220, 25, skinColor);

      // Flèches pour changer de skin (Logique basique)
      if (DrawButton((Rectangle){startX + sectorWidth / 2 - 60, 210, 20, 20},
                     "<"))
        setupJoueurs[i].skinID--;
      if (DrawButton((Rectangle){startX + sectorWidth / 2 + 40, 210, 20, 20},
                     ">"))
        setupJoueurs[i].skinID++;
    }

    DisplayMap(maps, mapCount, &mapIndex); // Use new display function

    // 4. BOUTON LANCER (En bas) - Moved down for spacing
    float launchBtnWidth = GetScreenWidth() * 0.25;
    if (launchBtnWidth < 200)
      launchBtnWidth = 200;
    float launchBtnHeight = GetScreenHeight() * 0.08;
    if (launchBtnHeight < 50)
      launchBtnHeight = 50;

    if (DrawButton((Rectangle){(GetScreenWidth() - launchBtnWidth) / 2,
                               GetScreenHeight() * 0.9, launchBtnWidth,
                               launchBtnHeight},
                   "LANCER")) {
      // If valid map
      if (mapIndex >= 0 && mapIndex < mapCount) {
        strcpy(selectedMapPath, maps[mapIndex].path);
        *finalPlayerCount = totalPlayers;
        mapSelected = true;
      }
    }

    EndDrawing();
  }

  // Cleanup
  if (allUsernames) {
    for (int i = 0; i < userCount; i++)
      free(allUsernames[i]);
    free(allUsernames);
  }
  free(matches);
  // UnloadTexture(setupBackground); // Removed

  // Cleanup Maps
  for (int i = 0; i < mapCount; i++) {
    if (maps[i].hasTexture)
      UnloadTexture(maps[i].texture);
  }
  free(maps);

  UnloadDirectoryFiles(files);
}
