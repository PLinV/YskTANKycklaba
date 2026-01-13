#ifndef GAMESETUP_H
#define GAMESETUP_H

#include "raylib.h"
#include <stdio.h>
#include <string.h>


// Lance la boucle "setup"
// - Gère le choix du nombre de joueurs
// - Gère les configurations (pseudo, séléection skin...)
// - Gère la sélection de la map (Carousel)
void RunGameSetup(char *selectedMapPath, int *finalPlayerCount);

// Fonction utilitaire pour dessiner un bouton
bool DrawButton(Rectangle bounds, const char *text);

#endif
