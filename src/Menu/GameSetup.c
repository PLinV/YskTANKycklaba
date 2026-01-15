#include "GameSetup.h"
#include "../dataCode/historicalGame.h"
#include "../dataCode/user.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char pseudo[32];
  int skinID; // 0=BLUE, 1=RED, 2=GREEN, 3=ORANGE
  Rectangle bounds;
  bool isEditing;
} PlayerConfig;

// Structure to hold pre-loaded map info
typedef struct {
  char name[128];
  char path[512];
  Texture2D texture;
  bool hasTexture;
} MapUI;

// Initialize defaults
PlayerConfig setupJoueurs[4] = {{"J1", 0, {0}, false},
                                {"J2", 0, {0}, false},
                                {"J3", 0, {0}, false},
                                {"J4", 0, {0}, false}};
int totalPlayers = 2; // Par défaut

// --- FONCTION BOUTON IMAGE (Inspirée de menu.c) ---
static bool DrawImageButton(Rectangle bounds, Texture2D texture) {
  bool pressed = false;
  Vector2 mousePoint = GetMousePosition();
  bool isHover = CheckCollisionPointRec(mousePoint, bounds);

  Color tint = isHover ? LIGHTGRAY : WHITE;

  if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    pressed = true;
  }

  DrawTexturePro(texture,
                 (Rectangle){0, 0, (float)texture.width, (float)texture.height},
                 bounds, (Vector2){0, 0}, 0.0f, tint);

  return pressed;
}

// Custom text button for smaller UI elements
static bool DrawStyledButton(Rectangle bounds, const char *text, bool active) {
  bool pressed = false;
  Vector2 mousePoint = GetMousePosition();

  bool isHover = CheckCollisionPointRec(mousePoint, bounds);

  Color baseColor = active ? GOLD : Fade(BLACK, 0.5f);
  if (isHover && !active)
    baseColor = Fade(GOLD, 0.5f);

  DrawRectangleRec(bounds, baseColor);
  DrawRectangleLinesEx(bounds, 2, active ? ORANGE : DARKGRAY);

  int fontSize = 20;
  int textWidth = MeasureText(text, fontSize);
  Color textColor = active ? BLACK : WHITE;

  DrawText(text, bounds.x + (bounds.width - textWidth) / 2,
           bounds.y + (bounds.height - fontSize) / 2, fontSize, textColor);

  if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    pressed = true;
  }
  return pressed;
}

