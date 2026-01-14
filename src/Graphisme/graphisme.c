#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- DÉFINITIONS GLOBALES ---
#define TILE_DEFAULT_SIZE 32
#define PALETTE_ITEM_SIZE 64
#define MAX_BLOCK_TYPES 12
#define MAX_MAPS_IN_LIST 20
#define MAPS_DIR "src/Graphisme/Maps"
#define ASSETS_BLOC_DIR "src/Graphisme/assets/texture_bloc"

// --- ENUMS ---
typedef enum { MODE_VISUAL, MODE_COLLISION } EditorMode;

// --- STRUCTURES ---
typedef struct {
  int id;
  char name[32];
  Color color;
  Texture2D texture;
} BlockDefinition;

typedef struct {
  int width;
  int height;
  int *tileMap;      // IDs for visual map
  int *collisionMap; // 0: Empty, 1: Wall, 2: Destructible Wall, 3: Spawn
  int underlayId;    // Default replacement texture ID
} MapData;

// --- VARIABLES GLOBALES ---
BlockDefinition palette[MAX_BLOCK_TYPES];
int selectedBlockId = 0;
int selectedCollisionType = 1; // Default to Wall
MapData currentMap;
Rectangle mapDrawArea;
EditorMode currentMode = MODE_VISUAL;
bool isChoosingUnderlay = false;

// File Management
FilePathList mapFiles;
int selectedFileIndex = -1;
char currentMapName[64] = "default";
bool isRenaming = false;
char renameBuffer[64] = {0};

// Setup Mode
bool isSetupMode = true;
char widthBuffer[16] = "25";
char heightBuffer[16] = "16";
bool editingWidth = false;
bool editingHeight = false;

// --- PROTOTYPES ---
void InitializeMapData(int width, int height);
void InitializePalette(void);
void LoadMapList(void);
void LoadMap(const char *filename);
void SaveMap(void);
void ExportMapImage(void);
void DrawUI(void);
void DrawSetupUI(void);
void DrawMap(void);
void UpdateMapTile(void);
void ResizeMap(int dW, int dH);
bool GuiButton(Rectangle bounds, const char *text);
bool GuiTextBox(Rectangle bounds, char *text, int textSize, bool editMode);

// =========================================================================

void RunEditor(void) {
  SetWindowTitle("Raylib 2D Map Editor - Advanced");

  // --- FIX CRASH: Reset Global State on Entry ---
  isSetupMode = true;
  currentMode = MODE_VISUAL;
  editingWidth = false;
  editingHeight = false;
  strcpy(widthBuffer, "25");
  strcpy(heightBuffer, "16");
  strcpy(currentMapName, "default");
  isRenaming = false;
  selectedBlockId = 0;
  selectedCollisionType = 1;
  // ----------------------------------------------

  InitializePalette();
  // Don't initialize map data yet, wait for setup
  LoadMapList();

  bool exitEditor = false;
  while (!WindowShouldClose() && !exitEditor) {
    // Fix Renaming Backspace: Only exit if NOT renaming/editing text
    if (IsKeyPressed(KEY_BACKSPACE) && !isRenaming && !editingWidth &&
        !editingHeight) {
      if (isSetupMode)
        exitEditor = true;
      else {
        exitEditor = true;
      }
    }

    if (!isSetupMode) {
      if (IsKeyPressed(KEY_TAB)) {
        currentMode =
            (currentMode == MODE_VISUAL) ? MODE_COLLISION : MODE_VISUAL;
      }
      UpdateMapTile();
    }

    // Reset isChoosingUnderlay if mode changes
    if (currentMode == MODE_COLLISION)
      isChoosingUnderlay = false;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int uiWidth = 400;
    mapDrawArea = (Rectangle){(float)uiWidth, 50, (float)(sw - uiWidth - 50),
                              (float)(sh - 100)};

    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (isSetupMode) {
      DrawSetupUI();
    } else {
      DrawUI();
      DrawMap();
      DrawRectangleLines(mapDrawArea.x, mapDrawArea.y, mapDrawArea.width,
                         mapDrawArea.height, DARKGRAY);
      DrawText("Press BACKSPACE to return | TAB to switch mode", 10, sh - 30,
               20, DARKGRAY);
    }

    EndDrawing();
  }

  // Cleanup
  if (currentMap.tileMap)
    free(currentMap.tileMap);
  currentMap.tileMap = NULL;
  if (currentMap.collisionMap)
    free(currentMap.collisionMap);
  currentMap.collisionMap = NULL;

  for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
    UnloadTexture(palette[i].texture);
    palette[i].texture.id = 0; // Prevent double-unload or invalid access
  }
  UnloadDirectoryFiles(mapFiles);
  memset(&mapFiles, 0, sizeof(FilePathList));
}

