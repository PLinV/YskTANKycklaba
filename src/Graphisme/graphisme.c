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
#define COL_ACCENT (Color){255, 165, 0, 255} // Orange
#define COL_ACCENT_HOVER (Color){255, 200, 0, 255} // Yellow-Orange Hover
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

  // Modern Background Panel
  DrawRectangleRounded((Rectangle){cx - 200, cy - 200, 400, 400}, 0.1f, 10, COL_PANEL);
  DrawRectangleRoundedLines((Rectangle){cx - 200, cy - 200, 400, 400}, 0.1f, 10, COL_ACCENT);

  DrawText("NEW MAP SETUP", cx - 90, cy - 170, 26, COL_TEXT_WHITE);

  // Width Input
  DrawText("Width:", cx - 150, cy - 100, 20, COL_TEXT_GRAY);
  Rectangle rWidth = {cx - 50, cy - 105, 120, 35};
  DrawRectangleRounded(rWidth, 0.2f, 5, editingWidth ? COL_ACCENT : DARKGRAY);
  // Inner box
  DrawRectangleRounded((Rectangle){rWidth.x+2, rWidth.y+2, rWidth.width-4, rWidth.height-4}, 0.2f, 5, (Color){30,30,30,255});
  
  if (GuiTextBox(rWidth, widthBuffer, 16, editingWidth)) {
    editingWidth = !editingWidth;
    editingHeight = false;
  }

  // Height Input
  DrawText("Height:", cx - 150, cy - 50, 20, COL_TEXT_GRAY);
  Rectangle rHeight = {cx - 50, cy - 55, 120, 35};
  DrawRectangleRounded(rHeight, 0.2f, 5, editingHeight ? COL_ACCENT : DARKGRAY);
  DrawRectangleRounded((Rectangle){rHeight.x+2, rHeight.y+2, rHeight.width-4, rHeight.height-4}, 0.2f, 5, (Color){30,30,30,255});

  if (GuiTextBox(rHeight, heightBuffer, 16, editingHeight)) {
    editingHeight = !editingHeight;
    editingWidth = false;
  }

  // Create Button
  Rectangle btnCreate = {(float)cx - 60, (float)cy + 20, 120, 40};
  DrawRectangleRounded(btnCreate, 0.3f, 6, CheckCollisionPointRec(GetMousePosition(), btnCreate) ? COL_ACCENT_HOVER : COL_ACCENT);
  if (CheckCollisionPointRec(GetMousePosition(), btnCreate) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    int w = atoi(widthBuffer);
    int h = atoi(heightBuffer);
    if (w > 0 && h > 0) {
      InitializeMapData(w, h);
      isSetupMode = false;
    }
  }
  int txtW = MeasureText("CREATE", 20);
  DrawText("CREATE", btnCreate.x + (btnCreate.width-txtW)/2, btnCreate.y + 10, 20, WHITE);

  DrawText("OR Load Existing Map:", cx - 150, cy + 90, 20, COL_TEXT_GRAY);

  // List existing maps - Compact
  int y = cy + 120;
  for (int i = 0; i < (int)mapFiles.count; i++) {
    if (GetFileExtension(mapFiles.paths[i]) &&
        strcmp(GetFileExtension(mapFiles.paths[i]), ".txt") == 0) {
      Rectangle rect = {(float)cx - 150, (float)y, 300, 24};
      bool hover = CheckCollisionPointRec(GetMousePosition(), rect);
      
      DrawRectangleRounded(rect, 0.2f, 4, hover ? (Color){60,60,60,255} : (Color){40,40,40,255});
      
      if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          LoadMap(GetFileName(mapFiles.paths[i]));
          isSetupMode = false;
      }
      DrawText(GetFileName(mapFiles.paths[i]), rect.x + 10, rect.y + 4, 16, COL_TEXT_WHITE);
      y += 28;
      if (y > cy + 180) break; 
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

  DrawText("MAP EDITOR", x, y, 32, COL_ACCENT);
  DrawRectangle(x, y + 36, w, 2, COL_ACCENT); 
  y += 50;

  // --- TABS ---
  int tabW = (w - 10) / 2;

  // Custom Tab Buttons
  Color colVis = (currentMode == MODE_VISUAL) ? COL_ACCENT : COL_PANEL;
  Color colCol = (currentMode == MODE_COLLISION) ? COL_ACCENT : COL_PANEL;
  
  Rectangle rVis = {(float)x, (float)y, (float)tabW, 40};
  DrawRectangleRounded(rVis, 0.3f, 6, colVis);
  DrawRectangleRoundedLines(rVis, 0.3f, 6, COL_TEXT_GRAY);
  DrawText("VISUEL", rVis.x + tabW/2 - MeasureText("VISUAL", 20)/2, rVis.y + 10, 20, (currentMode == MODE_VISUAL) ? WHITE : GRAY);
  if (CheckCollisionPointRec(GetMousePosition(), rVis) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      currentMode = MODE_VISUAL;

  Rectangle rCol = {(float)x + tabW + 10, (float)y, (float)tabW, 40};
  DrawRectangleRounded(rCol, 0.3f, 6, colCol);
  DrawRectangleRoundedLines(rCol, 0.3f, 6, COL_TEXT_GRAY);
  DrawText("COLLISION", rCol.x + tabW/2 - MeasureText("COLLISION", 20)/2, rCol.y + 10, 20, (currentMode == MODE_COLLISION) ? WHITE : GRAY);
  if (CheckCollisionPointRec(GetMousePosition(), rCol) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      currentMode = MODE_COLLISION;

  y += 60;

  // --- FILE LIST ---
  DrawText("MAPS ENREGISTRES", x, y, 18, COL_TEXT_GRAY);
  y += 25;

  // Scrollable container look
  DrawRectangleRounded((Rectangle){x, y, w, 200}, 0.1f, 8, (Color){20, 20, 20, 150});
  int listY = y + 5;

  for (int i = 0; i < (int)mapFiles.count; i++) {
    if (GetFileExtension(mapFiles.paths[i]) &&
        strcmp(GetFileExtension(mapFiles.paths[i]), ".txt") == 0) {

      bool isSelected = (selectedFileIndex == i);
      Rectangle rowRect = {(float)x + 5, (float)listY, (float)w - 10, 30};

      Color bgRow = isSelected ? COL_ACCENT : (Color){0,0,0,0};
      if (CheckCollisionPointRec(GetMousePosition(), rowRect)) {
        if (!isSelected) bgRow = (Color){50,50,50,200};
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          if (!CheckCollisionPointRec(
                  GetMousePosition(),
                  (Rectangle){(float)x + w - 35, (float)listY + 2, 26, 26})) {
            selectedFileIndex = i;
            LoadMap(GetFileName(mapFiles.paths[i]));
            isSetupMode = false;
          }
        }
      }
      
      DrawRectangleRounded(rowRect, 0.2f, 4, bgRow);
      DrawText(GetFileName(mapFiles.paths[i]), x + 15, listY + 7, 18, COL_TEXT_WHITE);

      // Delete Button (Align Right)
      Rectangle delRect = {(float)x + w - 35, (float)listY + 2, 26, 26};
      bool hoverDel = CheckCollisionPointRec(GetMousePosition(), delRect);
      DrawRectangleRounded(delRect, 0.3f, 4, hoverDel ? COL_DANGER : (Color){60,60,60,255});
      DrawText("X", delRect.x + 8, delRect.y + 4, 18, WHITE);
        
      if (hoverDel && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        strncpy(mapToDelete, GetFileName(mapFiles.paths[i]), 63);
        char *dot = strrchr(mapToDelete, '.');
        if (dot) *dot = '\0';
        showDeleteConfirmation = true;
      }

      listY += 32;
      if (listY > y + 190)
        break; // Clip
    }
  }
  y += 220;

  // --- NAME & SAVE ---
  DrawText("NOM", x, y, 18, COL_TEXT_GRAY);
  
  Rectangle rName = {(float)x + 70, (float)y - 5, (float)w - 70, 35};
  DrawRectangleRounded(rName, 0.2f, 5, isRenaming ? COL_ACCENT : (Color){20,20,20,255});
  DrawRectangleRounded((Rectangle){rName.x+2, rName.y+2, rName.width-4, rName.height-4}, 0.2f, 5, (Color){40,40,40,255});
  
  if (GuiTextBox(rName, currentMapName, 64, isRenaming)) {
    isRenaming = !isRenaming;
  }
  y += 45;

  Rectangle rSave = {(float)x, (float)y, (float)w, 40};
  bool hoverSave = CheckCollisionPointRec(GetMousePosition(), rSave);
  DrawRectangleRounded(rSave, 0.3f, 6, hoverSave ? COL_ACCENT_HOVER : COL_ACCENT);
  DrawText("SAUVEGARDER LA MAP", x + w/2 - MeasureText("SAUVEGARDER LA MAP", 20)/2, y + 10, 20, WHITE);
  
  if (hoverSave && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    SaveMap();
  }
  y += 60;

  // --- RESIZE CONTROLS ---
  DrawText("DIMENSIONS", x, y, 18, COL_TEXT_GRAY);
  y += 30;

  // Width Control
  DrawText(TextFormat("W: %02d", currentMap.width), x, y + 8, 20, COL_TEXT_WHITE);
  
  Rectangle btnWM = {(float)x + 80, (float)y, 30, 30};
  DrawRectangleRounded(btnWM, 0.3f, 4, CheckCollisionPointRec(GetMousePosition(), btnWM) ? GRAY : DARKGRAY);
  DrawText("-", btnWM.x+10, btnWM.y+2, 20, WHITE);
  if (CheckCollisionPointRec(GetMousePosition(), btnWM) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ResizeMap(-1, 0);

  Rectangle btnWP = {(float)x + 120, (float)y, 30, 30};
  DrawRectangleRounded(btnWP, 0.3f, 4, CheckCollisionPointRec(GetMousePosition(), btnWP) ? GRAY : DARKGRAY);
  DrawText("+", btnWP.x+9, btnWP.y+2, 20, WHITE);
  if (CheckCollisionPointRec(GetMousePosition(), btnWP) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ResizeMap(1, 0);

  // Height Control
  DrawText(TextFormat("H: %02d", currentMap.height), x + 180, y + 8, 20, COL_TEXT_WHITE);
  
  Rectangle btnHM = {(float)x + 250, (float)y, 30, 30};
  DrawRectangleRounded(btnHM, 0.3f, 4, CheckCollisionPointRec(GetMousePosition(), btnHM) ? GRAY : DARKGRAY);
  DrawText("-", btnHM.x+10, btnHM.y+2, 20, WHITE);
  if (CheckCollisionPointRec(GetMousePosition(), btnHM) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ResizeMap(0, -1);

  Rectangle btnHP = {(float)x + 290, (float)y, 30, 30};
  DrawRectangleRounded(btnHP, 0.3f, 4, CheckCollisionPointRec(GetMousePosition(), btnHP) ? GRAY : DARKGRAY);
  DrawText("+", btnHP.x+9, btnHP.y+2, 20, WHITE);
  if (CheckCollisionPointRec(GetMousePosition(), btnHP) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ResizeMap(0, 1);
  
  y += 50;

  // --- UNDERLAY SELECTOR ---
  DrawText("PIERRE D'EFFACAGE", x, y, 18, COL_TEXT_GRAY);
  y += 30;
  // Find current underlay texture
  Texture2D underlayTex = palette[0].texture;
  for (int i = 0; i < MAX_BLOCK_TYPES; i++)
    if (palette[i].id == currentMap.underlayId) {
      underlayTex = palette[i].texture;
      break;
    }

  Rectangle rSetup = {(float)x, (float)y, 48, 48};
  DrawTexturePro(
      underlayTex,
      (Rectangle){0, 0, (float)underlayTex.width, (float)underlayTex.height},
      rSetup, (Vector2){0, 0}, 0, WHITE);

  if (isChoosingUnderlay) {
    DrawRectangleRoundedLines(rSetup, 0.2f, 4, COL_ACCENT);
    DrawText("CLIQUEZ SUR LA PALETTE POUR CHOISIR...", x + 60, y + 15, 12, COL_ACCENT);
  } else {
    DrawRectangleRoundedLines(rSetup, 0.2f, 4, COL_TEXT_GRAY);
    if (CheckCollisionPointRec(GetMousePosition(), rSetup) &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      isChoosingUnderlay = !isChoosingUnderlay;
    }
    DrawText("(Pierre de remplacement)", x + 60, y + 15, 14, GRAY);
  }
  y += 60;

  // --- PALETTE / TOOLS ---
  if (currentMode == MODE_VISUAL) {
    DrawText("PALETTE", x, y, 18, COL_TEXT_GRAY);
    y += 30;
    int px = x;
    int py = y;

    for (int i = 0; i < MAX_BLOCK_TYPES; i++) {
      if (palette[i].id == -1)
        continue;

      Rectangle pRect = {(float)px, (float)py, 48, 48};
      DrawTexturePro(palette[i].texture,
                     (Rectangle){0, 0, (float)palette[i].texture.width,
                                 (float)palette[i].texture.height},
                     pRect, (Vector2){0, 0}, 0, WHITE);

      if (selectedBlockId == palette[i].id && !isChoosingUnderlay)
        DrawRectangleRoundedLines(pRect, 0.2f, 4, COL_ACCENT);
      else if (CheckCollisionPointRec(GetMousePosition(), pRect))
        DrawRectangleRoundedLines(pRect, 0.2f, 4, WHITE); // Hover

      if (CheckCollisionPointRec(GetMousePosition(), pRect) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (isChoosingUnderlay) {
          currentMap.underlayId = palette[i].id;
          isChoosingUnderlay = false;
        } else {
          selectedBlockId = palette[i].id;
        }
      }
      px += 55;
      if (px > x + w - 40) {
        px = x;
        py += 55;
      }
    }
  } else {
    DrawText("TYPES DE COLLISION", x, y, 18, COL_TEXT_GRAY);
    y += 30;

    const char *names[] = {"MUR", "DESTRUCTIBLE", "SPAWN"};
    Color colors[] = {RED, GREEN, BLUE}; // Keeping BLUE as user requested revert
    int ids[] = {1, 2, 3};

    for (int i = 0; i < 3; i++) {
      Rectangle btn = {(float)x, (float)y, 100.0f, 40.0f};
      bool active = (selectedCollisionType == ids[i]);

      DrawRectangleRounded(btn, 0.3f, 6, active ? Fade(colors[i], 0.3f) : (Color){0,0,0,0});
      DrawRectangleRoundedLines(btn, 0.3f, 6, colors[i]);
      DrawText(names[i], btn.x + 15, btn.y + 10, 18, colors[i]);

      if (CheckCollisionPointRec(GetMousePosition(), btn) &&
          IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedCollisionType = ids[i];
      }
      x += 110;
    }
    DrawText("CLIQUE DROIT POUR EFFACER", 20, y + 50, 20, COL_TEXT_GRAY);
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

// Helper to draw a modern popup button
bool DrawPopupButton(const char *text, Rectangle bounds) {
    bool pressed = false;
    bool hover = CheckCollisionPointRec(GetMousePosition(), bounds);
    
    Color btnColor = hover ? Fade(COL_ACCENT, 0.5f) : Fade(BLACK, 0.5f);
    
    DrawRectangleRec(bounds, btnColor);
    DrawRectangleLinesEx(bounds, 1, hover ? COL_ACCENT : Fade(COL_ACCENT, 0.3f));
    
    int txtW = MeasureText(text, 20);
    DrawText(text, bounds.x + (bounds.width - txtW)/2, bounds.y + (bounds.height - 20)/2, 20, WHITE);

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        pressed = true;
    }
    return pressed;
}

void DrawOverlay(void) {
  if (!showOverwriteConfirmation && !showDeleteConfirmation) return;

  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

  const char *title = NULL;
  const char *detail = NULL;
  
  if (showOverwriteConfirmation) {
      title = "LA MAP EXISTE DEJA";
      detail = currentMapName;
  } else {
      title = "SUPPRIMER LA MAP?";
      detail = mapToDelete;
  }

  // Popup Box
  float popupW = 400;
  float popupH = 220;
  float popupX = (GetScreenWidth() - popupW) / 2;
  float popupY = (GetScreenHeight() - popupH) / 2;
  Rectangle popupRect = {popupX, popupY, popupW, popupH};

  // Background
  DrawRectangleRec(popupRect, Fade(BLACK, 0.4f));
  DrawRectangleLinesEx(popupRect, 3, COL_ACCENT);

  // Titre
  if (title) {
      DrawText(title, popupX + (popupW - MeasureText(title, 26)) / 2, popupY + 30, 26, COL_ACCENT);
  }
  
  // Detail
  if (detail) {
      DrawText(detail, popupX + (popupW - MeasureText(detail, 20)) / 2, popupY + 80, 20, WHITE);
  }

  // Boutons
  float btnW = 120;
  float btnH = 40;
  float btnY = popupY + popupH - 60;
  float spacing = 40;
  float startX = popupX + (popupW - (2 * btnW + spacing)) / 2;

  if (DrawPopupButton("OUI", (Rectangle){startX, btnY, btnW, btnH})) {
      if (showOverwriteConfirmation) {
          ForceSaveMap();
      } else {
          DeleteMap(mapToDelete);
      }
  }

  if (DrawPopupButton("NON", (Rectangle){startX + btnW + spacing, btnY, btnW, btnH})) {
      showOverwriteConfirmation = false;
      showDeleteConfirmation = false;
  }
}