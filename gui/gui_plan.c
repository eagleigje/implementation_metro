/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'écran plan de l'interface graphique SDL2.
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"
#include "gui_plan.h"
#include "gui_commun.h"

// Déclaration pour l'utiliser avant sa définition
static void geo_vers_ecran(Etat_affichage *app, Une_coord coord, int largeur, int hauteur, int *x, int *y);

/**
 * Fonction qui calcule la distance au carré entre un point et un segment.
 * @param px : Coordonnée X du point.
 * @param py : Coordonnée Y du point.
 * @param x1 : Coordonnée X du segment (point 1).
 * @param y1 : Coordonnée Y du segment (point 1).
 * @param x2 : Coordonnée X du segment (point 2).
 * @param y2 : Coordonnée Y du segment (point 2).
 * @return : Distance au carré entre le point et le segment.
 */
static float distance_segment_carre(int px, int py, int x1, int y1, int x2, int y2) {
    float vx = (float)(x2 - x1);
    float vy = (float)(y2 - y1);
    float wx = (float)(px - x1);
    float wy = (float)(py - y1);

    float c1 = vx * wx + vy * wy;
    if (c1 <= 0.0f) {
        float dx = (float)(px - x1);
        float dy = (float)(py - y1);
        return dx * dx + dy * dy;
    }

    float c2 = vx * vx + vy * vy;
    if (c2 <= c1) {
        float dx = (float)(px - x2);
        float dy = (float)(py - y2);
        return dx * dx + dy * dy;
    }

    float b = c1 / c2;
    float bx = (float)x1 + b * vx;
    float by = (float)y1 + b * vy;
    float dx = (float)px - bx;
    float dy = (float)py - by;
    return dx * dx + dy * dy;
}

/**
 * Fonction qui recherche la ligne la plus proche de la souris.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 * @return : Ligne la plus proche ou NULL si aucune n'est à portée.
 */
static Une_ligne *trouver_ligne_proche(Etat_affichage *app, int largeur, int hauteur) {
    if (app == NULL || app->liste_connexions == NULL) {
        return NULL;
    }

    int seuil = PLAN_DISTANCE_SURVOL_LIGNE;
    float meilleur = (float)(seuil * seuil) + 1.0f;
    Une_ligne *ligne_proche = NULL;

    for (Un_elem *elem = app->liste_connexions; elem != NULL; elem = elem->suiv) {
        if (elem->truc == NULL || elem->truc->type != CON) {
            continue;
        }

        Un_truc *sta_dep = elem->truc->data.con.sta_dep;
        Un_truc *sta_arr = elem->truc->data.con.sta_arr;
        Une_ligne *ligne = elem->truc->data.con.ligne;
        if (sta_dep == NULL || sta_arr == NULL || ligne == NULL) {
            continue;
        }

        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;
        geo_vers_ecran(app, sta_dep->coord, largeur, hauteur, &x1, &y1);
        geo_vers_ecran(app, sta_arr->coord, largeur, hauteur, &x2, &y2);

        float dist2 = distance_segment_carre(app->souris_x, app->souris_y, x1, y1, x2, y2);
        if (dist2 < meilleur) {
            meilleur = dist2;
            ligne_proche = ligne;
        }
    }

    return ligne_proche;
}

/**
 * Fonction qui recherche la station la plus proche de la souris.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 * @param px : Sortie de la position X de la station.
 * @param py : Sortie de la position Y de la station.
 * @return : Station la plus proche ou NULL si aucune n'est à portée.
 */
static Un_truc *trouver_station_proche(Etat_affichage *app, int largeur, int hauteur,
                                       int x_ref, int y_ref, int *px, int *py, int distance_max) {
    if (app == NULL || app->liste_stations == NULL) {
        return NULL;
    }

    int seuil = distance_max;
    int meilleur = seuil * seuil + 1;
    Un_truc *station_proche = NULL;

    for (Un_elem *elem = app->liste_stations; elem != NULL; elem = elem->suiv) {
        if (elem->truc == NULL || elem->truc->type != STA) {
            continue;
        }

        int sx = 0;
        int sy = 0;
        geo_vers_ecran(app, elem->truc->coord, largeur, hauteur, &sx, &sy);

        int dx = x_ref - sx;
        int dy = y_ref - sy;
        int dist2 = dx * dx + dy * dy;
        if (dist2 < meilleur) {
            meilleur = dist2;
            station_proche = elem->truc;
            *px = sx;
            *py = sy;
        }
    }

    return station_proche;
}