void DrawSetupUI(void) {
  int sw = GetScreenWidth();
  int sh = GetScreenHeight();
  int cx = sw / 2;
  int cy = sh / 2;

  DrawText("NEW MAP SETUP", cx - 100, cy - 150, 30, DARKGRAY);

  DrawText("Width:", cx - 100, cy - 80, 20, GRAY);
  if (GuiTextBox((Rectangle){(float)cx, (float)cy - 85, 100, 30}, widthBuffer,
                 16, editingWidth)) {
    editingWidth = !editingWidth;
    editingHeight = false;
  }

  DrawText("Height:", cx - 100, cy - 40, 20, GRAY);
  if (GuiTextBox((Rectangle){(float)cx, (float)cy - 45, 100, 30}, heightBuffer,
                 16, editingHeight)) {
    editingHeight = !editingHeight;
    editingWidth = false;
  }

  if (GuiButton((Rectangle){(float)cx - 50, (float)cy + 20, 100, 40},
                "CREATE")) {
    int w = atoi(widthBuffer);
    int h = atoi(heightBuffer);
    if (w > 0 && h > 0) {
      InitializeMapData(w, h);
      isSetupMode = false;
    }
  }

  DrawText("OR Load Existing Map:", cx - 100, cy + 80, 20, GRAY);

  // List existing maps
  int y = cy + 110;
  for (int i = 0; i < (int)mapFiles.count; i++) {
    if (GetFileExtension(mapFiles.paths[i]) &&
        strcmp(GetFileExtension(mapFiles.paths[i]), ".txt") == 0) {
      Rectangle rect = {(float)cx - 100, (float)y, 200, 20};
      if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        DrawRectangleRec(rect, LIGHTGRAY);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          LoadMap(GetFileName(mapFiles.paths[i]));
          isSetupMode = false;
        }
      }
      DrawText(GetFileName(mapFiles.paths[i]), cx - 90, y + 2, 10, BLACK);
      y += 20;
    }
  }
}

// =========================================================================

void InitializeMapData(int width, int height) {
  if (currentMap.tileMap)
    free(currentMap.tileMap);
  if (currentMap.collisionMap)
    free(currentMap.collisionMap);

  currentMap.width = width;
  currentMap.height = height;
  int size = width * height;

  currentMap.tileMap = (int *)calloc(size, sizeof(int));
  currentMap.collisionMap = (int *)calloc(size, sizeof(int));

  // --- DEFAULT BORDERS ---
  // Set walls on edges
  for (int x = 0; x < width; x++) {
    // Top row
    currentMap.collisionMap[x] = 1; // Wall
    currentMap.tileMap[x] = 2;      // Mur Brique (Visual ID 2)
    // Bottom row
    int bottomIdx = (height - 1) * width + x;
    currentMap.collisionMap[bottomIdx] = 1;
    currentMap.tileMap[bottomIdx] = 2;
  }
  for (int y = 0; y < height; y++) {
    // Left column
    currentMap.collisionMap[y * width] = 1;
    currentMap.tileMap[y * width] = 2;
    // Right column
    int rightIdx = y * width + (width - 1);
    currentMap.collisionMap[rightIdx] = 1;
    currentMap.tileMap[rightIdx] = 2;
  }
}

