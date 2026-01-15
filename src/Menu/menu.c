#include "menu.h"
#include "../Credits/credits.h"
#include "../Game/Game.h"
#include "../Graphisme/graphisme.h"
#include "../dataCode/historicalGame.h"
#include "../dataCode/user.h"
#include "../statPage/statPage.h"
#include "GameOver.h"
#include "GameSetup.h"
#include "Settings.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- NOUVELLE FONCTION POUR LE BOUTON IMAGE ---
// enabled: si false, le bouton est dessiné (éventuellement grisé) mais pas
// cliquable.
bool DrawImageButton(Rectangle bounds, Texture2D texture, bool enabled) {
  bool pressed = false;
  Vector2 mousePoint = GetMousePosition();
  bool isHover = CheckCollisionPointRec(mousePoint, bounds);

  // Changement de teinte au survol pour donner un effet visuel
  // Si disable, on peut le griser davantage ou ne pas changer la teinte
  Color tint = enabled ? (isHover ? LIGHTGRAY : WHITE) : Fade(WHITE, 0.5f);

  if (enabled && isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    pressed = true;
  }

  // Effet d'agrandissement au survol
  Rectangle destRect = bounds;
  if (enabled && isHover) {
    float scale = 1.15f; // +5% de taille
    float newWidth = bounds.width * scale;
    float newHeight = bounds.height * scale;

    // Recentre le bouton agrandi
    destRect.x = bounds.x - (newWidth - bounds.width) / 2.0f;
    destRect.y = bounds.y - (newHeight - bounds.height) / 2.0f;
    destRect.width = newWidth;
    destRect.height = newHeight;
  }

  // On dessine l'image en l'étirant pour qu'elle remplisse le rectangle
  // "bounds" (ou destRect si scale)
  DrawTexturePro(texture,
                 (Rectangle){0, 0, (float)texture.width,
                             (float)texture.height}, // Source (toute l'image)
                 destRect, // Destination (taille du bouton définie dans le main
                           // ou agrandie)
                 (Vector2){0, 0}, 0.0f, tint);

  return pressed;
}

