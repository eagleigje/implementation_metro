/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'écran plan de l'interface graphique SDL2.
 */

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "gui.h"
#include "gui_plan.h"
#include "gui_commun.h"

/**
 * Fonction qui projette une coordonnée géographique sur l'écran.
 * Calcule les limites min/max à partir de la liste des stations,
 * puis applique une transformation linéaire avec zoom et décalage.
 * @param app : État global de l'application graphique.
 * @param coord : Coordonnée géographique à projeter.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 * @param x : Sortie de la position X en pixels.
 * @param y : Sortie de la position Y en pixels.
 */
static void geo_vers_ecran(Etat_affichage *app, Une_coord coord, int largeur, int hauteur, int *x, int *y) {
    if (app == NULL || app->liste_stations == NULL || x == NULL || y == NULL) {
        return;
    }

    // Calcul des limites géographiques à partir de la liste des stations
    Une_coord limite_no;
    Une_coord limite_se;
    limites_zone(app->liste_stations, &limite_no, &limite_se);

    float lon_min = limite_no.lon;
    float lon_max = limite_se.lon;
    float lat_min = limite_se.lat;
    float lat_max = limite_no.lat;

    // Sécurité pour éviter les divisions par zéro
    float largeur_geo = lon_max - lon_min;
    float hauteur_geo = lat_max - lat_min;
    if (largeur_geo == 0.0f || hauteur_geo == 0.0f) {
        *x = largeur / 2;
        *y = hauteur / 2;
        return;
    }

    // Normalisation des coordonnées en [0,1]
    float nx = (coord.lon - lon_min) / largeur_geo;
    float ny = (lat_max - coord.lat) / hauteur_geo; // Inversion de Y pour l'écran

    // Application de la transformation vers l'écran, avec marge
    float marge = (float)app->marge_plan;
    float largeur_util = (float)largeur - 2.0f * marge;
    float hauteur_util = (float)hauteur - 2.0f * marge;
    if (largeur_util <= 0.0f || hauteur_util <= 0.0f) {
        *x = largeur / 2;
        *y = hauteur / 2;
        return;
    }

    float px = marge + nx * largeur_util;
    float py = marge + ny * hauteur_util;

    // Zoom et décalage
    px = (px - largeur / 2.0f) * app->zoom + largeur / 2.0f + app->decalage_x;
    py = (py - hauteur / 2.0f) * app->zoom + hauteur / 2.0f + app->decalage_y;

    *x = (int)px;
    *y = (int)py;
}