void InitializePalette(void) {
  // Clear palette first
  for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
    palette[i].id = -1;
    palette[i].texture.id = 0;
  }

  // Load from directory
  if (!DirectoryExists(ASSETS_BLOC_DIR))
    return;

  FilePathList files = LoadDirectoryFiles(ASSETS_BLOC_DIR);
  for (int i = 0; i < (int)files.count; i++) {
    const char *fname = GetFileName(files.paths[i]);
    int id = -1;
    char nameBuffer[64] = {0};

    // Parse "X_name.ext"
    // Find first underscore
    const char *underscore = strchr(fname, '_');
    if (underscore) {
      // Parse ID
      char idStr[16] = {0};
      strncpy(idStr, fname, underscore - fname);
      id = atoi(idStr);

      // Parse Name (remove extension)
      strcpy(nameBuffer, underscore + 1);
      char *dot = strrchr(nameBuffer, '.');
      if (dot)
        *dot = '\0';
    }

    if (id >= 0 && id < MAX_BLOCK_TYPES) {
      palette[id].id = id;
      strncpy(palette[id].name, nameBuffer, 31);
      palette[id].texture = LoadTexture(files.paths[i]);
      palette[id].color = WHITE;
    }
  }
  UnloadDirectoryFiles(files);

  // Ensure we have at least defaults if empty (Fallback)
  if (palette[0].texture.id == 0) {
    Image img = GenImageColor(32, 32, GREEN);
    palette[0] =
        (BlockDefinition){0, "Default", GREEN, LoadTextureFromImage(img)};
    UnloadImage(img);
  }
}

void LoadMapList(void) {
  if (mapFiles.paths != NULL) {
    UnloadDirectoryFiles(mapFiles);
  }
  mapFiles = LoadDirectoryFiles(MAPS_DIR);
}

void LoadMap(const char *filename) {
  char path[256];
  sprintf(path, "%s/%s", MAPS_DIR, filename);

  char *text = LoadFileText(path);
  if (!text)
    return;

  // Replace all separators with spaces to simplify parsing
  for (int i = 0; text[i] != '\0'; i++) {
    if (text[i] == ',' || text[i] == '\n' || text[i] == '\r') {
      text[i] = ' ';
    }
  }

  int w, h, nbSpawn, underlayId;
  char *token = strtok(text, " ");

  if (!token) {
    UnloadFileText(text);
    return;
  }
  w = atoi(token);

  token = strtok(NULL, " ");
  if (!token) {
    UnloadFileText(text);
    return;
  }
  h = atoi(token);

  token = strtok(NULL, " ");
  if (!token) {
    UnloadFileText(text);
    return;
  }
  nbSpawn = atoi(token);

  token = strtok(NULL, " ");
  if (!token) {
    UnloadFileText(text);
    return;
  }
  underlayId = atoi(token);

  InitializeMapData(w, h);
  currentMap.underlayId = underlayId;

  // Read Collision Matrix
  for (int i = 0; i < w * h; i++) {
    token = strtok(NULL, " ");
    if (token) {
      int val = atoi(token);
      currentMap.collisionMap[i] = val;

      // Default visual if not provided later (backward compatibility)
      if (val == 0)
        currentMap.tileMap[i] = 0;
      else if (val == 1)
        currentMap.tileMap[i] = 2;
      else if (val == 2)
        currentMap.tileMap[i] = 2; // Default Box for Destr
      else if (val == 3)
        currentMap.tileMap[i] = 0; // Ground for Spawn
      // Note: visual map should be loaded from _vis file usually.
    } else {
      currentMap.collisionMap[i] = 0;
    }
  }

  // Read Visual Matrix (if available)
  for (int i = 0; i < w * h; i++) {
    token = strtok(NULL, " ");
    if (token) {
      int val = atoi(token);
      currentMap.tileMap[i] = val;
    }
    // If no more tokens, we just keep the defaults set above
  }

  UnloadFileText(text);

  // Update name without extension
  strncpy(currentMapName, filename, 63);
  char *dot = strrchr(currentMapName, '.');
  if (dot)
    *dot = '\0';
}