// 3D Cyclic Carousel Logic
void DisplayMap(MapUI *maps, int count, int *mapIndex) {
  if (count == 0)
    return;

  // Navigation (Cyclic)
  bool nextMap = IsKeyPressed(KEY_RIGHT);
  bool prevMap = IsKeyPressed(KEY_LEFT);
  // Center Position for Carousel
  float centerX = GetScreenWidth() / 2;
  float centerY =
      GetScreenHeight() * 0.72; // Slightly higher to leave room for button

  int current = *mapIndex;
  int prev = (current - 1 + count) % count;
  int next = (current + 1) % count;

  float size = 180; // Fixed size for consistency
  float x = centerX - size / 2;
  float y = centerY - size / 2;

  // CURRENT
  int aimTextWidth = MeasureText(maps[current].name, 24);
  DrawText(maps[current].name, centerX - aimTextWidth / 2, y - 40, 24, GOLD);

  // Border for current map
  DrawRectangle(x - 5, y - 5, size + 10, size + 10, GOLD);
  DrawRectangle(x, y, size, size, BLACK);

  if (maps[current].hasTexture) {
    DrawTexturePro(maps[current].texture,
                   (Rectangle){0, 0, maps[current].texture.width,
                               maps[current].texture.height},
                   (Rectangle){x, y, size, size}, (Vector2){0, 0}, 0.0f, WHITE);
  } else {
    DrawText("No Image", x + size * 0.3, y + size / 2, 20, GRAY);
  }
  // --- BOUTONS FLÈCHES (INTERACTIFS) ---
  Vector2 mousePoint = GetMousePosition();
  // 1. BOUTON GAUCHE "<"
  // On définit une zone cliquable autour du texte (un peu plus large pour
  // faciliter le clic)
  Rectangle btnLeftRect = {x - 70, y + size / 2 - 25, 50, 50};
  Color colorLeft = WHITE;

  if (CheckCollisionPointRec(mousePoint, btnLeftRect)) {
    colorLeft = GOLD; // Change de couleur au survol
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      prevMap = true; // Active le changement
    }
  }
  DrawText("<", x - 60, y + size / 2 - 20, 40, colorLeft);

  // 2. BOUTON DROIT ">"
  // On définit une zone cliquable
  Rectangle btnRightRect = {x + size + 30, y + size / 2 - 25, 50, 50};
  Color colorRight = WHITE;

  if (CheckCollisionPointRec(mousePoint, btnRightRect)) {
    colorRight = GOLD; // Change de couleur au survol
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      nextMap = true; // Active le changement
    }
  }
  DrawText(">", x + size + 40, y + size / 2 - 20, 40, colorRight);
  // --- APPLICATION DU CHANGEMENT ---
  // On applique le changement d'index ici, qu'il vienne du clavier ou de la
  // souris
  if (nextMap)
    *mapIndex = (*mapIndex + 1) % count;
  if (prevMap)
    *mapIndex = (*mapIndex - 1 + count) % count;

  // PREVIOUS (Smaller and darker)
  if (count > 1) {
    float sideSize = size * 0.7;
    float gap = 60;
    float sideX = x - gap - sideSize;
    float sideY = centerY - sideSize / 2;

    DrawRectangle(sideX, sideY, sideSize, sideSize, Fade(BLACK, 0.5f));
    if (maps[prev].hasTexture) {
      DrawTexturePro(maps[prev].texture,
                     (Rectangle){0, 0, maps[prev].texture.width,
                                 maps[prev].texture.height},
                     (Rectangle){sideX, sideY, sideSize, sideSize},
                     (Vector2){0, 0}, 0.0f, Fade(WHITE, 0.5f));
    }
  }

  // NEXT (Smaller and darker)
  if (count > 1) {
    float sideSize = size * 0.7;
    float gap = 60;
    float sideX = x + size + gap;
    float sideY = centerY - sideSize / 2;

    DrawRectangle(sideX, sideY, sideSize, sideSize, Fade(BLACK, 0.5f));
    if (maps[next].hasTexture) {
      DrawTexturePro(maps[next].texture,
                     (Rectangle){0, 0, maps[next].texture.width,
                                 maps[next].texture.height},
                     (Rectangle){sideX, sideY, sideSize, sideSize},
                     (Vector2){0, 0}, 0.0f, Fade(WHITE, 0.5f));
    }
  }
}

