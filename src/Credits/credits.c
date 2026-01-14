#include "credits.h"
#include <stdio.h>

void RunCredits(void) {
    SetTargetFPS(60);

    // Initialisation du bouton Home
    Rectangle btnHome = { 30, 30, 100, 40 };
    Texture2D background = LoadTexture("src/Menu/images/background_start.jpg");

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Dessiner le fond
        DrawTexturePro(
            background,
            (Rectangle){0, 0, (float)background.width, (float)background.height},
            (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            (Vector2){0, 0}, 0.0f, WHITE);

        // Dessiner l'overlay semi-transparent
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 150});

        // Gestion du bouton Home (Style plus moderne)
        Vector2 mouse = GetMousePosition();
        bool homeHover = CheckCollisionPointRec(mouse, btnHome);
        
        DrawRectangleRec(btnHome, homeHover ? LIGHTGRAY : DARKGRAY);
        DrawRectangleLinesEx(btnHome, 2, WHITE);
        
        // Centrer le texte "Home" dans le bouton
        const char* homeText = "< Home";
        int homeFontSize = 20;
        int homeTextWidth = MeasureText(homeText, homeFontSize);
        DrawText(homeText, 
                 btnHome.x + (btnHome.width - homeTextWidth) / 2, 
                 btnHome.y + (btnHome.height - homeFontSize) / 2, 
                 homeFontSize, WHITE);

        if (homeHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            EndDrawing(); // Important de finir le dessin avant de break
            break;
        }

        // Affichage du titre
        const char *title = "CREDITS";
        int titleFontSize = 60;
        int titleWidth = MeasureText(title, titleFontSize);
        DrawText(title, (GetScreenWidth() - titleWidth) / 2, 80, titleFontSize, GOLD);

        // Affichage des crédits
        const char *creditsText[] = {
            "Jeu développé par :",
            "",
            "BORDET Aurélien"
            "LAGRANGE Gauthier",
            "BENGHACHEM Hatem",
            "VERRIERE Paulin",
            "Projet encadré par : M. De Jonckère Olivier",
        };
        
        int startY = 250;
        int spacing = 50;

        for (int i = 0; i < 6; i++) {
            Color textColor = (i == 0) ? YELLOW : WHITE; // Titre de section en jaune
            int textFontSize = (i == 0) ? 35 : 30;
            
            int textWidth = MeasureText(creditsText[i], textFontSize);
            DrawText(creditsText[i], (GetScreenWidth() - textWidth) / 2, startY + i * spacing, textFontSize, textColor);
        }

        EndDrawing();
    }
    
    UnloadTexture(background);
}
