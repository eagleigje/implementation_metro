/*
 * PROJET MAIN3 / METRO
 * Interface graphique SDL2.
 */
#ifndef GUI_H
#define GUI_H

#include "config.h"

// =============================================================================
// TAILLES DE LA FENÊTRE
// =============================================================================

enum {
    GUI_LARGEUR_MIN = 1000,
    GUI_HAUTEUR_MIN = 700
};

// =============================================================================
// STRUCTURE COULEUR
// =============================================================================

typedef struct {
    char r; // Red
    char g; // Green
    char b; // Blue
    char a; // Alpha pour la transparence.
} Couleur;

// =============================================================================
// CONSTANTES DE COULEURS
// =============================================================================

// Couleurs du menu
#define COULEUR_FOND_MENU        ((Couleur){18, 24, 32, 255})
#define COULEUR_CARTE_1          ((Couleur){52, 120, 196, 255})   // Bleu
#define COULEUR_CARTE_2          ((Couleur){70, 156, 120, 255})   // Vert
#define COULEUR_CARTE_3          ((Couleur){160, 78, 78, 255})    // Rouge
#define COULEUR_CONTOUR_CARTE    ((Couleur){240, 240, 240, 255})  // Blanc
#define COULEUR_TEXTE_MENU       ((Couleur){245, 245, 245, 255})  // Blanc cassé

// Couleurs du plan
#define COULEUR_FOND_PLAN        ((Couleur){245, 240, 228, 255})  // Beige clair
#define COULEUR_GRILLE           ((Couleur){210, 210, 210, 255})  // Gris clair
#define COULEUR_PANNEAU_FOND     ((Couleur){30, 30, 30, 200})     // Noir semi-transparent
#define COULEUR_PANNEAU_BORDURE  ((Couleur){240, 240, 240, 255})  // Blanc
#define COULEUR_TEXTE_INFO       ((Couleur){240, 240, 240, 255})  // Blanc
#define COULEUR_CURSEUR          ((Couleur){200, 60, 60, 255})    // Rouge

// =============================================================================
// CONSTANTES DE DIMENSIONS - MENU
// =============================================================================

#define MENU_LARGEUR_CARTE       440
#define MENU_HAUTEUR_CARTE       70
#define MENU_OFFSET_CARTE_1_Y    -140
#define MENU_OFFSET_CARTE_2_Y    -50
#define MENU_OFFSET_CARTE_3_Y    40
#define MENU_OFFSET_TITRE_Y      -210
#define MENU_MARGE_TEXTE_X       18
#define MENU_MARGE_TEXTE_Y       20

// =============================================================================
// CONSTANTES DE DIMENSIONS - PLAN
// =============================================================================

#define PLAN_TAILLE_GRILLE       40
#define PLAN_PANNEAU_X           12
#define PLAN_PANNEAU_Y           12
#define PLAN_PANNEAU_LARGEUR     220
#define PLAN_PANNEAU_HAUTEUR     60
#define PLAN_MARGE_TEXTE_X       8
#define PLAN_MARGE_TEXTE_Y_1     8
#define PLAN_MARGE_TEXTE_Y_2     32
#define PLAN_CURSEUR_DEMI_TAILLE 4
#define PLAN_CURSEUR_TAILLE      8

// =============================================================================
// CONSTANTES DE CONTRÔLE
// =============================================================================

#define ZOOM_FACTEUR_AVANT       1.1f
#define ZOOM_FACTEUR_ARRIERE     0.9f
#define DEPLACEMENT_PAS          10.0f

// =============================================================================
// CONSTANTES DE POLICES
// =============================================================================

#define TAILLE_PETITE_POLICE     18
#define TAILLE_GRANDE_POLICE     26

// Convertit une Couleur en SDL_Color pour les fonctions SDL
#define COULEUR_SDL(c) ((SDL_Color){c.r, c.g, c.b, c.a})

// Fonction pour démarrer l'interface.
int gui_lancer(const Une_config *config);

#endif
