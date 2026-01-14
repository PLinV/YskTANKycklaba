#include "menu.h"
#include "../Game/Game.h"
#include "../Graphisme/graphisme.h"
#include "../statPage/statPage.h"
#include "../Credits/credits.h"
#include "GameSetup.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- NOUVELLE FONCTION POUR LE BOUTON IMAGE ---
bool DrawImageButton(Rectangle bounds, Texture2D texture) {
  bool pressed = false;
  Vector2 mousePoint = GetMousePosition();
  bool isHover = CheckCollisionPointRec(mousePoint, bounds);

  // Changement de teinte au survol pour donner un effet visuel
  // WHITE = couleur normale, LIGHTGRAY = un peu plus sombre quand on passe la
  // souris
  Color tint = isHover ? LIGHTGRAY : WHITE;

  if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    pressed = true;
  }

  // On dessine l'image en l'étirant pour qu'elle remplisse le rectangle
  // "bounds"
  DrawTexturePro(texture,
                 (Rectangle){0, 0, (float)texture.width,
                             (float)texture.height}, // Source (toute l'image)
                 bounds, // Destination (taille du bouton définie dans le main)
                 (Vector2){0, 0}, 0.0f, tint);

  return pressed;
}

int main() {
  InitWindow(1000, 640, "YskTANKycklaba - Menu");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  // 1. INITIALISER LE PÉRIPHÉRIQUE AUDIO
  InitAudioDevice();
  // 2. CHARGER LA MUSIQUE
  // Formats supportés : .mp3, .ogg, .wav, .flac
  Music music = LoadMusicStream("src/Menu/musique/tank-battle-13719.wav");
  // Dans la boucle while, après BeginDrawing() :

  // 3. LANCER LA LECTURE
  PlayMusicStream(music);
  SetMusicVolume(music, 0.5f);

  Texture2D background = LoadTexture("src/Menu/images/background_start.jpg");
  // TEST POUR VOIR SI L'IMAGE CHARGE BIEN
  if (background.id == 0) {
    printf("Failed to load background image.\n");
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

  // --- 2. BOUCLE PRINCIPALE ---
  while (!WindowShouldClose()) {
    UpdateMusicStream(music);

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

    int buttonHeight = GetScreenHeight() * 0.1; // 10% of screen height
    if (buttonHeight < 40)
      buttonHeight = 40;

    int spacing = GetScreenHeight() * 0.04; // 4% spacing

    int centerX = (GetScreenWidth() - buttonWidth) / 2;
    int startY = GetScreenHeight() * 0.35; // Start at 35% height

    // 1. Jouer
    if (DrawImageButton((Rectangle){centerX, startY, buttonWidth, buttonHeight},
                        btnPlayImg)) {
      char mapPath[512] = {0};
      int playerCount = 2;
      RunGameSetup(mapPath, &playerCount);
      if (mapPath[0] != '\0') {
        RunGame(mapPath, playerCount);
        SetWindowTitle("YskTANKycklaba - Menu");
      }
    }

    // 2. Statistiques
    if (DrawImageButton((Rectangle){centerX, startY + buttonHeight + spacing,
                                    buttonWidth, buttonHeight},
                        btnStatsImg)) {
      RunStats();
      SetWindowTitle("YskTANKycklaba - Menu");
    }

    // 3. Map Editor
    if (DrawImageButton((Rectangle){centerX,
                                    startY + 2 * (buttonHeight + spacing),
                                    buttonWidth, buttonHeight},
                        btnEditorImg)) {
      RunEditor();
      SetWindowTitle("YskTANKycklaba - Menu");
    }

    // 4. Quitter
    if (DrawImageButton((Rectangle){centerX,
                                    startY + 3 * (buttonHeight + spacing),
                                    buttonWidth, buttonHeight},
                        btnLeaveImg)) {
      break;
    }

    // 5. Crédits (En bas à gauche)
    int creditsWidth = 150;
    int creditsHeight = 50;
    int creditsMargin = 20;
    if (DrawImageButton((Rectangle){creditsMargin,
                                    GetScreenHeight() - creditsHeight - creditsMargin,
                                    creditsWidth, creditsHeight},
                        btnCreditsImg)) {
      RunCredits();
      SetWindowTitle("YskTANKycklaba - Menu");
    }

    EndDrawing();
  }

  // --- 3. DÉCHARGEMENT DE LA MÉMOIRE ---
  UnloadTexture(background);
  UnloadTexture(btnPlayImg);
  UnloadTexture(btnCreditsImg);
  UnloadMusicStream(music); // Décharger la musique de la RAM
  CloseAudioDevice();       // Fermer le périphérique audio
  CloseWindow();
  return 0;
}