void SaveMap(void) {
  char path[256];
  sprintf(path, "%s/%s.txt", MAPS_DIR, currentMapName);

  FILE *file = fopen(path, "w");
  if (!file)
    return;

  // Count Spawns
  // Count Spawns
  int nbSpawn = 0;
  for (int i = 0; i < currentMap.width * currentMap.height; i++) {
    if (currentMap.collisionMap[i] == 3)
      nbSpawn++;
  }

  fprintf(file, "%d,%d,%d,%d\n", currentMap.width, currentMap.height, nbSpawn,
          currentMap.underlayId);

  // Write Collision Matrix
  for (int y = 0; y < currentMap.height; y++) {
    for (int x = 0; x < currentMap.width; x++) {
      int val = currentMap.collisionMap[y * currentMap.width + x];
      fprintf(file, "%d%s", val, (x < currentMap.width - 1) ? "," : "");
    }
    fprintf(file, "\n");
  }

  // Write Visual Matrix
  for (int y = 0; y < currentMap.height; y++) {
    for (int x = 0; x < currentMap.width; x++) {
      int val = currentMap.tileMap[y * currentMap.width + x];
      fprintf(file, "%d%s", val, (x < currentMap.width - 1) ? "," : "");
    }
    fprintf(file, "\n");
  }

  fclose(file);

  ExportMapImage();
  LoadMapList(); // Refresh list
}

void ExportMapImage(void) {
  int w = currentMap.width * TILE_DEFAULT_SIZE;
  int h = currentMap.height * TILE_DEFAULT_SIZE;

  RenderTexture2D target = LoadRenderTexture(w, h);
  BeginTextureMode(target);
  ClearBackground(RAYWHITE);

  for (int y = 0; y < currentMap.height; y++) {
    for (int x = 0; x < currentMap.width; x++) {
      int index = y * currentMap.width + x;
      int id = currentMap.tileMap[index];
      // Find texture
      Texture2D tex = palette[0].texture;
      for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
        if (palette[i].id == id) {
          tex = palette[i].texture;
          break;
        }
      }
      DrawTexture(tex, x * TILE_DEFAULT_SIZE, y * TILE_DEFAULT_SIZE, WHITE);
    }
  }
  EndTextureMode();

  Image img = LoadImageFromTexture(target.texture);
  ImageFlipVertical(&img); // OpenGL texture is flipped

  char path[256];
  sprintf(path, "%s/%s.png", MAPS_DIR, currentMapName);
  ExportImage(img, path);

  UnloadImage(img);
  UnloadRenderTexture(target);
}

void ResizeMap(int dW, int dH) {
  int newW = currentMap.width + dW;
  int newH = currentMap.height + dH;
  if (newW < 1 || newH < 1)
    return;

  int *newTileMap = (int *)calloc(newW * newH, sizeof(int));
  int *newColMap = (int *)calloc(newW * newH, sizeof(int));

  // Copy existing data
  for (int y = 0; y < newH; y++) {
    for (int x = 0; x < newW; x++) {
      int newIndex = y * newW + x;
      if (x < currentMap.width && y < currentMap.height) {
        int oldIndex = y * currentMap.width + x;
        newTileMap[newIndex] = currentMap.tileMap[oldIndex];
        newColMap[newIndex] = currentMap.collisionMap[oldIndex];
      }
    }
  }

  // Smart Border Logic (Only when expanding)
  if (dW > 0) {
    // 1. Clear old right border (excluding corners to preserve top/bottom
    // walls)
    int oldRightX = currentMap.width - 1;
    for (int y = 1; y < currentMap.height - 1; y++) {
      int idx = y * newW + oldRightX;
      newColMap[idx] = 0;  // Empty
      newTileMap[idx] = 0; // Grass
    }
    // 2. Set new right border to Wall
    int newRightX = newW - 1;
    for (int y = 0; y < newH; y++) {
      int idx = y * newW + newRightX;
      newColMap[idx] = 1;  // Wall
      newTileMap[idx] = 2; // Brick
    }
  }

  if (dH > 0) {
    // 1. Clear old bottom border (excluding corners)
    int oldBottomY = currentMap.height - 1;
    for (int x = 1; x < currentMap.width - 1; x++) {
      int idx = oldBottomY * newW + x;
      newColMap[idx] = 0;
      newTileMap[idx] = 0;
    }
    // 2. Set new bottom border to Wall
    int newBottomY = newH - 1;
    for (int x = 0; x < newW; x++) {
      int idx = newBottomY * newW + x;
      newColMap[idx] = 1;
      newTileMap[idx] = 2;
    }
  }

  free(currentMap.tileMap);
  free(currentMap.collisionMap);
  currentMap.width = newW;
  currentMap.height = newH;
  currentMap.tileMap = newTileMap;
  currentMap.collisionMap = newColMap;
}