/**
 * Fonction qui dessine une bulle de texte près de la souris.
 * @param app : État global de l'application graphique.
 * @param texte : Texte à afficher.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
static void dessiner_bulle_texte(Etat_affichage *app, const char *texte, int largeur, int hauteur) {
    if (app == NULL || texte == NULL || app->petite_police == NULL) {
        return;
    }

    int largeur_texte = 0;
    int hauteur_texte = 0;
    if (TTF_SizeUTF8(app->petite_police, texte, &largeur_texte, &hauteur_texte) != 0) {
        return;
    }

    int bx = app->souris_x + PLAN_BULLE_OFFSET_X;
    int by = app->souris_y + PLAN_BULLE_OFFSET_Y;
    int bw = largeur_texte + 2 * PLAN_BULLE_MARGE;
    int bh = hauteur_texte + 2 * PLAN_BULLE_MARGE;

    // On garde la bulle dans la fenêtre pour éviter les débordements
    if (bx + bw > largeur) {
        bx = largeur - bw - app->marge_plan;
    }
    if (by + bh > hauteur) {
        by = hauteur - bh - app->marge_plan;
    }
    if (bx < 0) bx = 0;
    if (by < 0) by = 0;

    SDL_Rect bulle = {bx, by, bw, bh};
    Couleur fond_bulle = COULEUR_PANNEAU_FOND;
    SDL_SetRenderDrawColor(app->moteurRendu, fond_bulle.r, fond_bulle.g, fond_bulle.b, fond_bulle.a);
    SDL_RenderFillRect(app->moteurRendu, &bulle);
    Couleur bordure_bulle = COULEUR_PANNEAU_BORDURE;
    SDL_SetRenderDrawColor(app->moteurRendu, bordure_bulle.r, bordure_bulle.g, bordure_bulle.b, bordure_bulle.a);
    SDL_RenderDrawRect(app->moteurRendu, &bulle);

    SDL_Color couleur_texte = COULEUR_SDL(COULEUR_TEXTE_INFO);
    gui_rendu_texte(app, app->petite_police, texte, bx + PLAN_BULLE_MARGE, by + PLAN_BULLE_MARGE, couleur_texte);
}

/**
 * Fonction qui convertit une couleur hexadécimale (#RRGGBB) en Couleur.
 * @param texte : Chaîne hexadécimale (avec ou sans #).
 * @param couleur : Sortie de la couleur convertie.
 * @return : 1 si la conversion est valide, 0 sinon.
 */
