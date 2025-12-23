/*
 * PROJET MAIN3 / METRO
 * Gestion de l'écran plan de l'interface graphique SDL2.
 */
#ifndef GUI_PLAN_H
#define GUI_PLAN_H

#include <SDL2/SDL.h>

// Déclaration de la structure d'état
typedef struct Etat_affichage Etat_affichage;

void gui_dessiner_plan(Etat_affichage *app, int largeur, int hauteur);
void gui_plan_gerer_clavier(Etat_affichage *app, SDL_Keycode key, bool *running);
void gui_plan_gerer_clic(Etat_affichage *app, int x, int y);
void gui_plan_actualiser_recherche(Etat_affichage *app);

#endif