void DrawUI(void) {
  int y = 10;
  DrawText("MAP EDITOR", 10, y, 20, DARKGRAY);
  y += 30;

  // --- TABS ---
  int tabW = 150;
  if (GuiButton((Rectangle){10, (float)y, (float)tabW, 30},
                currentMode == MODE_VISUAL ? "[ VISUAL ]" : "VISUAL")) {
    currentMode = MODE_VISUAL;
  }
  if (GuiButton((Rectangle){(float)10 + tabW + 10, (float)y, (float)tabW, 30},
                currentMode == MODE_COLLISION ? "[ COLLISION ]"
                                              : "COLLISION")) {
    currentMode = MODE_COLLISION;
  }
  y += 40;

  // --- FILE LIST ---
  DrawText("Saved Maps:", 10, y, 10, GRAY);
  y += 15;
  for (int i = 0; i < (int)mapFiles.count; i++) {
    if (GetFileExtension(mapFiles.paths[i]) &&
        strcmp(GetFileExtension(mapFiles.paths[i]), ".txt") == 0) {
      Rectangle rect = {10, (float)y, 200, 20};
      bool isSelected = (selectedFileIndex == i);

      if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        DrawRectangleRec(rect, LIGHTGRAY);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          selectedFileIndex = i;
          LoadMap(GetFileName(mapFiles.paths[i]));
          isSetupMode = false;
        }
      }

      DrawText(GetFileName(mapFiles.paths[i]), 15, y + 2, 10,
               isSelected ? BLUE : BLACK);
      y += 20;
    }
  }
  y += 10;

  // --- NAME & SAVE ---
  DrawText("Map Name:", 10, y, 10, GRAY);
  y += 15;
  if (GuiTextBox((Rectangle){10, (float)y, 200, 30}, currentMapName, 64,
                 isRenaming)) {
    isRenaming = !isRenaming;
  }
  y += 35;

  if (GuiButton((Rectangle){10, (float)y, 100, 30}, "SAVE ALL")) {
    SaveMap();
  }
  y += 40;

  // --- RESIZE CONTROLS ---
  DrawText("Resize Map:", 10, y, 10, GRAY);
  y += 15;
  DrawText(TextFormat("W: %d", currentMap.width), 10, y + 5, 10, BLACK);
  if (GuiButton((Rectangle){60, (float)y, 20, 20}, "-"))
    ResizeMap(-1, 0);
  if (GuiButton((Rectangle){85, (float)y, 20, 20}, "+"))
    ResizeMap(1, 0);

  DrawText(TextFormat("H: %d", currentMap.height), 120, y + 5, 10, BLACK);
  if (GuiButton((Rectangle){170, (float)y, 20, 20}, "-"))
    ResizeMap(0, -1);
  if (GuiButton((Rectangle){195, (float)y, 20, 20}, "+"))
    ResizeMap(0, 1);
  y += 30;

  // --- UNDERLAY SELECTOR (REPLACER) ---
  DrawText("Destructible Replace Texture:", 10, y, 10, GRAY);
  y += 20;

  // Find current underlay texture
  Texture2D underlayTex = palette[0].texture; // Default
  for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
    if (palette[i].id == currentMap.underlayId) {
      underlayTex = palette[i].texture;
      break;
    }
  }

  Rectangle rSetup = {10, (float)y, 50, 50};
  DrawTexturePro(
      underlayTex,
      (Rectangle){0, 0, (float)underlayTex.width, (float)underlayTex.height},
      rSetup, (Vector2){0, 0}, 0, WHITE);

  if (isChoosingUnderlay) {
    DrawRectangleLinesEx(rSetup, 3, GREEN);
    DrawText("SELECT BELOW...", 70, y + 20, 10, GREEN);
  } else {
    DrawRectangleLinesEx(rSetup, 1, DARKGRAY);
    if (CheckCollisionPointRec(GetMousePosition(), rSetup) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      isChoosingUnderlay = !isChoosingUnderlay;
    }
  }
  y += 60;

  // --- PALETTE / COLLISION TOOLS ---
  if (currentMode == MODE_VISUAL) {
    DrawText("Visual Palette:", 10, y, 10, GRAY);
    y += 15;
    int px = 10;
    int py = y;

    for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
      if (palette[i].id == -1)
        continue; // Skip empty

      Rectangle pRect = {(float)px, (float)py, 40, 40};
      DrawTexturePro(palette[i].texture,
                     (Rectangle){0, 0, (float)palette[i].texture.width,
                                 (float)palette[i].texture.height},
                     pRect, (Vector2){0, 0}, 0, WHITE);

      if (selectedBlockId == palette[i].id && !isChoosingUnderlay)
        DrawRectangleLinesEx(pRect, 2, RED);

      // Logic for selection or underlay choice
      if (CheckCollisionPointRec(GetMousePosition(), pRect) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (isChoosingUnderlay) {
          currentMap.underlayId = palette[i].id;
          isChoosingUnderlay = false; // Auto-exit selection mode
        } else {
          selectedBlockId = palette[i].id;
        }
      }

      px += 45;
      if (px > 350) {
        px = 10;
        py += 45;
      }
    }
  } else {
    DrawText("Collision Types:", 10, y, 10, GRAY);
    y += 15;

    // Wall Button
    Rectangle rWall = {10, (float)y, 80, 30};
    if (selectedCollisionType == 1)
      DrawRectangleRec(rWall, LIGHTGRAY);
    DrawRectangleLinesEx(rWall, 1, RED);
    DrawText("WALL", 30, y + 10, 10, RED);
    if (CheckCollisionPointRec(GetMousePosition(), rWall) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      selectedCollisionType = 1;

    // Destructible Button
    Rectangle rDest = {100, (float)y, 80, 30};
    if (selectedCollisionType == 2)
      DrawRectangleRec(rDest, LIGHTGRAY);
    DrawRectangleLinesEx(rDest, 1, GREEN);
    DrawText("DESTRUCT", 110, y + 10, 10, GREEN);
    if (CheckCollisionPointRec(GetMousePosition(), rDest) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      selectedCollisionType = 2;

    // Spawn Button
    Rectangle rSpawn = {190, (float)y, 80, 30};
    if (selectedCollisionType == 3)
      DrawRectangleRec(rSpawn, LIGHTGRAY);
    DrawRectangleLinesEx(rSpawn, 1, BLUE);
    DrawText("SPAWN", 200, y + 10, 10, BLUE);
    if (CheckCollisionPointRec(GetMousePosition(), rSpawn) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      selectedCollisionType = 3;

    y += 40;
    DrawText("Right Click to Clear", 10, y, 10, DARKGRAY);
  }
}

void DrawMap(void) {
  float tileW = mapDrawArea.width / currentMap.width;
  float tileH = mapDrawArea.height / currentMap.height;

  for (int y = 0; y < currentMap.height; y++) {
    for (int x = 0; x < currentMap.width; x++) {
      int index = y * currentMap.width + x;
      Rectangle dest = {mapDrawArea.x + x * tileW, mapDrawArea.y + y * tileH,
                        tileW, tileH};

      // Draw Visual Layer
      int id = currentMap.tileMap[index];
      Texture2D tex = palette[0].texture; // Default
      for (int i = 0; i < MAX_BLOCK_TYPES; i++)
        if (palette[i].id == id)
          tex = palette[i].texture;

      DrawTexturePro(tex, (Rectangle){0, 0, 32, 32}, dest, (Vector2){0, 0}, 0,
                     WHITE);

      // Draw Collision Layer Overlay
      if (currentMode == MODE_COLLISION) {
        int colType = currentMap.collisionMap[index];
        if (colType == 1) { // Wall
          DrawRectangleRec(dest, (Color){255, 0, 0, 150});
          DrawText("W", dest.x + 5, dest.y + 5, 10, WHITE);
        } else if (colType == 2) { // Destructible
          DrawRectangleRec(dest, (Color){0, 255, 0, 150});
          DrawText("D", dest.x + 5, dest.y + 5, 10, WHITE);
        } else if (colType == 3) { // Spawn
          DrawRectangleRec(dest, (Color){0, 0, 255, 150});
          DrawText("S", dest.x + 5, dest.y + 5, 10, WHITE);
        } else {
          DrawRectangleRec(dest, (Color){0, 255, 0, 50}); // Empty
        }
      }

      DrawRectangleLinesEx(dest, 1, (Color){200, 200, 200, 100});
    }
  }
}

void UpdateMapTile(void) {
  static bool isPaintingCollision = false;
  static int paintingType = 0;

  if (CheckCollisionPointRec(GetMousePosition(), mapDrawArea)) {
    float tileW = mapDrawArea.width / currentMap.width;
    float tileH = mapDrawArea.height / currentMap.height;
    int x = (GetMousePosition().x - mapDrawArea.x) / tileW;
    int y = (GetMousePosition().y - mapDrawArea.y) / tileH;

    if (x >= 0 && x < currentMap.width && y >= 0 && y < currentMap.height) {
      int index = y * currentMap.width + x;

      if (currentMode == MODE_VISUAL) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
          if (!isChoosingUnderlay) {
            currentMap.tileMap[index] = selectedBlockId;
          }
        }
      } else {
        // Collision Mode
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          isPaintingCollision = true;
          paintingType = selectedCollisionType;
          currentMap.collisionMap[index] = paintingType;
        } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
                   isPaintingCollision) {
          currentMap.collisionMap[index] = paintingType;
        }

        // Right click to clear
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
          currentMap.collisionMap[index] = 0;
        }
      }
    }
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    isPaintingCollision = false;
  }
}

