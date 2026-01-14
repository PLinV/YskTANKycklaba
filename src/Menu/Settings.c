#include "Settings.h"
#include <stdio.h>

// Variables globales de volume
static float musicVolume = 0.5f;
static float sfxVolume = 0.5f;

float GetMusicVolumeValue() {
    return musicVolume;
}

float GetSFXVolumeValue() {
    return sfxVolume;
}

// Implémentation simple d'un slider
// Retourne true si la valeur a changé
bool DrawSlider(Rectangle bounds, const char *text, float *value, float min, float max) {
    bool changed = false;
    Vector2 mouse = GetMousePosition();
    
    // Dessiner le texte
    DrawText(text, bounds.x, bounds.y - 25, 20, WHITE);
    
    // Dessiner le fond de la barre
    DrawRectangleRec(bounds, Fade(WHITE, 0.2f));
    DrawRectangleLinesEx(bounds, 1, DARKGRAY);
    
    // Calculer la position du slider
    float normalized = (*value - min) / (max - min);
    
    // Dessiner la barre remplie
    Rectangle filledBounds = {bounds.x, bounds.y, normalized * bounds.width, bounds.height};
    DrawRectangleRec(filledBounds, GOLD);

    // Curseur (Knob)
    float knobX = bounds.x + normalized * bounds.width;
    Rectangle knob = { knobX - 8, bounds.y - 4, 16, bounds.height + 8 };
    
    bool isHover = CheckCollisionPointRec(mouse, bounds) || CheckCollisionPointRec(mouse, knob);
    
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
         // Zone de détection un peu plus large pour faciliter la prise en main
         if (isHover || (CheckCollisionPointRec(mouse, (Rectangle){bounds.x - 20, bounds.y - 20, bounds.width + 40, bounds.height + 40}))) {
            float newVal = (mouse.x - bounds.x) / bounds.width;
            if (newVal < 0) newVal = 0;
            if (newVal > 1) newVal = 1;
            *value = min + newVal * (max - min);
            changed = true;
         }
    }
    
    // Dessiner le curseur
    DrawRectangleRec(knob, isHover ? ORANGE : GOLD);
    DrawRectangleLinesEx(knob, 1, BLACK);
    
    // Dessiner la valeur textuelle
    char valText[10];
    sprintf(valText, "%d%%", (int)(*value * 100));
    DrawText(valText, bounds.x + bounds.width + 15, bounds.y, 20, GOLD);
    
    return changed;
}

bool DrawSettingsPopup(Music musicMenu) {
    // Overlay (Assombrir l'arrière-plan du menu)
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

    // Fenêtre Popup
    float popupWidth = 400;
    float popupHeight = 400;
    float popupX = (GetScreenWidth() - popupWidth) / 2;
    float popupY = (GetScreenHeight() - popupHeight) / 2;
    Rectangle popupRect = {popupX, popupY, popupWidth, popupHeight};
    
    // Style d'arrière-plan plus transparent (demande utilisateur: "bcp transparent")
    DrawRectangleRec(popupRect, Fade(BLACK, 0.4f)); 
    DrawRectangleLinesEx(popupRect, 3, GOLD);
    
    // Titre
    const char* title = "PARAMÈTRES";
    DrawText(title, popupX + (popupWidth - MeasureText(title, 30)) / 2, popupY + 20, 30, GOLD);

    // Séparateur
    DrawLine(popupX + 20, popupY + 60, popupX + popupWidth - 20, popupY + 60, GOLD);

    // Slider Volume Musique
    float sliderWidth = 250;
    float sliderHeight = 15;
    float startX = popupX + (popupWidth - sliderWidth) / 2;
    float startY = popupY + 120;
    
    if (DrawSlider((Rectangle){startX, startY, sliderWidth, sliderHeight}, "Volume Musique", &musicVolume, 0.0f, 1.0f)) {
        SetMusicVolume(musicMenu, musicVolume);
    }
    
    // Slider Volume Effets
    DrawSlider((Rectangle){startX, startY + 100, sliderWidth, sliderHeight}, "Volume Effets", &sfxVolume, 0.0f, 1.0f);
    
    // Bouton Retour
    float btnWidth = 120;
    float btnHeight = 40;
    Rectangle backBtn = { popupX + (popupWidth - btnWidth) / 2, popupY + popupHeight - 80, btnWidth, btnHeight };
    
    Vector2 mouse = GetMousePosition();
    bool hoverBack = CheckCollisionPointRec(mouse, backBtn);
    
    // Style du bouton (inspiré de GameSetup)
    Color btnColor = Fade(BLACK, 0.5f);
    if (hoverBack) btnColor = Fade(GOLD, 0.5f);
    
    DrawRectangleRec(backBtn, btnColor);
    DrawRectangleLinesEx(backBtn, 2, BLANK); // Bordure transparente comme demandé précédemment
    
    const char* backText = "< Retour";
    int textW = MeasureText(backText, 20);
    DrawText(backText, backBtn.x + (btnWidth - textW) / 2, backBtn.y + (btnHeight - 20) / 2, 20, WHITE);
    
    if (hoverBack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return false; // Fermer le popup
    }
    
    return true; // Garder ouvert
}
