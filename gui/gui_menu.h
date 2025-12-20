/*
 * PROJET MAIN3 / METRO
 * Gestion de l'écran menu de l'interface graphique SDL2.
 */
#ifndef GUI_MENU_H
#define GUI_MENU_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Forward declaration de la structure d'état
typedef struct Etat_affichage Etat_affichage;

/**
 * Fonction qui dessine l'écran menu principal.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_menu_dessiner(Etat_affichage *app, int largeur, int hauteur);

/**
 * Fonction qui gère les événements clavier dans le menu.
 * @param app : État global de l'application graphique.
 * @param key : Code de la touche pressée.
 * @param running : Pointeur vers le flag d'exécution de la boucle principale.
 */
void gui_menu_gerer_clavier(Etat_affichage *app, SDL_Keycode key, bool *running);

/**
 * Fonction qui gère les clics souris dans le menu.
 * @param app : État global de l'application graphique.
 * @param x : Position X du clic.
 * @param y : Position Y du clic.
 * @param running : Pointeur vers le flag d'exécution de la boucle principale.
 */
void gui_menu_gerer_clic(Etat_affichage *app, int x, int y, bool *running);

#endif