/**
 * Fonction qui dessine l'écran plan avec grille et panneau d'information.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_plan_dessiner(Etat_affichage *app, int largeur, int hauteur) {
    // On efface l'écran avec une couleur beige clair (style carte).
    Couleur fond = COULEUR_FOND_PLAN;
    SDL_SetRenderDrawColor(app->moteurRendu, fond.r, fond.g, fond.b, fond.a);
    SDL_RenderClear(app->moteurRendu);

    // On dessine une grille de repérage (quadrillage tous les N pixels).
    Couleur grille = COULEUR_GRILLE;
    SDL_SetRenderDrawColor(app->moteurRendu, grille.r, grille.g, grille.b, grille.a);
    for (int x = 0; x < largeur; x += PLAN_TAILLE_GRILLE) {
        SDL_RenderDrawLine(app->moteurRendu, x, 0, x, hauteur);
    }
    for (int y = 0; y < hauteur; y += PLAN_TAILLE_GRILLE) {
        SDL_RenderDrawLine(app->moteurRendu, 0, y, largeur, y);
    }

    // On dessine le panneau d'information en haut à gauche
    SDL_Rect panneau_info = {PLAN_PANNEAU_X, PLAN_PANNEAU_Y, PLAN_PANNEAU_LARGEUR, PLAN_PANNEAU_HAUTEUR};
    Couleur fond_panneau = COULEUR_PANNEAU_FOND;
    SDL_SetRenderDrawColor(app->moteurRendu, fond_panneau.r, fond_panneau.g, fond_panneau.b, fond_panneau.a);
    SDL_RenderFillRect(app->moteurRendu, &panneau_info);
    Couleur bordure_panneau = COULEUR_PANNEAU_BORDURE;
    SDL_SetRenderDrawColor(app->moteurRendu, bordure_panneau.r, bordure_panneau.g, bordure_panneau.b, bordure_panneau.a);
    SDL_RenderDrawRect(app->moteurRendu, &panneau_info);

    // On affiche le mode actif dans le panneau
    SDL_Color couleur_info = COULEUR_SDL(COULEUR_TEXTE_INFO);
    if (app->mode == MODE_SELECTION) {
        gui_rendu_texte(app, app->petite_police, "Mode : SÉLECTION", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_1, couleur_info);
    } else {
        gui_rendu_texte(app, app->petite_police, "Mode : PLUS COURT CHEMIN", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_1, couleur_info);
    }
    // On affiche les raccourcis clavier
    gui_rendu_texte(app, app->petite_police, "S/P mode  -  ESC menu  -  Q quitter", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_2, couleur_info);

    // Si l'utilisateur a cliqué, on affiche un curseur rouge à la position du clic
    if (app->dernier_clic_x >= 0 && app->dernier_clic_y >= 0) {
        SDL_Rect curseur = {app->dernier_clic_x - PLAN_CURSEUR_DEMI_TAILLE, app->dernier_clic_y - PLAN_CURSEUR_DEMI_TAILLE, PLAN_CURSEUR_TAILLE, PLAN_CURSEUR_TAILLE};
        Couleur couleur_curseur = COULEUR_CURSEUR;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_curseur.r, couleur_curseur.g, couleur_curseur.b, couleur_curseur.a);
        SDL_RenderFillRect(app->moteurRendu, &curseur);
    }
}

/**
 * Fonction qui gère les événements clavier sur le plan.
 * @param app : État global de l'application graphique.
 * @param key : Code de la touche pressée.
 * @param running : Pointeur vers le flag d'exécution de la boucle principale.
 */
void gui_plan_gerer_clavier(Etat_affichage *app, SDL_Keycode key, bool *running) {
    // On est sur le plan : touches ESC, S, P, Q, +/-, flèches
    if (key == SDLK_ESCAPE) {
        app->ecran = ECRAN_MENU;
        gui_mettre_a_jour_titre(app);
    } else if (key == SDLK_s) {
        app->mode = MODE_SELECTION;
        gui_mettre_a_jour_titre(app);
    } else if (key == SDLK_p) {
        app->mode = MODE_CHEMIN;
        gui_mettre_a_jour_titre(app);
    } else if (key == SDLK_q) {
        *running = false;
    } else if (key == SDLK_PLUS || key == SDLK_KP_PLUS) {
        app->zoom *= ZOOM_FACTEUR_AVANT;  // Zoom avant
    } else if (key == SDLK_MINUS || key == SDLK_KP_MINUS) {
        app->zoom *= ZOOM_FACTEUR_ARRIERE;  // Zoom arrière
    } else if (key == SDLK_LEFT) {
        app->decalage_x -= DEPLACEMENT_PAS;  // Déplacement vers la gauche
    } else if (key == SDLK_RIGHT) {
        app->decalage_x += DEPLACEMENT_PAS;  // Déplacement vers la droite
    } else if (key == SDLK_UP) {
        app->decalage_y -= DEPLACEMENT_PAS;  // Déplacement vers le haut
    } else if (key == SDLK_DOWN) {
        app->decalage_y += DEPLACEMENT_PAS;  // Déplacement vers le bas
    }
}

/**
 * Fonction qui gère les clics souris sur le plan.
 * @param app : État global de l'application graphique.
 * @param x : Position X du clic.
 * @param y : Position Y du clic.
 */
void gui_plan_gerer_clic(Etat_affichage *app, int x, int y) {
    // On enregistre la position du clic pour afficher un curseur
    app->dernier_clic_x = x;
    app->dernier_clic_y = y;
}
