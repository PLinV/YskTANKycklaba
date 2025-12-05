#include "raylib.h"
//gcc YskTANKycklaba/src/screen_select_users/screen_select_users.c \
    -o hello \
    -Iraylib/src \
    raylib/src/libraylib.a \
    -lm -lpthread -ldl -lrt -lX11 -lGL
// Pour lancer
#include "screen_select_users.h"




bool DrawButton(Rectangle *bounds, const char *text, int font_size) {
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

int main(void)
{
  InitWindow(800, 450, "STATS PAGE");
  SetTargetFPS(60);

  // partie titre
  char *titre = "STATS";
  int fontSizeTitre = 40;
  int titreWidth = MeasureText(titre, fontSizeTitre);

  // partie btn home 
  char *msgBtnHome = "changer l'action"; // il faut changer cette ligne pour revenir a l'écran d'accueil
  int fontSizeBtnHome = 40;
  int msgBtnHomeWidth = MeasureText(msgBtnHome, fontSizeBtnHome);
  Rectangle btnHome = { 20, 20, 0, 0}; // heigh et width sera calculé par la fonction en fonction du texte donné
  bool show = false;

  

  while (!WindowShouldClose()) {

    

    // si le bouton est cliqué => activer l'affichage du message
    if (DrawButton(&btnHome, "< Home", 20)) {
        show = true;
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // affiche le titre de la page stats
    DrawText(titre, center_x() - titreWidth/2, 20, fontSizeTitre, BLACK);

    // si show est vrai on affiche changer l'action
    if (show) {
      DrawText(msgBtnHome, center_x() - msgBtnHomeWidth/2, center_y() - fontSizeBtnHome/2, fontSizeBtnHome, BLACK);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
