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

// --- THEME COLORS ---
#define COL_BG_DARK (Color){30, 30, 30, 255}
#define COL_PANEL (Color){45, 45, 48, 255}
#define COL_ACCENT (Color){0, 122, 204, 255} // VS Code Blue
#define COL_ACCENT_HOVER (Color){28, 151, 234, 255}
#define COL_TEXT_WHITE (Color){240, 240, 240, 255}
#define COL_TEXT_GRAY (Color){160, 160, 160, 255}
#define COL_DANGER (Color){220, 50, 50, 255}

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
bool showOverwriteConfirmation = false;
bool showDeleteConfirmation = false;
char mapToDelete[64] = {0};

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
void ForceSaveMap(void);
void SaveMap(void);
void DeleteMap(const char *filename);
void ExportMapImage(void);
void DrawUI(void);
void DrawOverlay(void);
void DrawSetupUI(void);
void DrawMap(void);
void UpdateMapTile(void);
void ResizeMap(int dW, int dH);
bool GuiButton(Rectangle bounds, const char *text);
bool GuiTextBox(Rectangle bounds, char *text, int textSize, bool editMode);

// =========================================================================

void RunEditor(void) {
  SetWindowTitle("Map Editor");

  // --- FIX CRASH: Reset Global State on Entry ---
  // --- FIX CRASH: Reset Global State on Entry ---
  isSetupMode = false;
  InitializeMapData(25, 16); // Default Size
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
      if (!showOverwriteConfirmation && !showDeleteConfirmation) {
        if (IsKeyPressed(KEY_TAB)) {
          currentMode =
              (currentMode == MODE_VISUAL) ? MODE_COLLISION : MODE_VISUAL;
        }
        UpdateMapTile();
      }
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
      // Draw Layout Backgrounds
      DrawRectangle(0, 0, uiWidth, sh, COL_PANEL);              // Left Panel
      DrawRectangle(uiWidth, 0, sw - uiWidth, sh, COL_BG_DARK); // Map Area

      DrawUI();
      DrawMap();

      // Map Border
      DrawRectangleLinesEx(mapDrawArea, 2, BLACK);

      DrawText("BACKSPACE: Return  |  TAB: Switch Mode", uiWidth + 20, sh - 30,
               20, COL_TEXT_GRAY);
      DrawOverlay();
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

void ForceSaveMap(void) {
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
  ExportMapImage();
  LoadMapList(); // Refresh list
  showOverwriteConfirmation = false;
}

void SaveMap(void) {
  char path[256];
  sprintf(path, "%s/%s.txt", MAPS_DIR, currentMapName);

  if (FileExists(path)) {
    showOverwriteConfirmation = true;
  } else {
    ForceSaveMap();
  }
}

void DeleteMap(const char *filename) {
  char pathTxt[256];
  char pathPng[256];

  // Construct paths
  sprintf(pathTxt, "%s/%s.txt", MAPS_DIR, filename);
  sprintf(pathPng, "%s/%s.png", MAPS_DIR, filename);

  // Delete files
  if (FileExists(pathTxt))
    remove(pathTxt);
  if (FileExists(pathPng))
    remove(pathPng);

  // Refresh
  LoadMapList();
  selectedFileIndex = -1; // Deselect
  showDeleteConfirmation = false;
  memset(mapToDelete, 0, 64);
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
  int x = 20;
  int y = 20;
  int w = 360; // Panel width is ~400, leaving margin

  DrawText("MAP EDITOR", x, y, 30, COL_TEXT_WHITE);
  y += 50;

  // --- TABS ---
  int tabW = (w - 10) / 2;

  // Custom Tab Buttons
  if (GuiButton((Rectangle){(float)x, (float)y, (float)tabW, 40}, "VISUAL"))
    currentMode = MODE_VISUAL;
  if (currentMode == MODE_VISUAL)
    DrawRectangle((float)x, (float)y + 35, (float)tabW, 5, COL_TEXT_WHITE);

  if (GuiButton((Rectangle){(float)x + tabW + 10, (float)y, (float)tabW, 40},
                "COLLISION"))
    currentMode = MODE_COLLISION;
  if (currentMode == MODE_COLLISION)
    DrawRectangle((float)x + tabW + 10, (float)y + 35, (float)tabW, 5,
                  COL_TEXT_WHITE);

  y += 60;

  // --- FILE LIST ---
  DrawText("SAVED MAPS", x, y, 20, COL_TEXT_GRAY);
  y += 30;

  // Scrollable area background (simulation)
  DrawRectangle(x, y, w, 200, (Color){30, 30, 30, 100});
  int listY = y + 5;

  for (int i = 0; i < (int)mapFiles.count; i++) {
    if (GetFileExtension(mapFiles.paths[i]) &&
        strcmp(GetFileExtension(mapFiles.paths[i]), ".txt") == 0) {

      bool isSelected = (selectedFileIndex == i);
      Rectangle rowRect = {(float)x, (float)listY, (float)w, 30};

      // Selection Highlight
      if (isSelected)
        DrawRectangleRec(rowRect, (Color){60, 60, 60, 255});

      if (CheckCollisionPointRec(GetMousePosition(), rowRect)) {
        DrawRectangleRec(rowRect, (Color){80, 80, 80, 50}); // Hover
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          if (!CheckCollisionPointRec(
                  GetMousePosition(),
                  (Rectangle){(float)x + w - 30, (float)listY, 30, 30})) {
            selectedFileIndex = i;
            LoadMap(GetFileName(mapFiles.paths[i]));
            isSetupMode = false;
          }
        }
      }

      DrawText(GetFileName(mapFiles.paths[i]), x + 10, listY + 8, 20,
               COL_TEXT_WHITE);

      // Delete Button (Align Right)
      Rectangle delRect = {(float)x + w - 30, (float)listY + 5, 20, 20};
      if (CheckCollisionPointRec(GetMousePosition(), delRect)) {
        DrawRectangleRec(delRect, COL_DANGER);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          strncpy(mapToDelete, GetFileName(mapFiles.paths[i]), 63);
          char *dot = strrchr(mapToDelete, '.');
          if (dot)
            *dot = '\0';
          showDeleteConfirmation = true;
        }
      } else {
        DrawRectangleLinesEx(delRect, 1, COL_DANGER);
      }
      DrawText("X", delRect.x + 6, delRect.y + 2, 10, WHITE);

      listY += 30;
      if (listY > y + 200)
        break; // Clip
    }
  }
  y += 220;

  // --- NAME & SAVE ---
  DrawText("NAME", x, y, 20, COL_TEXT_GRAY);
  if (GuiTextBox((Rectangle){(float)x + 60, (float)y - 5, (float)w - 60, 35},
                 currentMapName, 64, isRenaming)) {
    isRenaming = !isRenaming;
  }
  y += 45;

  if (GuiButton((Rectangle){(float)x, (float)y, (float)w, 40},
                "SAVE CURRENT MAP")) {
    SaveMap();
  }
  y += 60;

  // --- RESIZE CONTROLS ---
  DrawText("DIMENSIONS", x, y, 20, COL_TEXT_GRAY);
  y += 30;

  // Width Control
  DrawText(TextFormat("W: %02d", currentMap.width), x, y + 10, 20,
           COL_TEXT_WHITE);
  if (GuiButton((Rectangle){(float)x + 70, (float)y, 30, 30}, "-"))
    ResizeMap(-1, 0);
  if (GuiButton((Rectangle){(float)x + 105, (float)y, 30, 30}, "+"))
    ResizeMap(1, 0);

  // Height Control
  DrawText(TextFormat("H: %02d", currentMap.height), x + 160, y + 10, 20,
           COL_TEXT_WHITE);
  if (GuiButton((Rectangle){(float)x + 230, (float)y, 30, 30}, "-"))
    ResizeMap(0, -1);
  if (GuiButton((Rectangle){(float)x + 265, (float)y, 30, 30}, "+"))
    ResizeMap(0, 1);
  y += 50;

  // --- UNDERLAY SELECTOR ---
  DrawText("DESTRUCTION REPLACE", x, y, 20, COL_TEXT_GRAY);
  y += 30;
  // Find current underlay texture
  Texture2D underlayTex = palette[0].texture;
  for (int i = 0; i < MAX_BLOCK_TYPES; i++)
    if (palette[i].id == currentMap.underlayId) {
      underlayTex = palette[i].texture;
      break;
    }

  Rectangle rSetup = {(float)x, (float)y, 40, 40};
  DrawTexturePro(
      underlayTex,
      (Rectangle){0, 0, (float)underlayTex.width, (float)underlayTex.height},
      rSetup, (Vector2){0, 0}, 0, WHITE);

  if (isChoosingUnderlay) {
    DrawRectangleLinesEx(rSetup, 3, COL_ACCENT);
    DrawText("CLICK TILE BELOW...", x + 50, y + 12, 10, COL_ACCENT);
  } else {
    DrawRectangleLinesEx(rSetup, 1, COL_TEXT_GRAY);
    if (CheckCollisionPointRec(GetMousePosition(), rSetup) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      isChoosingUnderlay = !isChoosingUnderlay;
    }
  }
  y += 60;

  // --- PALETTE / TOOLS ---
  if (currentMode == MODE_VISUAL) {
    DrawText("PALETTE", x, y, 20, COL_TEXT_GRAY);
    y += 30;
    int px = x;
    int py = y;

    for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
      if (palette[i].id == -1)
        continue;

      Rectangle pRect = {(float)px, (float)py, 40, 40};
      DrawTexturePro(palette[i].texture,
                     (Rectangle){0, 0, (float)palette[i].texture.width,
                                 (float)palette[i].texture.height},
                     pRect, (Vector2){0, 0}, 0, WHITE);

      if (selectedBlockId == palette[i].id && !isChoosingUnderlay)
        DrawRectangleLinesEx(pRect, 3, COL_ACCENT);
      else if (CheckCollisionPointRec(GetMousePosition(), pRect))
        DrawRectangleLinesEx(pRect, 1, WHITE); // Hover

      if (CheckCollisionPointRec(GetMousePosition(), pRect) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (isChoosingUnderlay) {
          currentMap.underlayId = palette[i].id;
          isChoosingUnderlay = false;
        } else {
          selectedBlockId = palette[i].id;
        }
      }
      px += 45;
      if (px > x + w - 40) {
        px = x;
        py += 45;
      }
    }
  } else {
    DrawText("COLLISION TYPES", x, y, 20, COL_TEXT_GRAY);
    y += 30;

    const char *names[] = {"WALL", "DESTR", "SPAWN"};
    Color colors[] = {RED, GREEN, BLUE};
    int ids[] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
      Rectangle btn = {(float)x, (float)y, 100.0f, 40.0f};
      bool active = (selectedCollisionType == ids[i]);

      if (active)
        DrawRectangleRec(btn, Fade(colors[i], 0.3f));
      DrawRectangleLinesEx(btn, 2, colors[i]);
      DrawText(names[i], btn.x + 10, btn.y + 10, 20, colors[i]);

      if (CheckCollisionPointRec(GetMousePosition(), btn) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedCollisionType = ids[i];
      }
      x += 110;
    }
    DrawText("RIGHT CLICK TO ERASE", 20, y + 50, 20, COL_TEXT_GRAY);
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