int main() {
  InitWindow(1000, 640, "YskTANKycklaba - Menu");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  User *users[4] = {NULL};

  // 1. INITIALISER LE PÉRIPHÉRIQUE AUDIO
  InitAudioDevice();
  // 2. CHARGER LA MUSIQUE
  Music musicMenu = LoadMusicStream("src/Menu/musique/musiqueMenu.mp3");
  bool musiqueLoad;
  // 3. LANCER LA LECTURE
  if (musicMenu.frameCount > 0) {
    printf("Musique chargée avec succès !\n");
    PlayMusicStream(musicMenu);
    musiqueLoad = true;
    SetMusicVolume(musicMenu, 0.5f);
  } else {
    printf("Echec du chargement de la musique !\n");
    musiqueLoad = false;
  }

  // --- Chargement de l'arrière-plan (GIF ou Statique) ---
  int animFrames = 0;
  Image imBackgroundAnim =
      LoadImageAnim("src/Menu/images/background_start_dyna.gif", &animFrames);
  Texture2D background;
  bool isAnim = false;
  int currentAnimFrame = 0;
  int frameCounter = 0;
  int animSpeed = 4; // Vitesse de l'animation

  if (imBackgroundAnim.data != NULL) {
    printf("Animation d'arrière-plan chargée ! Frames: %d\n", animFrames);
    background = LoadTextureFromImage(imBackgroundAnim);
    isAnim = true;
  } else {
    background = LoadTexture("src/Menu/images/background_start.jpg");
    if (background.id == 0) {
      printf("Echec du chargement de l'image d'arrière-plan.\n");
    }
  }

  Texture2D btnPlayImg = LoadTexture("src/Menu/images/bouton_jouer.jpg");
  if (btnPlayImg.id == 0) {
    printf("Failed to load image bouton jouer.\n");
  }

  Texture2D btnLeaveImg = LoadTexture("src/Menu/images/bouton_quitter.jpg");
  if (btnLeaveImg.id == 0) {
    printf("Failed to load image bouton quitter.\n");
  }

  Texture2D btnEditorImg = LoadTexture("src/Menu/images/bouton_editionmap.jpg");
  if (btnEditorImg.id == 0) {
    printf("Failed to load image bouton edition map.\n");
  }

  Texture2D btnStatsImg = LoadTexture("src/Menu/images/bouton_stats.jpg");
  if (btnStatsImg.id == 0) {
    printf("Failed to load image bouton stats.\n");
  }

  Texture2D btnCreditsImg = LoadTexture("src/Menu/images/bouton_credits.jpg");
  if (btnCreditsImg.id == 0) {
    printf("Failed to load image bouton credits.\n");
  }

  Texture2D btnSettingsImg =
      LoadTexture("src/Menu/images/bouton_paramètre.jpg");
  if (btnSettingsImg.id == 0) {
    printf("Failed to load image bouton parametre.\n");
  }

  bool isSettingsOpen = false;

  // --- 2. BOUCLE PRINCIPALE ---
  while (!WindowShouldClose()) {

    if (musiqueLoad) {
      UpdateMusicStream(musicMenu);
    }

    // Mise à jour de l'animation d'arrière-plan
    if (isAnim) {
      frameCounter++;
      if (frameCounter >= animSpeed) {
        currentAnimFrame++;
        if (currentAnimFrame >= animFrames)
          currentAnimFrame = 0;

        // Mise à jour vers la frame suivante
        unsigned int nextFrameDataOffset =
            (unsigned int)imBackgroundAnim.width * imBackgroundAnim.height * 4 *
            currentAnimFrame;
        UpdateTexture(background, ((unsigned char *)imBackgroundAnim.data) +
                                      nextFrameDataOffset);

        frameCounter = 0;
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTexturePro(
        background,
        (Rectangle){0, 0, (float)background.width,
                    (float)background.height}, // Source (toute l'image)
        (Rectangle){0, 0, (float)GetScreenWidth(),
                    (float)GetScreenHeight()}, // Destination (tout l'écran)
        (Vector2){0, 0}, 0.0f,
        WHITE // Teinte (WHITE = couleur originale)
    );

    int buttonWidth = GetScreenWidth() * 0.25; // 25% of screen width
    if (buttonWidth < 200)
      buttonWidth = 200;

    // Preserving Aspect Ratio for buttons (assuming all have similar ratio to
    // btnPlayImg)
    float aspectRatio = (float)btnPlayImg.height / (float)btnPlayImg.width;
    int buttonHeight = (int)(buttonWidth * aspectRatio);

    // Fallback if texture invalid or ratio extreme
    if (buttonHeight < 40)
      buttonHeight = 40;

    int spacing = GetScreenHeight() * 0.04; // 4% spacing

    int centerX = (GetScreenWidth() - buttonWidth) / 2;
    int startY = GetScreenHeight() * 0.35; // Start at 35% height

    // 1. Jouer
    if (DrawImageButton((Rectangle){centerX, startY, buttonWidth, buttonHeight},
                        btnPlayImg, !isSettingsOpen)) {
      char mapPath[512] = {0};
      int playerCount = 2;

      // Loop to allow returning to setup after game
      do {
        mapPath[0] =
            '\0'; // Reset map path ensures we stay unless start clicked
        RunGameSetup(mapPath, &playerCount, users);

        if (mapPath[0] != '\0') {
          int winner = RunGame(mapPath, playerCount, users);
          SetWindowTitle("YskTANKycklaba - Game");

          if (winner != -1) {
            RunGameOver(winner, users);
            SetWindowTitle("YskTANKycklaba - Menu");
          }
        }
      } while (mapPath[0] != '\0');
    }

    // 2. Statistiques
    if (DrawImageButton((Rectangle){centerX, startY + buttonHeight + spacing,
                                    buttonWidth, buttonHeight},
                        btnStatsImg, !isSettingsOpen)) {
      RunStats(musicMenu);
    }

    // 3. Map Editor
    if (DrawImageButton((Rectangle){centerX,
                                    startY + 2 * (buttonHeight + spacing),
                                    buttonWidth, buttonHeight},
                        btnEditorImg, !isSettingsOpen)) {
      RunEditor();
    }

    // 4. Quitter
    if (DrawImageButton((Rectangle){centerX,
                                    startY + 3 * (buttonHeight + spacing),
                                    buttonWidth, buttonHeight},
                        btnLeaveImg, !isSettingsOpen)) {
      break;
    }

    // 5. Crédits
    int creditsWidth = 150;
    int creditsHeight = 50;
    int creditsMargin = 20;
    if (DrawImageButton(
            (Rectangle){creditsMargin,
                        GetScreenHeight() - creditsHeight - creditsMargin,
                        creditsWidth, creditsHeight},
            btnCreditsImg, !isSettingsOpen)) {
      RunCredits();
    }

    // 6. Paramètres É
    int settingsSize = 65;
    if (DrawImageButton(
            (Rectangle){GetScreenWidth() - settingsSize - creditsMargin,
                        GetScreenHeight() - settingsSize - creditsMargin,
                        settingsSize, settingsSize},
            btnSettingsImg, !isSettingsOpen)) {
      isSettingsOpen = true;
    }

    if (isSettingsOpen) {
      isSettingsOpen = DrawSettingsPopup(musicMenu);
    }

    EndDrawing();
  }

  // --- 3. DÉCHARGEMENT DE LA MÉMOIRE ---
  if (isAnim)
    UnloadImage(imBackgroundAnim);
  UnloadTexture(background);
  UnloadTexture(btnPlayImg);
  UnloadTexture(btnCreditsImg);
  UnloadTexture(btnSettingsImg);
  UnloadTexture(btnEditorImg);
  UnloadTexture(btnLeaveImg);
  UnloadTexture(btnStatsImg);
  UnloadTexture(btnSettingsImg);
  UnloadMusicStream(musicMenu); // Décharger la musique de la RAM
  CloseAudioDevice();           // Fermer le périphérique audio
  CloseWindow();
  return 0;
}