static int couleur_depuis_hex(const char *texte, Couleur *couleur) {
    if (texte == NULL) {
        return 0;
    }

    // On accepte les formats "#RRGGBB" ou "RRGGBB".
    const char *ptr = texte;
    if (texte[0] == '#') {
        ptr = texte + 1;
    }
    if (strlen(ptr) != 6) {
        return 0;
    }

    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;
    // "%2x" lit exactement 2 caractères hexadécimaux (RR, puis GG, puis BB).
    if (sscanf(ptr, "%2x%2x%2x", &r, &g, &b) != 3) {
        return 0;
    }

    // On affecte les composantes lues
    couleur->r = (unsigned char)r;
    couleur->g = (unsigned char)g;
    couleur->b = (unsigned char)b;
    couleur->a = 255;                   // Alpha opaque par défaut.
    return 1;
}

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
    if (app == NULL || app->liste_stations == NULL) {
        return;
    }

    // On calcule les limites géographiques à partir de la liste des stations
    Une_coord limite_no;
    Une_coord limite_se;
    limites_zone(app->liste_stations, &limite_no, &limite_se);

    float lon_min = limite_no.lon;
    float lon_max = limite_se.lon;
    float lat_min = limite_se.lat;
    float lat_max = limite_no.lat;

    // On vérifie les dimensions pour éviter les divisions par zéro
    float largeur_geo = lon_max - lon_min;
    float hauteur_geo = lat_max - lat_min;
    if (largeur_geo == 0.0f || hauteur_geo == 0.0f) {
        *x = largeur / 2;
        *y = hauteur / 2;
        return;
    }

    // On normalise les coordonnées dans l'intervalle [0,1].
    float nx = (coord.lon - lon_min) / largeur_geo;
    float ny = (lat_max - coord.lat) / hauteur_geo; // Inversion de Y pour l'écran

    // On applique la transformation vers l'écran en tenant compte de la marge
    float marge = (float)app->marge_plan;
    float largeur_util = (float)largeur - 2.0f * marge;
    float hauteur_util = (float)hauteur - 2.0f * marge;
    if (largeur_util <= 0.0f || hauteur_util <= 0.0f) {
        *x = largeur / 2;
        *y = hauteur / 2;
        return;
    }

    // On positionne le point dans la zone utile
    float px = marge + nx * largeur_util;
    float py = marge + ny * hauteur_util;

    // On applique le zoom et le décalage de l'affichage
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
void gui_dessiner_plan(Etat_affichage *app, int largeur, int hauteur) {
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

    // On dessine les connexions entre stations
    if (app->liste_connexions != NULL) {
        for (Un_elem *elem = app->liste_connexions; elem != NULL; elem = elem->suiv) {
            if (elem->truc == NULL || elem->truc->type != CON) {
                continue;
            }

            Un_truc *sta_dep = elem->truc->data.con.sta_dep;
            Un_truc *sta_arr = elem->truc->data.con.sta_arr;
            Une_ligne *ligne = elem->truc->data.con.ligne;
            if (sta_dep == NULL || sta_arr == NULL || ligne == NULL || ligne->color == NULL) {
                continue;
            }

            Couleur couleur_ligne;
            if (!couleur_depuis_hex(ligne->color, &couleur_ligne)) {
                couleur_ligne = COULEUR_GRILLE;
            }

            int x1 = 0;
            int y1 = 0;
            int x2 = 0;
            int y2 = 0;
            geo_vers_ecran(app, sta_dep->coord, largeur, hauteur, &x1, &y1);
            geo_vers_ecran(app, sta_arr->coord, largeur, hauteur, &x2, &y2);

            SDL_SetRenderDrawColor(app->moteurRendu, couleur_ligne.r, couleur_ligne.g, couleur_ligne.b, couleur_ligne.a);
            // SDL ne gère pas l'épaisseur nativement, on superpose plusieurs lignes.
            for (int offset = -PLAN_EPAISSEUR_LIGNE / 2; offset <= PLAN_EPAISSEUR_LIGNE / 2; offset++) {
                SDL_RenderDrawLine(app->moteurRendu, x1, y1 + offset, x2, y2 + offset);
            }
        }
    }

    // On dessine toutes les stations sous forme de petits carrés
    if (app->liste_stations != NULL) {
        Couleur couleur_station = COULEUR_STATION;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_station.r, couleur_station.g, couleur_station.b, couleur_station.a);

        for (Un_elem *elem = app->liste_stations; elem != NULL; elem = elem->suiv) {
            if (elem->truc == NULL || elem->truc->type != STA) {
                continue;
            }

            int px = 0;
            int py = 0;
            geo_vers_ecran(app, elem->truc->coord, largeur, hauteur, &px, &py);

            int demi_taille = PLAN_TAILLE_STATION / 2;
            SDL_Rect station = {px - demi_taille, py - demi_taille, PLAN_TAILLE_STATION, PLAN_TAILLE_STATION};
            SDL_RenderFillRect(app->moteurRendu, &station);
        }
    }

    // On met en évidence la station sélectionnée
    if (app->station_selectionnee != NULL) {
        int sx = 0;
        int sy = 0;
        geo_vers_ecran(app, app->station_selectionnee->coord, largeur, hauteur, &sx, &sy);

        Couleur couleur_sel = COULEUR_STATION_SEL;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_sel.r, couleur_sel.g, couleur_sel.b, couleur_sel.a);
        int demi_taille = PLAN_TAILLE_STATION_SEL / 2;
        SDL_Rect marqueur = {sx - demi_taille, sy - demi_taille, PLAN_TAILLE_STATION_SEL, PLAN_TAILLE_STATION_SEL};
        SDL_RenderDrawRect(app->moteurRendu, &marqueur);
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
    gui_rendu_texte(app, app->petite_police, "S/P mode  -  R réinitialiser  -  ESC menu  -  Q quitter", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_2, couleur_info);

    // Si l'utilisateur a cliqué, on affiche un curseur rouge à la position du clic
    if (app->dernier_clic_x >= 0 && app->dernier_clic_y >= 0) {
        int demi_taille = PLAN_CURSEUR_TAILLE / 2;
        SDL_Rect curseur = {app->dernier_clic_x - demi_taille, app->dernier_clic_y - demi_taille, PLAN_CURSEUR_TAILLE, PLAN_CURSEUR_TAILLE};
        Couleur couleur_curseur = COULEUR_CURSEUR;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_curseur.r, couleur_curseur.g, couleur_curseur.b, couleur_curseur.a);
        SDL_RenderFillRect(app->moteurRendu, &curseur);
    }

    // Si la souris survole une station, on affiche son nom près du curseur
    if (app->souris_x >= 0 && app->souris_y >= 0) {
        int sx = 0;
        int sy = 0;
        Un_truc *station = trouver_station_proche(
            app, largeur, hauteur, app->souris_x, app->souris_y, &sx, &sy, PLAN_DISTANCE_SURVOL
        );
        if (station != NULL && station->data.sta.nom != NULL) {
            dessiner_bulle_texte(app, station->data.sta.nom, largeur, hauteur);
        } else {
            Une_ligne *ligne = trouver_ligne_proche(app, largeur, hauteur);
            if (ligne != NULL && ligne->code != NULL) {
                char etiquette[64];
                snprintf(etiquette, sizeof(etiquette), "Ligne %s", ligne->code);
                dessiner_bulle_texte(app, etiquette, largeur, hauteur);
            }
        }
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
    } else if (key == SDLK_r) {
        app->zoom = 1.0f;
        app->decalage_x = 0.0f;
        app->decalage_y = 0.0f;
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

    // On tente de sélectionner la station la plus proche du clic
    int largeur = 0;
    int hauteur = 0;
    SDL_GetWindowSize(app->fenetre, &largeur, &hauteur);
    int sx = 0;
    int sy = 0;
    Un_truc *station = trouver_station_proche(app, largeur, hauteur, x, y, &sx, &sy, PLAN_DISTANCE_SELECTION);
    if (station != NULL) {
        if (station != app->station_selectionnee && station->data.sta.nom != NULL) {
            printf("Station sélectionnée : %s\n", station->data.sta.nom);
        }
        app->station_selectionnee = station;
    } else {
        if (app->station_selectionnee != NULL) {
            printf("Station sélectionnée : aucune\n");
        }
        app->station_selectionnee = NULL;
    }
}