void DrawOverlay(void) {
  if (showOverwriteConfirmation) {
    // Draw Modal
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  (Color){0, 0, 0, 100});
    int boxW = 300, boxH = 150;
    int boxX = (GetScreenWidth() - boxW) / 2;
    int boxY = (GetScreenHeight() - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, RAYWHITE);
    DrawRectangleLines(boxX, boxY, boxW, boxH, BLACK);

    DrawText("OVERWRITE MAP?", boxX + 60, boxY + 30, 20, RED);
    DrawText(currentMapName, boxX + (300 - MeasureText(currentMapName, 20)) / 2,
             boxY + 60, 20, DARKGRAY);

    if (GuiButton((Rectangle){(float)boxX + 20, (float)boxY + 100, 100, 30},
                  "YES")) {
      ForceSaveMap();
    }
    if (GuiButton((Rectangle){(float)boxX + 180, (float)boxY + 100, 100, 30},
                  "NO")) {
      showOverwriteConfirmation = false;
    }
  }

  if (showDeleteConfirmation) {
    // Draw Modal
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  (Color){0, 0, 0, 100});
    int boxW = 300, boxH = 150;
    int boxX = (GetScreenWidth() - boxW) / 2;
    int boxY = (GetScreenHeight() - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, RAYWHITE);
    DrawRectangleLines(boxX, boxY, boxW, boxH, BLACK);

    DrawText("DELETE MAP?", boxX + 80, boxY + 30, 20, RED);
    DrawText(mapToDelete, boxX + (300 - MeasureText(mapToDelete, 20)) / 2,
             boxY + 60, 20, DARKGRAY);

    if (GuiButton((Rectangle){(float)boxX + 20, (float)boxY + 100, 100, 30},
                  "YES")) {
      DeleteMap(mapToDelete);
    }
    if (GuiButton((Rectangle){(float)boxX + 180, (float)boxY + 100, 100, 30},
                  "NO")) {
      showDeleteConfirmation = false;
    }
  }
}