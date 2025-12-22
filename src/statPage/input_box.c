#include "input_box.h"
#include <string.h>

bool InputTextBox(Rectangle bounds, char *text, int maxLength, bool *active) {
  Vector2 mouse = GetMousePosition();
  bool hovered = CheckCollisionPointRec(mouse, bounds);

  // active l’input au clic
  if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    *active = true;
  else if (!hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    *active = false;

  // si l’input est actif => on récupère les caractères tapés
  if (*active) {
    int key = GetCharPressed();

    // lis tous les caractères tapés
    while (key > 0) {
      int len = strlen(text);

      if (key >= 32 && key <= 125 && len < maxLength) {
        text[len] = (char)key;
        text[len + 1] = '\0';
      }

      key = GetCharPressed();
    }

    // gére le backspace
    if (IsKeyPressed(KEY_BACKSPACE)) {
      int len = strlen(text);
      if (len > 0) text[len - 1] = '\0';
    }
  }

  // couleur selon focus
  DrawRectangleRec(bounds, *active ? SKYBLUE : LIGHTGRAY);
  DrawRectangleLinesEx(bounds, 2, BLACK);

  // affichage du texte
  DrawText(text, bounds.x + 10, bounds.y + 10, 20, BLACK);

  // curseur clignotant
  if (*active && ((GetTime() * 2) - (int)(GetTime() * 2)) > 0.5) {
    int cursorX = bounds.x + 10 + MeasureText(text, 20);
    DrawRectangle(cursorX, bounds.y + 10, 8, 20, BLACK);
  }

  return *active;
}
