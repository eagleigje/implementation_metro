/*
 * PROJET MAIN3 / METRO
 * Gestion de l'écran menu de l'interface graphique SDL2.
 */
#ifndef GUI_MENU_H
#define GUI_MENU_H

#include <SDL2/SDL.h>

#include "gui.h"

void gui_menu_dessiner(Etat_affichage *app, int largeur, int hauteur);
void gui_menu_gerer_clavier(Etat_affichage *app, SDL_Keycode key, int *running);
void gui_menu_gerer_clic(Etat_affichage *app, int x, int y, int *running);

#endif
