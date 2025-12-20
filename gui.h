/*
 * PROJET MAIN3 / METRO
 * Interface graphique SDL2.
 */
#ifndef GUI_H
#define GUI_H

#include "config.h"

// Tailles minimales de la fenêtre graphique
enum {
    GUI_LARGEUR_MIN = 1000,
    GUI_HAUTEUR_MIN = 700
};

int gui_lancer(const Une_config *config);

#endif
