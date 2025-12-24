/*
 * PROJET MAIN3 / METRO
 * Fonctions communes de l'interface graphique SDL2.
 */
#ifndef GUI_COMMUN_H
#define GUI_COMMUN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"

void gui_mettre_a_jour_titre(Etat_affichage *app);
void gui_afficher_aide(void);
TTF_Font *gui_charger_police(int taille);
void gui_rendu_texte(Etat_affichage *app, TTF_Font *police, const char *texte, int x, int y, SDL_Color couleur);

#endif