// --- FONCTION PRINCIPALE SETUP ---
void RunGameSetup(char *selectedMapPath, int *finalPlayerCount, User *users[]) {

  SetWindowTitle("YskTANKycklaba - Game Setup");

  // Init flag to force score refresh on entry
  bool forceScoreUpdate = true;

  // Init
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();
  (void)screenWidth;
  (void)screenHeight;
  Music musicMenu = LoadMusicStream("src/Menu/musique/musiqueMenuStart.mp3");
  bool musiqueLoad;
  // 3. LANCER LA LECTURE
  if (musicMenu.frameCount > 0) {
    PlayMusicStream(musicMenu);
    musiqueLoad = true;
    SetMusicVolume(musicMenu, 0.5f);
  } else {
    musiqueLoad = false;
  }

  // --- CHARGEMENT DES RESSOURCES ---
  Texture2D background = LoadTexture("src/Menu/images/background_start.jpg");
  Texture2D btnStartImg = LoadTexture("src/Menu/images/bouton_jouer.jpg");

  FilePathList files = LoadDirectoryFiles("src/Graphisme/Maps");

  int editingPlayer = -1;

  // Autocomplete Data
  char **allUsernames = NULL;
  int userCount = GetAllUsers(&allUsernames);
  int maxMatches = 10;
  char **matches = malloc(maxMatches * sizeof(char *));
  int matchCount = 0;

  int mapIndex = 0;
  bool mapSelected = false;

  // Load Maps
  int mapCount = 0;
  for (int i = 0; i < (int)files.count; i++) {
    if (IsFileExtension(files.paths[i], ".txt"))
      mapCount++;
  }

  MapUI *maps = malloc(mapCount * sizeof(MapUI));
  int currentIdx = 0;
  for (int i = 0; i < (int)files.count; i++) {
    if (IsFileExtension(files.paths[i], ".txt")) {
      strcpy(maps[currentIdx].path, files.paths[i]);
      strcpy(maps[currentIdx].name, GetFileNameWithoutExt(files.paths[i]));

      char imgPath[512];
      strcpy(imgPath, files.paths[i]);
      char *ext = strrchr(imgPath, '.');
      if (ext)
        strcpy(ext, ".png");

      if (FileExists(imgPath)) {
        maps[currentIdx].texture = LoadTexture(imgPath);
        maps[currentIdx].hasTexture = true;
      } else {
        maps[currentIdx].hasTexture = false;
      }
      currentIdx++;
    }
  }

  while (!WindowShouldClose() && !mapSelected) {

    if (musiqueLoad) {
      UpdateMusicStream(musicMenu);
    }

    // --- INPUT GESTION ---
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
      if (editingPlayer != -1) {
        int len = strlen(setupJoueurs[editingPlayer].pseudo);
        if (len > 0)
          setupJoueurs[editingPlayer].pseudo[len - 1] = '\0';
      } else {
        selectedMapPath[0] = '\0';
        break;
      }
    }

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

      // Update autocomplete
      matchCount = 0;
      char *current = setupJoueurs[editingPlayer].pseudo;
      int currentLen = strlen(current);
      if (currentLen > 0) {
        for (int u = 0; u < userCount && matchCount < maxMatches; u++) {
          if (strncmp(allUsernames[u], current, currentLen) == 0)
            matches[matchCount++] = allUsernames[u];
        }
      } else {
        for (int u = 0; u < userCount && matchCount < maxMatches; u++)
          matches[matchCount++] = allUsernames[u];
      }
    } else {
      matchCount = 0;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && editingPlayer != -1) {
      // Logic to close edit mode is handled in drawing if not clicked on
      // interaction
    }

    // --- DESSIN ---
    BeginDrawing();
    ClearBackground(BLACK);

    // 1. BACKGROUND
    DrawTexturePro(background,
                   (Rectangle){0, 0, background.width, background.height},
                   (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                   (Vector2){0, 0}, 0.0f, WHITE);

    // Overlay sombre pour lisibilité
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

    DrawText("CONFIGURATION DE LA PARTIE", 20, 20, 40, GOLD);

    // 2. NOMBRE DE JOUEURS
    int startY = 80;
    const char *nbPlayersText = "NOMBRE DE JOUEURS :";
    int textWidth = MeasureText(nbPlayersText, 24);
    int buttonsTotalWidth = 3 * 60; // 3 buttons * 60px stride
    int totalBlockWidth = textWidth + 20 + buttonsTotalWidth;

    int blockStartX = (GetScreenWidth() - totalBlockWidth) / 2;

    DrawText(nbPlayersText, blockStartX, startY, 24, WHITE);

    for (int i = 2; i <= 4; i++) {
      Rectangle r = {blockStartX + textWidth + 20 + (i - 2) * 60, startY - 5,
                     50, 40};
      if (DrawStyledButton(r, TextFormat("%d", i), (totalPlayers == i))) {
        totalPlayers = i;
      }
    }

    // 3. ZONE CONFIGURATION JOUEURS
    // Panel semi-transparent
    int panelY = 140;
    int panelHeight = 240;
    DrawRectangle(0, panelY, GetScreenWidth(), panelHeight, Fade(BLACK, 0.6f));
    DrawLine(0, panelY, GetScreenWidth(), panelY, GOLD);
    DrawLine(0, panelY + panelHeight, GetScreenWidth(), panelY + panelHeight,
             GOLD);

    float sectorWidth = (float)GetScreenWidth() / totalPlayers;

    // Track if we clicked anywhere in UI this frame
    bool clickedInUI = false;

    for (int i = 0; i < totalPlayers; i++) {
      float startX = i * sectorWidth;
      float centerX = startX + sectorWidth / 2;

      // Séparateur vertical
      if (i > 0)
        DrawLine(startX, panelY + 10, startX, panelY + panelHeight - 10, GRAY);

      // Titre JOUEUR
      DrawText(TextFormat("JOUEUR %d", i + 1),
               centerX - MeasureText(TextFormat("JOUEUR %d", i + 1), 20) / 2,
               panelY + 20, 20, GOLD);

      // Saisie Pseudo
      Rectangle pseudoRect = {centerX - 80, panelY + 50, 160, 30};

      bool isHover = CheckCollisionPointRec(GetMousePosition(), pseudoRect);
      if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        editingPlayer = i;
        clickedInUI = true;
      }

      Color boxColor = (editingPlayer == i) ? WHITE : LIGHTGRAY;
      DrawRectangleRec(pseudoRect, boxColor);
      DrawRectangleLinesEx(pseudoRect, 2,
                           (editingPlayer == i) ? GOLD : DARKGRAY);

      DrawText(setupJoueurs[i].pseudo, pseudoRect.x + 5, pseudoRect.y + 5, 20,
               BLACK);
      if (setupJoueurs[i].pseudo[0] == '\0' && editingPlayer != i) {
        DrawText("Pseudo...", pseudoRect.x + 5, pseudoRect.y + 5, 20,
                 Fade(GRAY, 0.8f));
      }

      // Autocomplete list drawing (Overlay)
      if (editingPlayer == i) {
        float listY = pseudoRect.y + pseudoRect.height;
        float listHeight = matchCount * 30.0f;
        if (listHeight > 0) {
          DrawRectangle(pseudoRect.x, listY, pseudoRect.width + 120,
                        listHeight + 10, Fade(BLACK, 0.9f));
          DrawRectangleLines(pseudoRect.x, listY, pseudoRect.width + 120,
                             listHeight + 10, GOLD);
        }

        for (int m = 0; m < matchCount; m++) {
          Rectangle matchRect = {pseudoRect.x, listY + m * 30,
                                 pseudoRect.width + 120, 30};
          bool hoverMatch =
              CheckCollisionPointRec(GetMousePosition(), matchRect);
          if (hoverMatch)
            DrawRectangleRec(matchRect, Fade(GOLD, 0.3f));

          DrawText(matches[m], matchRect.x + 5, matchRect.y + 5, 20, WHITE);

          if (hoverMatch && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            strcpy(setupJoueurs[i].pseudo, matches[m]);
            editingPlayer = -1;
            clickedInUI = true;
          }
        }

        // Check existing for Create logic
        bool exists = false;
        for (int u = 0; u < userCount; u++) {
          if (strcmp(allUsernames[u], setupJoueurs[i].pseudo) == 0) {
            exists = true;
            break;
          }
        }
        if (!exists && strlen(setupJoueurs[i].pseudo) > 0) {
          Rectangle createBtn = {pseudoRect.x + pseudoRect.width + 5,
                                 pseudoRect.y, 60, 30};
          if (DrawStyledButton(createBtn, "Créer", false)) {

            User *currentUser = createUser(setupJoueurs[i].pseudo);
            saveUser(currentUser);
            editingPlayer = -1;
            clickedInUI = true;
          }
        }
      }

      // 1v1 score
      if (totalPlayers == 2 && setupJoueurs[0].pseudo[0] != '\0' &&
          setupJoueurs[1].pseudo[0] != '\0') {

        // Optimization: Check if pseudos changed before heavy I/O
        static char lastP1[32] = "";
        static char lastP2[32] = "";
        static int cachedJ1score = 0;
        static int cachedJ2score = 0;
        // removed static firstRun

        if (forceScoreUpdate || strcmp(setupJoueurs[0].pseudo, lastP1) != 0 ||
            strcmp(setupJoueurs[1].pseudo, lastP2) != 0) {
          User *u1 = getUser(setupJoueurs[0].pseudo);
          User *u2 = getUser(setupJoueurs[1].pseudo);
          if (u1 && u2) {
            J1vsJ2Score(u1->username, u2->username, &cachedJ1score,
                        &cachedJ2score);
            free(u1);
            free(u2);
          } else {
            cachedJ1score = 0;
            cachedJ2score = 0;
          }
          strcpy(lastP1, setupJoueurs[0].pseudo);
          strcpy(lastP2, setupJoueurs[1].pseudo);
          forceScoreUpdate = false;
        }

        const char *scoreText =
            TextFormat(": %d vs %d :", cachedJ1score, cachedJ2score);

        // Update UI
        int fontSize = 40;
        int padding = 20;
        int textW = MeasureText(scoreText, fontSize);
        float rectW = textW + padding * 2;
        float rectH = fontSize + padding;
        float screenCenter = GetScreenWidth() / 2.0f;
        float rectX = screenCenter - rectW / 2.0f;
        float rectY = 140; // Aligned with start of player panels

        // Draw top-aligned bridge/connector between panels style
        DrawRectangle(rectX, rectY, rectW, rectH, Fade(BLACK, 0.8f));
        DrawRectangleLines(rectX, rectY, rectW, rectH, GOLD);
        DrawText(scoreText, rectX + padding, rectY + padding / 2, fontSize,
                 GOLD);
      }

      // Sélection Skin
      DrawText("SKIN", centerX - MeasureText("SKIN", 18) / 2, panelY + 100, 18,
               LIGHTGRAY);

      // Color definition
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

      DrawCircle(centerX, panelY + 150, 30, skinColor);
      DrawCircleLines(centerX, panelY + 150, 32, GOLD);

      // Flèches
      Rectangle leftArrow = {centerX - 60, panelY + 140, 30, 30};
      Rectangle rightArrow = {centerX + 30, panelY + 140, 30, 30};

      if (DrawStyledButton(leftArrow, "<", false)) {
        setupJoueurs[i].skinID--;
        clickedInUI = true;
      }
      if (DrawStyledButton(rightArrow, ">", false)) {
        setupJoueurs[i].skinID++;
        clickedInUI = true;
      }
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !clickedInUI &&
        editingPlayer != -1) {
      editingPlayer = -1;
    }

    // 4. DISPLAY MAP CAROUSEL
    DisplayMap(maps, mapCount, &mapIndex);

    // 5. START BUTTON
    int btnW = 300;
    int btnH = 80;
    Rectangle startBtnRect = {(GetScreenWidth() - btnW) / 2,
                              GetScreenHeight() - 100, btnW, btnH};

    if (DrawImageButton(startBtnRect, btnStartImg)) {
      if (mapIndex >= 0 && mapIndex < mapCount) {
        strcpy(selectedMapPath, maps[mapIndex].path);
        *finalPlayerCount = totalPlayers;
        mapSelected = true; // Va permettre de finir le while de RunGameSetup
                            // pour faire ensuite le RunGame
        for (int i = 0; i < *finalPlayerCount; i++) {
          users[i] = getUser(setupJoueurs[i].pseudo);
          if (users[i]) {
            users[i]->currentSkinID = setupJoueurs[i].skinID;
          }
          printf("\n ----%d---- \n", users[i]->gamePlayed);
        }
        // Cleanup remaining slots to avoid stale data
        for (int i = *finalPlayerCount; i < 4; i++) {
          users[i] = NULL;
        }
      }
    }

    EndDrawing();
  }

  // --- CLEANUP ---
  if (allUsernames) {
    for (int i = 0; i < userCount; i++)
      free(allUsernames[i]);
    free(allUsernames);
  }
  free(matches);

  for (int i = 0; i < mapCount; i++) {
    if (maps[i].hasTexture)
      UnloadTexture(maps[i].texture);
  }
  free(maps);

  UnloadTexture(background);
  UnloadTexture(btnStartImg);

  UnloadDirectoryFiles(files);
}
