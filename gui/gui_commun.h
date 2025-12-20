/*
 * PROJET MAIN3 / METRO
 * Fonctions communes de l'interface graphique SDL2.
 */
#ifndef GUI_COMMUN_H
#define GUI_COMMUN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Forward declaration de la structure d'état
typedef struct Etat_affichage Etat_affichage;

/**
 * Fonction qui met à jour le titre de la fenêtre pour afficher le mode courant.
 * @param app : État global de l'application graphique.
 */
void gui_mettre_a_jour_titre(Etat_affichage *app);

/**
 * Fonction qui affiche les contrôles dans la console au lancement de la fenêtre.
 */
void gui_afficher_aide(void);

/**
 * Fonction qui charge une police TrueType à partir de chemins connus.
 * @param taille : Taille en points de la police à charger.
 * @return : Un pointeur vers la police chargée, ou NULL si aucune n'est disponible.
 */
TTF_Font *gui_charger_police(int taille);

/**
 * Fonction qui rend une ligne de texte à l'écran.
 * @param app : État global de l'application graphique.
 * @param police : Police à utiliser.
 * @param texte : Texte à afficher.
 * @param x : Position X.
 * @param y : Position Y.
 * @param couleur : Couleur du texte.
 */
void gui_rendu_texte(Etat_affichage *app, TTF_Font *police, const char *texte, int x, int y, SDL_Color couleur);

#endif
