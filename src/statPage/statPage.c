  #include "raylib.h"
  //gcc src/statPage/statPage.c \
    src/statPage/input_box.c \
    src/dataCode/user.c \
    -o hello \
    -Iraylib/src \
    raylib/src/libraylib.a \
    -lm -lpthread -ldl -lrt -lX11 -lGL
    
  // Pour lancer le test
  #include "statPage.h"
  #include "input_box.h"
  #include <string.h>
  #include "../dataCode/user.h"



    int center_x() {
        int screenWidth = GetScreenWidth();
        int x_center = screenWidth/2;
        return x_center;
    }

    int center_y() {
        int screenHeight = GetScreenHeight();
        int y_center = screenHeight/2;
        return y_center;
    }


    int RunStats(void){
      SetTargetFPS(60);

      Texture2D background = LoadTexture("src/Menu/images/background_start.jpg");

      // partie titre
      char *titre = "STATS";
      int fontSizeTitre = 60; // Plus grand
      int titreWidth = MeasureText(titre, fontSizeTitre);

      // partie btn home 
      Rectangle btnHome = { 30, 30, 100, 40}; 

      //partie user1 
      char username1[32] = "";
      char username1Temp[32] = "";
      User* user1 = NULL;
      Rectangle inputBox1;
      inputBox1.width = 300;
      inputBox1.height = 50;
      inputBox1.y = 200;
      bool active1 = false;

      // partie user2
      char username2[32] = "";
      char username2Temp[32] = "";
      User* user2 = NULL;
      Rectangle inputBox2;
      inputBox2.width = 300;
      inputBox2.height = 50;
      inputBox2.y = 200;
      bool active2 = false;


      while (!WindowShouldClose()) {

        // input Users 
        inputBox1.x = GetScreenWidth() / 4 - (inputBox1.width / 2);
        inputBox2.x = (GetScreenWidth() * 3) / 4 - (inputBox2.width / 2);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 1. Fond et Overlay
        DrawTexturePro(
            background,
            (Rectangle){0, 0, (float)background.width, (float)background.height},
            (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
            (Vector2){0, 0}, 0.0f, WHITE);
        
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 150}); // Overlay sombre

        // 2. Bouton Home moderne
        Vector2 mouse = GetMousePosition();
        bool homeHover = CheckCollisionPointRec(mouse, btnHome);
        DrawRectangleRec(btnHome, homeHover ? LIGHTGRAY : DARKGRAY);
        DrawRectangleLinesEx(btnHome, 2, WHITE);
        
        const char* homeText = "< Home";
        int homeFontSize = 20;
        int homeTextWidth = MeasureText(homeText, homeFontSize);
        DrawText(homeText, 
                 btnHome.x + (btnHome.width - homeTextWidth) / 2, 
                 btnHome.y + (btnHome.height - homeFontSize) / 2, 
                 homeFontSize, WHITE);

        if (homeHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          EndDrawing();
          break;
        }

        // --- NOUVEAUTÉ: SÉPARATEUR ET CADRES ---
        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();
        
        // Séparateur vertical
        DrawLineEx((Vector2){screenW / 2, 120}, (Vector2){screenW / 2, screenH - 50}, 3.0f, LIGHTGRAY);

        // Cadres pour les joueurs
        // Player 1 Frame
        Rectangle frame1 = { 50, 150, (screenW / 2) - 100, screenH - 250 };
        DrawRectangleLinesEx(frame1, 2.0f, GOLD);
        
        // Player 2 Frame
        Rectangle frame2 = { (screenW / 2) + 50, 150, (screenW / 2) - 100, screenH - 250 };
        DrawRectangleLinesEx(frame2, 2.0f, GOLD);


        // 3. Titre
        DrawText(titre, center_x() - titreWidth/2, 50, fontSizeTitre, GOLD);

        // 4. Users Inputs
        // On dessine d'abord des rectangles blancs pour le fond des inputs si InputTextBox ne le fait pas (mais il le fait probablement)
        // Supposons que InputTextBox gère le dessin.
        InputTextBox(inputBox1, username1, 31, &active1);
        InputTextBox(inputBox2, username2, 31, &active2);

        DrawText("Entrez votre pseudo 1 :", inputBox1.x, inputBox1.y - 30, 20, WHITE);
        DrawText("Entrez votre pseudo 2 :", inputBox2.x, inputBox1.y - 30, 20, WHITE);
        
        if (IsKeyPressed(KEY_ENTER)) {
          strcpy(username1Temp, username1);
          strcpy(username2Temp, username2);
          user1 = getUser(username1);
          user2 = getUser(username2);
        }
        

        DrawText(TextFormat("Pseudo 1 : %s", username1Temp), inputBox1.x, inputBox1.y + 60, 20, LIGHTGRAY);
        DrawText(TextFormat("Pseudo 2 : %s", username2Temp), inputBox2.x, inputBox1.y + 60, 20, LIGHTGRAY);

        if(user1 != NULL) {
          const char* txtGames = TextFormat("Parties jouées : %d", user1->gamePlayed);
          const char* txtRatio = TextFormat("Ratio K/D : %.2f", getKdRatio(user1));
          const char* txtMaxScore = TextFormat("Score max : %d", user1->maxScore);
          
          int fontSizeStats = 30;
          int wGames = MeasureText(txtGames, fontSizeStats);
          int wRatio = MeasureText(txtRatio, fontSizeStats);
          int wMaxScore = MeasureText(txtMaxScore, fontSizeStats);
          
          DrawText(txtGames, (GetScreenWidth()/4) - (wGames/2), 400, fontSizeStats, YELLOW);
          DrawText(txtRatio, (GetScreenWidth()/4) - (wRatio/2), 450, fontSizeStats, WHITE);
          DrawText(txtMaxScore, (GetScreenWidth()/4) - (wMaxScore/2), 500, fontSizeStats, RED);
        }

        if(user2 != NULL) {
          const char* txtGames = TextFormat("Parties jouées : %d", user2->gamePlayed);
          const char* txtRatio = TextFormat("Ratio K/D : %.2f", getKdRatio(user2));
          const char* txtMaxScore = TextFormat("Score max : %d", user2->maxScore);
          
          int fontSizeStats = 35;
          int wGames = MeasureText(txtGames, fontSizeStats);
          int wRatio = MeasureText(txtRatio, fontSizeStats);
          int wMaxScore = MeasureText(txtMaxScore, fontSizeStats);
          
          DrawText(txtGames, ((GetScreenWidth()*3)/4) - (wGames/2), 400, fontSizeStats, YELLOW);
          DrawText(txtRatio, ((GetScreenWidth()*3)/4) - (wRatio/2), 450, fontSizeStats, WHITE);
          DrawText(txtMaxScore, (GetScreenWidth()*3/4) - (wMaxScore/2), 500, fontSizeStats, RED);
        }


        EndDrawing();
    }

    UnloadTexture(background);
    return 0;
  }
