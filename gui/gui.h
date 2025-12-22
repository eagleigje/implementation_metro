/*
 * PROJET MAIN3 / METRO
 * Interface graphique SDL2.
 */
#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../config.h"
#include "../liste.h"
#include "../abr.h"
#include "../aqrtopo.h"
#include "../ligne.h"

// =============================================================================
// TAILLES DE LA FENÊTRE
// =============================================================================

enum {
    GUI_LARGEUR_MIN = 1000,  // Largeur minimale de la fenêtre.
    GUI_HAUTEUR_MIN = 700    // Hauteur minimale de la fenêtre.
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
#define COULEUR_FOND_MENU        ((Couleur){18, 24, 32, 255})      // Fond principal du menu.
#define COULEUR_BOUTON_MENU_1    ((Couleur){52, 120, 196, 255})    // Bouton 1 du menu (bleu).
#define COULEUR_BOUTON_MENU_2    ((Couleur){70, 156, 120, 255})    // Bouton 2 du menu (vert).
#define COULEUR_BOUTON_MENU_3    ((Couleur){160, 78, 78, 255})     // Bouton 3 du menu (rouge).
#define COULEUR_CONTOUR_CARTE    ((Couleur){240, 240, 240, 255})   // Contour des cartes.
#define COULEUR_TEXTE_MENU       ((Couleur){245, 245, 245, 255})   // Texte du menu (blanc cassé).

// Couleurs du plan
#define COULEUR_FOND_PLAN        ((Couleur){245, 240, 228, 255})   // Fond du plan (beige clair).
#define COULEUR_GRILLE           ((Couleur){210, 210, 210, 255})   // Couleur de la grille.
#define COULEUR_PANNEAU_FOND     ((Couleur){30, 30, 30, 200})      // Fond du panneau d'information.
#define COULEUR_PANNEAU_BORDURE  ((Couleur){240, 240, 240, 255})   // Bordure du panneau.
#define COULEUR_TEXTE_INFO       ((Couleur){240, 240, 240, 255})   // Texte d'information.
#define COULEUR_CURSEUR          ((Couleur){200, 60, 60, 255})     // Curseur de clic.
#define COULEUR_STATION          ((Couleur){20, 20, 20, 255})      // Couleur des stations.

// =============================================================================
// CONSTANTES DE DIMENSIONS - MENU PRINCIPAL
// =============================================================================

#define MENU_LARGEUR_CARTE       440   // Largeur d'une carte du menu.
#define MENU_HAUTEUR_CARTE       70    // Hauteur d'une carte du menu.
#define MENU_OFFSET_CARTE_1_Y    -140  // Décalage vertical de la carte 1 (par rapport au centre).
#define MENU_OFFSET_CARTE_2_Y    -50   // Décalage vertical de la carte 2 (par rapport au centre).
#define MENU_OFFSET_CARTE_3_Y    40    // Décalage vertical de la carte 3 (par rapport au centre).
#define MENU_OFFSET_TITRE_Y      -210  // Décalage vertical du titre (par rapport au centre).
#define MENU_MARGE_TEXTE_X       18    // Marge horizontale du texte dans les cartes.
#define MENU_MARGE_TEXTE_Y       20    // Marge verticale du texte dans les cartes.

// =============================================================================
// CONSTANTES DE DIMENSIONS - PLAN
// =============================================================================

#define PLAN_TAILLE_GRILLE       40   // Pas de la grille en pixels.
#define PLAN_MARGE_PLAN          24   // Marge en pixels autour du plan projeté.
#define PLAN_PANNEAU_X           12   // Position X du panneau d'information.
#define PLAN_PANNEAU_Y           12   // Position Y du panneau d'information.
#define PLAN_PANNEAU_LARGEUR     400  // Largeur du panneau d'information.
#define PLAN_PANNEAU_HAUTEUR     60   // Hauteur du panneau d'information.
#define PLAN_MARGE_TEXTE_X       8    // Marge X du texte dans le panneau.
#define PLAN_MARGE_TEXTE_Y_1     8    // Décalage Y du premier texte du panneau.
#define PLAN_MARGE_TEXTE_Y_2     32   // Décalage Y du second texte du panneau.
#define PLAN_CURSEUR_DEMI_TAILLE 4    // Demi-taille du curseur de clic.
#define PLAN_CURSEUR_TAILLE      8    // Taille du curseur de clic.
#define PLAN_TAILLE_STATION      6    // Taille d'une station (carré).
#define PLAN_EPAISSEUR_LIGNE     2    // Épaisseur des lignes de connexion (en pixels).

// =============================================================================
// CONSTANTES DE CONTRÔLE
// =============================================================================

#define ZOOM_FACTEUR_AVANT       1.1f   // Facteur de zoom avant.
#define ZOOM_FACTEUR_ARRIERE     0.9f   // Facteur de zoom arrière.
#define DEPLACEMENT_PAS          10.0f  // Pas de déplacement au clavier.

// =============================================================================
// CONSTANTES DE POLICES
// =============================================================================

#define TAILLE_PETITE_POLICE     18   // Taille de la police pour les textes secondaires.
#define TAILLE_GRANDE_POLICE     26   // Taille de la police pour les titres.

// =============================================================================
// MACROS UTILITAIRES
// =============================================================================

// Convertit une Couleur en SDL_Color pour les fonctions SDL
#define COULEUR_SDL(c) ((SDL_Color){c.r, c.g, c.b, c.a})  // Conversion Couleur -> SDL_Color.

// =============================================================================
// TYPES ÉNUMÉRÉS
// =============================================================================

/**
 * Type énuméré représentant les différents écrans de l'application.
 * ECRAN_MENU : Écran d'accueil avec les options de navigation.
 * ECRAN_PLAN : Écran principal affichant le plan du métro.
 */
typedef enum {
    ECRAN_MENU = 0,
    ECRAN_PLAN = 1
} Ecran;

/**
 * Type énuméré représentant les modes d'interaction disponibles sur le plan.
 * MODE_SELECTION : Permet de sélectionner des stations individuellement.
 * MODE_CHEMIN : Permet de calculer le plus court chemin entre deux stations.
 */
typedef enum {
    MODE_SELECTION = 0,
    MODE_CHEMIN = 1
} Mode;

// =============================================================================
// STRUCTURE D'ÉTAT
// =============================================================================

/**
 * Structure contenant l'état global de l'interface graphique.
 * Regroupe les ressources SDL, les paramètres d'affichage et les données du métro.
 */
typedef struct Etat_affichage {
    SDL_Window *fenetre;            // Fenêtre principale de l'application
    SDL_Renderer *moteurRendu;      // Moteur de rendu SDL pour dessiner à l'écran
    TTF_Font *petite_police;        // Police de petite taille pour les textes secondaires
    TTF_Font *grande_police;        // Police de grande taille pour les titres
    Ecran ecran;                    // Écran actuellement affiché (menu ou plan)
    Mode mode;                      // Mode d'interaction actif (sélection ou chemin)
    float zoom;                     // Facteur de zoom appliqué au plan (1.0 = échelle normale)
    float decalage_x;               // Décalage horizontal de la caméra en pixels
    float decalage_y;               // Décalage vertical de la caméra en pixels
    int marge_plan;                 // Marge en pixels autour du plan projeté
    int dernier_clic_x;             // Position X du dernier clic souris (-1 si aucun)
    int dernier_clic_y;             // Position Y du dernier clic souris (-1 si aucun)
    // Données du métro
    Un_elem *liste_stations;        // Liste chaînée de toutes les stations
    Un_elem *liste_connexions;      // Liste chaînée de toutes les connexions
    Un_nabr *abr_stations;          // Arbre binaire de recherche pour recherche rapide par nom
    Une_ligne *lignes;              // Liste des lignes de métro
    Un_noeud *aqr_topo;             // Arbre quaternaire pour recherche spatiale des stations
} Etat_affichage;

// =============================================================================
// FONCTION PRINCIPALE
// =============================================================================S
int lancer_gui(const Une_config *config);

#endif
