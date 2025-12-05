#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

// renvoie true si le bouton est cliqué sinon false
// font_size permet aussi de faire le taille plus au moins gros car il dépend de la taille du texte
bool DrawButton(Rectangle *bounds, const char *text, int font_size);

int center_x(); // le x du milieu
int center_y(); // le y du milieu

#endif
