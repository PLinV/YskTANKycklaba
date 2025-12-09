#ifndef INPUT_BOX_H
#define INPUT_BOX_H

#include "raylib.h"
#include <stdbool.h>

bool InputTextBox(Rectangle bounds, char *text, int maxLength, bool *active);

#endif
