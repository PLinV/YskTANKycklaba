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

  static bool DrawButton(Rectangle *bounds, const char *text, int font_size) {
    Font font = GetFontDefault();

    // padding horizontal et vertical
    const float padding_x = 12.0f;
    const float padding_y = 8.0f;

    // mesure le texte
    Vector2 text_size = MeasureTextEx(font, text, font_size, 1);

    // nouvelle taille du bouton
    bounds->width  = text_size.x + padding_x * 2;
    bounds->height = text_size.y + padding_y * 2;

    // logique hover / clic
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, *bounds);
    bool clicked = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    // dessin du bouton
    DrawRectangleRec(*bounds, hover ? SKYBLUE : LIGHTGRAY);
    DrawRectangleLines(bounds->x, bounds->y, bounds->width, bounds->height, BLACK);

    // position centrée du texte
    Vector2 text_pos = {
      bounds->x + (bounds->width  - text_size.x) / 2.0f,
      bounds->y + (bounds->height - text_size.y) / 2.0f
    };

    DrawTextEx(font, text, text_pos, font_size, 1, BLACK);

    return clicked;
  }

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

      // partie titre
      char *titre = "STATS";
      int fontSizeTitre = 50;
      int titreWidth = MeasureText(titre, fontSizeTitre);

      // partie btn home 
      char *msgBtnHome = "changer l'action"; // il faut changer cette ligne pour revenir a l'écran d'accueil
      int fontSizeBtnHome = 30;
      int msgBtnHomeWidth = MeasureText(msgBtnHome, fontSizeBtnHome);
      Rectangle btnHome = { 30, 30, 0, 0}; // heigh et width sera calculé par la fonction en fonction du texte donné
      bool show = false;

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

        // --- DÉBUT DU DESSIN ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // si le bouton est cliqué => activer l'affichage du message
        if (DrawButton(&btnHome, "< Home", 20)) {
          EndDrawing();
          break;
        }

        // input Users 
        inputBox1.x = GetScreenWidth() / 4 - (inputBox1.width / 2);
        inputBox2.x = (GetScreenWidth() * 3) / 4 - (inputBox2.width / 2);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // affiche le titre de la page stats
        DrawText(titre, center_x() - titreWidth/2, 50, fontSizeTitre, BLACK);

        // users
        InputTextBox(inputBox1, username1, 31, &active1);
        InputTextBox(inputBox2, username2, 31, &active2);

        DrawText("Entrez votre pseudo 1 :", inputBox1.x, inputBox1.y - 30, 20, BLACK);
        DrawText("Entrez votre pseudo 2 :", inputBox2.x, inputBox1.y - 30, 20, BLACK);
        
        if (IsKeyPressed(KEY_ENTER)) {
          strcpy(username1Temp, username1);
          strcpy(username2Temp, username2);
          user1 = getUser(username1);
          user2 = getUser(username2);
        }
        

        DrawText(TextFormat("Pseudo 1 : %s", username1Temp), inputBox1.x, inputBox1.y + 60, 20, DARKGRAY);
        DrawText(TextFormat("Pseudo 2 : %s", username2Temp), inputBox2.x, inputBox1.y + 60, 20, DARKGRAY);

        if(user1 != NULL) {
          DrawText(TextFormat("nbPartie user1 : %d", user1->gamePlayed), center_x() - center_x()/2, 450, 20, DARKGRAY);
          DrawText(TextFormat("Kill/death ratio user1 : %.2f", getKdRatio(user1)), center_x() - center_x()/2, 500, 20, DARKGRAY);
        }

        if(user2 != NULL) {
          DrawText(TextFormat("nbPartie user2 : %d", user2->gamePlayed), center_x() + center_x()/2, 450, 20, DARKGRAY);
          DrawText(TextFormat("Kill/death ratio user2 : %.2f", getKdRatio(user2)), center_x() + center_x()/2, 500, 20, DARKGRAY);
        }


        EndDrawing();
    }

    return 0;
  }