// Simple GUI Helpers
bool GuiButton(Rectangle bounds, const char *text) {
  bool clicked = false;
  bool hover = CheckCollisionPointRec(GetMousePosition(), bounds);
  if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    clicked = true;
  DrawRectangleRec(bounds, hover ? LIGHTGRAY : RAYWHITE);
  DrawRectangleLinesEx(bounds, 1, DARKGRAY);
  DrawText(text, bounds.x + 10, bounds.y + 8, 10, BLACK);
  return clicked;
}

bool GuiTextBox(Rectangle bounds, char *text, int textSize, bool editMode) {
  DrawRectangleRec(bounds, editMode ? LIGHTGRAY : RAYWHITE);
  DrawRectangleLinesEx(bounds, 1, editMode ? RED : DARKGRAY);
  DrawText(text, bounds.x + 5, bounds.y + 8, 20, BLACK);

  if (editMode) {
    int key = GetCharPressed();
    while (key > 0) {
      int len = strlen(text);
      if (len < textSize - 1) {
        text[len] = (char)key;
        text[len + 1] = '\0';
      }
      key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
      int len = strlen(text);
      if (len > 0)
        text[len - 1] = '\0';
    }
    if (IsKeyPressed(KEY_ENTER))
      return true;
  }

  if (CheckCollisionPointRec(GetMousePosition(), bounds) &&
      IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    return true;
  }
  return false;
}

void UpdateMapDimensions(int newWidth, int newHeight) {
  InitializeMapData(newWidth, newHeight);
}
void ExportCollisionMap(const char *filename) {} // Unused, replaced by SaveMap