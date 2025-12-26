/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'écran plan de l'interface graphique SDL2.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "gui_plan.h"
#include "gui_commun.h"
#include "../dijkstra.h"
#include "../abr.h"
#include "../ligne.h"

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
 * Fonction qui convertit une position écran en coordonnée géographique.
 * Cette opération est l'inverse de geo_vers_ecran :
 * - annule le zoom et le décalage de la vue,
 * - retire la marge d'affichage,
 * - repasse en coordonnées normalisées [0,1],
 * - recompose longitude/latitude à partir des limites du plan.
 *
 * @param app : État global de l'application graphique.
 * @param x : Position X en pixels.
 * @param y : Position Y en pixels.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 * @param coord : Sortie de la coordonnée géographique.
 * @return : 1 si la conversion est valide, 0 sinon.
 */
static int ecran_vers_geo(Etat_affichage *app, int x, int y, int largeur, int hauteur, Une_coord *coord) {
    if (app == NULL || app->liste_stations == NULL || coord == NULL) {
        return 0;
    }

    Une_coord limite_no;
    Une_coord limite_se;
    limites_zone(app->liste_stations, &limite_no, &limite_se);

    float lon_min = limite_no.lon;
    float lon_max = limite_se.lon;
    float lat_min = limite_se.lat;
    float lat_max = limite_no.lat;

    // Dimensions géographiques utiles (évite divisions par zéro).
    float largeur_geo = lon_max - lon_min;
    float hauteur_geo = lat_max - lat_min;
    if (largeur_geo == 0.0f || hauteur_geo == 0.0f) {
        return 0;
    }

    // Zone utile de dessin (on retire la marge du plan).
    float marge = (float)app->marge_plan;
    float largeur_util = (float)largeur - 2.0f * marge;
    float hauteur_util = (float)hauteur - 2.0f * marge;
    if (largeur_util <= 0.0f || hauteur_util <= 0.0f) {
        return 0;
    }

    if (app->zoom == 0.0f) {
        return 0;
    }

    // On annule le zoom et le décalage pour revenir aux coordonnées écran de base.
    float px = ((float)x - largeur / 2.0f - app->decalage_x) / app->zoom + largeur / 2.0f;
    float py = ((float)y - hauteur / 2.0f - app->decalage_y) / app->zoom + hauteur / 2.0f;

    // On convertit les pixels en coordonnées normalisées [0,1] dans la zone utile.
    float nx = (px - marge) / largeur_util;
    float ny = (py - marge) / hauteur_util;

    // On reconstruit lon/lat depuis les limites (Y inversé par rapport à l'écran).
    coord->lon = lon_min + nx * largeur_geo;
    coord->lat = lat_max - ny * hauteur_geo;
    return 1;
}

/**
 * Fonction qui actualise la recherche de station par préfixe.
 * @param app : État global de l'application graphique.
 */
void gui_plan_actualiser_recherche(Etat_affichage *app) {
    if (app == NULL || app->abr_stations == NULL) {
        return;
    }

    if (strlen(app->recherche_texte) > 0) {
        app->recherche_resultat = chercher_station_prefixe(app->abr_stations, app->recherche_texte);
    } else {
        app->recherche_resultat = NULL;
    }
}

/**
 * Fonction qui calcule la distance au carré entre un point et un segment.
 * Utilise la projection vectorielle pour déterminer le point le plus proche sur le segment.
 *
 * @param px : Coordonnée X du point.
 * @param py : Coordonnée Y du point.
 * @param x1 : Coordonnée X du début du segment.
 * @param y1 : Coordonnée Y du début du segment.
 * @param x2 : Coordonnée X de la fin du segment.
 * @param y2 : Coordonnée Y de la fin du segment.
 * @return : Distance au carré entre le point et le segment.
 */
static float distance_segment_carre(int px, int py, int x1, int y1, int x2, int y2) {
    // Vecteur du segment : v = (x2-x1, y2-y1)
    float vecteur_segment_x = (float)(x2 - x1);
    float vecteur_segment_y = (float)(y2 - y1);

    // Vecteur du début du segment au point : w = (px - x1, py - y1)
    float vecteur_point_x = (float)(px - x1);
    float vecteur_point_y = (float)(py - y1);

    // Produit scalaire w·v (projection du point sur la droite du segment)
    float produit_scalaire = vecteur_segment_x * vecteur_point_x + vecteur_segment_y * vecteur_point_y;

    // CAS 1 : La projection tombe avant le début du segment
    // Si le produit scalaire est négatif ou nul, le point le plus proche est le début du segment
    if (produit_scalaire <= 0.0f) {
        float dx = (float)(px - x1);
        float dy = (float)(py - y1);
        return dx * dx + dy * dy;  // Distance au carré au point de départ
    }

    // Norme au carré du vecteur segment : ||v||²
    float norme_segment_carre = vecteur_segment_x * vecteur_segment_x + vecteur_segment_y * vecteur_segment_y;

    // CAS 2 : La projection tombe après la fin du segment
    // Si le produit scalaire est supérieur à ||v||², le point le plus proche est la fin du segment
    if (norme_segment_carre <= produit_scalaire) {
        float dx = (float)(px - x2);
        float dy = (float)(py - y2);
        return dx * dx + dy * dy;  // Distance au carré au point d'arrivée
    }

    // CAS 3 : La projection tombe sur le segment
    // On calcule les coordonnées de la projection perpendiculaire
    float parametre_projection = produit_scalaire / norme_segment_carre;
    float projection_x = (float)x1 + parametre_projection * vecteur_segment_x;
    float projection_y = (float)y1 + parametre_projection * vecteur_segment_y;

    // Distance au carré à la projection perpendiculaire
    float dx = (float)px - projection_x;
    float dy = (float)py - projection_y;
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

    // On travaille en distance au carré pour éviter la racine carrée.
    int seuil = PLAN_DISTANCE_SURVOL_LIGNE;
    float meilleur = (float)(seuil * seuil) + 1.0f;
    Une_ligne *ligne_proche = NULL;

    // On parcourt toutes les connexions et on mesure la distance souris/segment.
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

        // Distance souris/segment en pixels (au carré).
        float dist2 = distance_segment_carre(app->souris_x, app->souris_y, x1, y1, x2, y2);
        if (dist2 < meilleur) {
            meilleur = dist2;
            ligne_proche = ligne;
        }
    }

    // Si la ligne la plus proche est au-delà du seuil, on ne renvoie rien.
    if (meilleur > (float)(seuil * seuil)) {
        return NULL;
    }

    return ligne_proche;
}

/**
 * Fonction qui recherche la station la plus proche de la souris.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 * @param x_ref : Position X de référence (souris ou clic).
 * @param y_ref : Position Y de référence (souris ou clic).
 * @param px : Sortie de la position X de la station la plus proche.
 * @param py : Sortie de la position Y de la station la plus proche.
 * @param distance_max : Rayon maximal en pixels pour accepter une station.
 * @return : Station la plus proche ou NULL si aucune n'est à portée.
 */
static Un_truc *trouver_station_proche(Etat_affichage *app, int largeur, int hauteur,
                                       int x_ref, int y_ref, int *px, int *py, int distance_max) {
    if (app == NULL || app->liste_stations == NULL) {
        return NULL;
    }

    static int info_aqr_loggee = 0;
    // Seuil de recherche en pixels (distance au carré pour éviter les racines).
    int seuil = distance_max;
    int meilleur = seuil * seuil + 1;
    Un_truc *station_proche = NULL;

    // Si l'AQR est disponible, on cherche un candidat proche en coordonnées géographiques.
    // On valide ensuite en pixels pour éviter les erreurs dues au zoom/décalage.
    if (app->aqr_topo != NULL) {
        if (!info_aqr_loggee) {
            printf("Sélection : AQR activé pour la recherche de station.\n");
            info_aqr_loggee = 1;
        }
        Une_coord coord_ref;
        if (ecran_vers_geo(app, x_ref, y_ref, largeur, hauteur, &coord_ref)) {
            Un_truc *candidat = chercher_aqr(app->aqr_topo, coord_ref);
            if (candidat != NULL) {
                // Vérification finale en pixels : la station doit être dans le rayon demandé.
                int sx = 0;
                int sy = 0;
                geo_vers_ecran(app, candidat->coord, largeur, hauteur, &sx, &sy);
                int dx = x_ref - sx;
                int dy = y_ref - sy;
                int dist2 = dx * dx + dy * dy;
                if (dist2 <= meilleur) {
                    station_proche = candidat;
                    *px = sx;
                    *py = sy;
                    return station_proche;
                }
                // Candidat trop loin : on abandonne sans parcourir la liste complète.
                return NULL;
            }
        }
    } else {
        if (!info_aqr_loggee) {
            printf("Sélection : AQR indisponible, recherche par liste.\n");
            info_aqr_loggee = 1;
        }
    }

    // Si l'AQR n'est pas disponible ou n'a rien trouvé, on parcourt toutes les stations.
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
 * La bulle est dimensionnée selon le texte et reste dans la fenêtre.
 * Un fond et une bordure sont appliqués pour garder la lecture claire.
 *
 * @param app : État global de l'application graphique.
 * @param texte : Texte à afficher.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
static void dessiner_bulle_texte(Etat_affichage *app, const char *texte, int largeur, int hauteur) {
    if (app == NULL || texte == NULL || app->petite_police == NULL) {
        return;
    }

    // On mesure le texte pour calculer la taille de la bulle.
    int largeur_texte = 0;
    int hauteur_texte = 0;
    if (TTF_SizeUTF8(app->petite_police, texte, &largeur_texte, &hauteur_texte) != 0) {
        return;
    }

    // Position de départ près du curseur.
    int bx = app->souris_x + PLAN_BULLE_OFFSET_X;
    int by = app->souris_y + PLAN_BULLE_OFFSET_Y;
    int bw = largeur_texte + 2 * PLAN_BULLE_MARGE;
    int bh = hauteur_texte + 2 * PLAN_BULLE_MARGE;

    // On garde la bulle dans la fenêtre pour éviter les débordements.
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

    // On dessine le texte dans la bulle avec une marge interne.
    SDL_Color couleur_texte = COULEUR_SDL(COULEUR_TEXTE_INFO);
    gui_rendu_texte(app, app->petite_police, texte, bx + PLAN_BULLE_MARGE, by + PLAN_BULLE_MARGE, couleur_texte);
}

/**
 * Fonction qui affiche un chemin en console sous forme de segments.
 * Les connexions consécutives d'une même ligne sont regroupées en un seul
 * segment "Ligne X de A à B" pour éviter la redondance.
 *
 * @param chemin : Liste de connexions constituant le chemin.
 */
static void afficher_chemin_console(Un_elem *chemin) {
    if (chemin == NULL) {
        return;
    }

    printf("Chemin (segments) :\n");
    float temps_total = 0.0f;
    const char *ligne_courante = NULL;
    const char *station_debut = NULL;
    const char *station_fin = NULL;

    // On parcourt le chemin dans l'ordre et on fusionne par ligne.
    for (Un_elem *elem = chemin; elem != NULL; elem = elem->suiv) {
        if (elem->truc == NULL || elem->truc->type != CON) {
            continue;
        }
        Une_connexion *con = &elem->truc->data.con;
        if (con->ligne == NULL || con->ligne->code == NULL ||
            con->sta_dep == NULL || con->sta_arr == NULL ||
            con->sta_dep->data.sta.nom == NULL || con->sta_arr->data.sta.nom == NULL) {
            continue;
        }

        temps_total += elem->truc->user_val;

        if (ligne_courante == NULL) {
            // Premier segment.
            ligne_courante = con->ligne->code;
            station_debut = con->sta_dep->data.sta.nom;
            station_fin = con->sta_arr->data.sta.nom;
            continue;
        }

        if (strcmp(ligne_courante, con->ligne->code) == 0) {
            // Même ligne, on étend la station d'arrivée.
            station_fin = con->sta_arr->data.sta.nom;
        } else {
            // Changement de ligne, on affiche le segment précédent.
            printf("  Ligne %s de %s à %s\n", ligne_courante, station_debut, station_fin);
            ligne_courante = con->ligne->code;
            station_debut = con->sta_dep->data.sta.nom;
            station_fin = con->sta_arr->data.sta.nom;
        }
    }

    // Dernier segment à afficher.
    if (ligne_courante != NULL && station_debut != NULL && station_fin != NULL) {
        printf("  Ligne %s de %s à %s\n", ligne_courante, station_debut, station_fin);
    }
    // Temps total cumulé sur les connexions du chemin.
    printf("Temps total estimé : %.1f min\n", temps_total);
}

/**
 * Fonction qui construit un résumé du chemin pour l'affichage graphique.
 * Elle regroupe les connexions consécutives par ligne et ajoute une ligne
 * finale avec le temps total estimé.
 *
 * @param chemin : Liste des connexions du chemin.
 * @param lignes : Tableau de chaînes à remplir.
 * @param max_lignes : Capacité maximale du tableau.
 * @param temps_total : Sortie du temps total estimé.
 * @return : Nombre de lignes écrites dans le tableau.
 */
static int construire_resume_chemin(Un_elem *chemin, char lignes[][128], int max_lignes, float *temps_total) {
    if (chemin == NULL || lignes == NULL || max_lignes <= 0 || temps_total == NULL) {
        return 0;
    }

    // Initialisation des valeurs cumulées.
    *temps_total = 0.0f;
    int count = 0;
    const char *ligne_courante = NULL;
    const char *station_debut = NULL;
    const char *station_fin = NULL;

    // Parcours des connexions du chemin pour fusionner les segments par ligne.
    for (Un_elem *elem = chemin; elem != NULL; elem = elem->suiv) {
        if (elem->truc == NULL || elem->truc->type != CON) {
            continue;
        }
        Une_connexion *con = &elem->truc->data.con;
        if (con->ligne == NULL || con->ligne->code == NULL ||
            con->sta_dep == NULL || con->sta_arr == NULL ||
            con->sta_dep->data.sta.nom == NULL || con->sta_arr->data.sta.nom == NULL) {
            continue;
        }

        // Temps cumulé à partir des temps de connexion.
        *temps_total += elem->truc->user_val;

        if (ligne_courante == NULL) {
            // Premier segment rencontré.
            ligne_courante = con->ligne->code;
            station_debut = con->sta_dep->data.sta.nom;
            station_fin = con->sta_arr->data.sta.nom;
            continue;
        }

        if (strcmp(ligne_courante, con->ligne->code) == 0) {
            // Même ligne : on prolonge la station de fin.
            station_fin = con->sta_arr->data.sta.nom;
        } else {
            // Changement de ligne : on écrit la ligne précédente si possible.
            if (count < max_lignes) {
                snprintf(lignes[count], 128, "Ligne %s de %s à %s", ligne_courante, station_debut, station_fin);
                count++;
            }
            ligne_courante = con->ligne->code;
            station_debut = con->sta_dep->data.sta.nom;
            station_fin = con->sta_arr->data.sta.nom;
        }
    }

    // On écrit le dernier segment restant.
    if (ligne_courante != NULL && station_debut != NULL && station_fin != NULL) {
        if (count < max_lignes) {
            snprintf(lignes[count], 128, "Ligne %s de %s à %s", ligne_courante, station_debut, station_fin);
            count++;
        }
    }

    // Ajout d'une ligne de synthèse avec le temps total estimé.
    if (count < max_lignes) {
        snprintf(lignes[count], 128, "Temps total estimé : %.1f min", *temps_total);
        count++;
    }

    return count;
}

/**
 * Fonction qui dessine un encart résumant le chemin courant.
 * La taille de l'encart dépend du texte à afficher et il est placé en bas à droite.
 * L'encart n'est affiché que si un départ et une arrivée sont définis.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
static void dessiner_encart_chemin(Etat_affichage *app, int largeur, int hauteur) {
    if (app == NULL || app->chemin_courant == NULL || app->petite_police == NULL) {
        return;
    }
    if (app->station_depart == NULL || app->station_arrivee == NULL) {
        return;
    }

    // On prépare le résumé des segments.
    char lignes[32][128];
    float temps_total = 0.0f;
    int nb_lignes = construire_resume_chemin(app->chemin_courant, lignes, 32, &temps_total);
    if (nb_lignes == 0) {
        return;
    }

    // Mesure de la taille du texte pour dimensionner l'encart.
    int largeur_texte_max = 0;
    int hauteur_texte = 0;
    for (int i = 0; i < nb_lignes; i++) {
        int w = 0;
        int h = 0;
        if (TTF_SizeUTF8(app->petite_police, lignes[i], &w, &h) == 0) {
            if (w > largeur_texte_max) {
                largeur_texte_max = w;
            }
            if (h > hauteur_texte) {
                hauteur_texte = h;
            }
        }
    }

    int hauteur_totale = nb_lignes * hauteur_texte + (nb_lignes - 1) * PLAN_ENCART_ECART_LIGNE;
    int bw = largeur_texte_max + 2 * PLAN_ENCART_MARGE;
    int bh = hauteur_totale + 2 * PLAN_ENCART_MARGE;

    // Position en bas à droite avec marge, sans sortir de l'écran.
    int bx = largeur - bw - app->marge_plan;
    int by = hauteur - bh - app->marge_plan;
    if (bx < 0) bx = 0;
    if (by < 0) by = 0;

    SDL_Rect encart = {bx, by, bw, bh};
    Couleur fond = COULEUR_PANNEAU_FOND;
    SDL_SetRenderDrawColor(app->moteurRendu, fond.r, fond.g, fond.b, fond.a);
    SDL_RenderFillRect(app->moteurRendu, &encart);
    Couleur bordure = COULEUR_PANNEAU_BORDURE;
    SDL_SetRenderDrawColor(app->moteurRendu, bordure.r, bordure.g, bordure.b, bordure.a);
    SDL_RenderDrawRect(app->moteurRendu, &encart);

    SDL_Color couleur_texte = COULEUR_SDL(COULEUR_TEXTE_INFO);
    int y = by + PLAN_ENCART_MARGE;
    for (int i = 0; i < nb_lignes; i++) {
        gui_rendu_texte(app, app->petite_police, lignes[i], bx + PLAN_ENCART_MARGE, y, couleur_texte);
        y += hauteur_texte + PLAN_ENCART_ECART_LIGNE;
    }
}

/**
 * Fonction qui convertit une couleur hexadécimale (#RRGGBB) en Couleur.
 * La composante alpha est forcée à 255 (opaque).
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

    // On affecte les composantes lues.
    couleur->r = (unsigned char)r;
    couleur->g = (unsigned char)g;
    couleur->b = (unsigned char)b;
    couleur->a = 255;                   // Alpha opaque par défaut.
    return 1;
}

/**
 * Fonction qui centre la vue sur une station donnée.
 * Le décalage est recalculé pour placer la station au centre de l'écran.
 * @param app : État global de l'application graphique.
 * @param station : Station sur laquelle centrer la vue.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
static void centrer_sur_station(Etat_affichage *app, Un_truc *station, int largeur, int hauteur) {
    if (station == NULL || station->type != STA) {
        return;
    }

    // Réinitialiser les décalages pour calculer la position de base.
    app->decalage_x = 0.0f;
    app->decalage_y = 0.0f;

    int sx = 0;
    int sy = 0;
    geo_vers_ecran(app, station->coord, largeur, hauteur, &sx, &sy);

    // Calculer le décalage pour placer la station au centre.
    app->decalage_x = (largeur / 2) - sx * app->zoom;
    app->decalage_y = (hauteur / 2) - sy * app->zoom;
}

/**
 * Fonction qui dessine le panneau de recherche de station.
 * Le panneau affiche la saisie en cours et le résultat s'il existe.
 * @param app : État global de l'application graphique.
 */
static void dessiner_panneau_recherche(Etat_affichage *app) {
    if (!app->recherche_active) {
        return;
    }

    // Dessiner le panneau de recherche.
    SDL_Rect panneau = {PLAN_RECHERCHE_X, PLAN_RECHERCHE_Y, PLAN_RECHERCHE_LARGEUR, PLAN_RECHERCHE_HAUTEUR};
    Couleur fond = COULEUR_PANNEAU_FOND;
    SDL_SetRenderDrawColor(app->moteurRendu, fond.r, fond.g, fond.b, fond.a);
    SDL_RenderFillRect(app->moteurRendu, &panneau);
    Couleur bordure = COULEUR_PANNEAU_BORDURE;
    SDL_SetRenderDrawColor(app->moteurRendu, bordure.r, bordure.g, bordure.b, bordure.a);
    SDL_RenderDrawRect(app->moteurRendu, &panneau);

    SDL_Color couleur_texte = COULEUR_SDL(COULEUR_TEXTE_INFO);

    // Afficher le label.
    gui_rendu_texte(app, app->petite_police, "Recherche (ESC pour annuler) :",
                    panneau.x + PLAN_MARGE_TEXTE_X, panneau.y + PLAN_MARGE_TEXTE_Y_1, couleur_texte);

    // Afficher le texte de recherche et, si possible, le résultat.
    char texte_affiche[PLAN_RECHERCHE_MAX_TEXTE + 30];  // Un peu plus grand pour ne pas couper.
    if (strlen(app->recherche_texte) > 0) {
        if (app->recherche_resultat != NULL) {
            snprintf(texte_affiche, sizeof(texte_affiche), "%s → %s",
                     app->recherche_texte, app->recherche_resultat->data.sta.nom);
        } else {
            snprintf(texte_affiche, sizeof(texte_affiche), "%s (aucune station)", app->recherche_texte);
        }
    } else {
        snprintf(texte_affiche, sizeof(texte_affiche), "_");
    }
    gui_rendu_texte(app, app->petite_police, texte_affiche,
                    panneau.x + PLAN_MARGE_TEXTE_X, panneau.y + PLAN_MARGE_TEXTE_Y_2, couleur_texte);
}

/**
 * Fonction qui dessine l'écran plan avec grille et panneau d'information.
 * L'ordre de rendu est important pour garder les éléments lisibles :
 * fond -> grille -> connexions -> stations -> surbrillances -> panneaux.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_dessiner_plan(Etat_affichage *app, int largeur, int hauteur) {
    // On efface l'écran avec la couleur de fond.
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

    // On dessine les connexions entre stations.
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

    // On dessine toutes les stations sous forme de petits carrés.
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

    // On met en évidence la station sélectionnée.
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

    // On dessine le chemin courant s'il existe.
    if (app->chemin_courant != NULL) {
        Couleur couleur_chemin = COULEUR_STATION_SEL;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_chemin.r, couleur_chemin.g, couleur_chemin.b, couleur_chemin.a);
        for (Un_elem *elem = app->chemin_courant; elem != NULL; elem = elem->suiv) {
            if (elem->truc == NULL || elem->truc->type != CON) {
                continue;
            }
            Un_truc *sta_dep = elem->truc->data.con.sta_dep;
            Un_truc *sta_arr = elem->truc->data.con.sta_arr;
            if (sta_dep == NULL || sta_arr == NULL) {
                continue;
            }
            int x1 = 0;
            int y1 = 0;
            int x2 = 0;
            int y2 = 0;
            geo_vers_ecran(app, sta_dep->coord, largeur, hauteur, &x1, &y1);
            geo_vers_ecran(app, sta_arr->coord, largeur, hauteur, &x2, &y2);
            for (int offset = -PLAN_EPAISSEUR_LIGNE / 2; offset <= PLAN_EPAISSEUR_LIGNE / 2; offset++) {
                SDL_RenderDrawLine(app->moteurRendu, x1, y1 + offset, x2, y2 + offset);
            }
        }
    }

    // On met en évidence la station de départ et d'arrivée en mode chemin.
    if (app->station_depart != NULL) {
        int sx = 0;
        int sy = 0;
        geo_vers_ecran(app, app->station_depart->coord, largeur, hauteur, &sx, &sy);
        Couleur couleur_sel = COULEUR_STATION_SEL;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_sel.r, couleur_sel.g, couleur_sel.b, couleur_sel.a);
        int demi_taille = PLAN_TAILLE_STATION_SEL / 2;
        SDL_Rect marqueur = {sx - demi_taille, sy - demi_taille, PLAN_TAILLE_STATION_SEL, PLAN_TAILLE_STATION_SEL};
        SDL_RenderDrawRect(app->moteurRendu, &marqueur);
    }
    if (app->station_arrivee != NULL) {
        int sx = 0;
        int sy = 0;
        geo_vers_ecran(app, app->station_arrivee->coord, largeur, hauteur, &sx, &sy);
        Couleur couleur_sel = COULEUR_STATION_SEL;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_sel.r, couleur_sel.g, couleur_sel.b, couleur_sel.a);
        int demi_taille = PLAN_TAILLE_STATION_SEL / 2;
        SDL_Rect marqueur = {sx - demi_taille, sy - demi_taille, PLAN_TAILLE_STATION_SEL, PLAN_TAILLE_STATION_SEL};
        SDL_RenderDrawRect(app->moteurRendu, &marqueur);
    }

    // On dessine le panneau d'information en haut à gauche.
    SDL_Rect panneau_info = {PLAN_PANNEAU_X, PLAN_PANNEAU_Y, PLAN_PANNEAU_LARGEUR, PLAN_PANNEAU_HAUTEUR};
    Couleur fond_panneau = COULEUR_PANNEAU_FOND;
    SDL_SetRenderDrawColor(app->moteurRendu, fond_panneau.r, fond_panneau.g, fond_panneau.b, fond_panneau.a);
    SDL_RenderFillRect(app->moteurRendu, &panneau_info);
    Couleur bordure_panneau = COULEUR_PANNEAU_BORDURE;
    SDL_SetRenderDrawColor(app->moteurRendu, bordure_panneau.r, bordure_panneau.g, bordure_panneau.b, bordure_panneau.a);
    SDL_RenderDrawRect(app->moteurRendu, &panneau_info);

    // On affiche le mode actif dans le panneau.
    SDL_Color couleur_info = COULEUR_SDL(COULEUR_TEXTE_INFO);
    if (app->mode == MODE_SELECTION) {
        gui_rendu_texte(app, app->petite_police, "Mode : SÉLECTION", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_1, couleur_info);
    } else {
        gui_rendu_texte(app, app->petite_police, "Mode : PLUS COURT CHEMIN", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_1, couleur_info);
    }
    // On affiche les raccourcis clavier.
    gui_rendu_texte(app, app->petite_police, "S/P mode  -  F rechercher  -  R réinit  -  ESC menu  -  Q quitter", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_2, couleur_info);

    // Si l'utilisateur a cliqué, on affiche un curseur rouge à la position du clic.
    if (app->dernier_clic_x >= 0 && app->dernier_clic_y >= 0) {
        int demi_taille = PLAN_CURSEUR_TAILLE / 2;
        SDL_Rect curseur = {app->dernier_clic_x - demi_taille, app->dernier_clic_y - demi_taille, PLAN_CURSEUR_TAILLE, PLAN_CURSEUR_TAILLE};
        Couleur couleur_curseur = COULEUR_CURSEUR;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_curseur.r, couleur_curseur.g, couleur_curseur.b, couleur_curseur.a);
        SDL_RenderFillRect(app->moteurRendu, &curseur);
    }

    // On affiche un encart récapitulatif du chemin si disponible.
    dessiner_encart_chemin(app, largeur, hauteur);

    // On affiche le panneau de recherche si actif.
    dessiner_panneau_recherche(app);

    // Si la souris survole une station, on affiche son nom près du curseur.
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
 * Priorité à la recherche quand elle est active, sinon gestion des raccourcis du plan.
 * @param app : État global de l'application graphique.
 * @param key : Code de la touche pressée.
 * @param running : Pointeur vers le drapeau d'exécution de la boucle principale.
 */
void gui_plan_gerer_clavier(Etat_affichage *app, SDL_Keycode key, int *running) {
    // Si la recherche est active, on gère d'abord les touches spéciales
    if (app->recherche_active) {
        if (key == SDLK_ESCAPE) {
            // Désactiver la recherche
            app->recherche_active = 0;
            app->recherche_texte[0] = '\0';
            app->recherche_resultat = NULL;
            SDL_StopTextInput();
            return;
        } else if (key == SDLK_BACKSPACE) {
            // Effacer le dernier caractère
            int len = strlen(app->recherche_texte);
            if (len > 0) {
                app->recherche_texte[len - 1] = '\0';
                // Rechercher la nouvelle station (recherche par préfixe)
                gui_plan_actualiser_recherche(app);
            }
            return;
        } else if (key == SDLK_RETURN) {
            // Valider la recherche et sélectionner la station trouvée
            if (app->recherche_resultat != NULL) {
                app->station_selectionnee = app->recherche_resultat;
                printf("Station trouvée : %s\n", app->recherche_resultat->data.sta.nom);

                // Centrer la vue sur la station trouvée
                int largeur = 0;
                int hauteur = 0;
                SDL_GetWindowSize(app->fenetre, &largeur, &hauteur);
                centrer_sur_station(app, app->recherche_resultat, largeur, hauteur);
            }
            app->recherche_active = 0;
            app->recherche_texte[0] = '\0';
            app->recherche_resultat = NULL;
            SDL_StopTextInput();
            return;
        }
        // En mode recherche, on ignore les autres touches
        return;
    }

    // On est sur le plan : touches ESC, F, S, P, Q, +/-, flèches.
    if (key == SDLK_ESCAPE) {
        app->ecran = ECRAN_MENU;
        gui_mettre_a_jour_titre(app);
    } else if (key == SDLK_f) {
        // Activer la recherche
        app->recherche_active = 1;
        app->recherche_ignorer_prochain = 1;  // Ignorer le 'f' qui sera capturé par SDL_TEXTINPUT
        app->recherche_texte[0] = '\0';
        app->recherche_resultat = NULL;
        SDL_StartTextInput();
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
        *running = 0;
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
 * La station la plus proche est sélectionnée, puis utilisée comme départ/arrivée
 * si l'on est en mode chemin.
 *
 * @param app : État global de l'application graphique.
 * @param x : Position X du clic.
 * @param y : Position Y du clic.
 */
void gui_plan_gerer_clic(Etat_affichage *app, int x, int y) {
    // On enregistre la position du clic pour afficher un curseur
    app->dernier_clic_x = x;
    app->dernier_clic_y = y;

    // On tente de sélectionner la station la plus proche du clic.
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

    // En mode chemin, on choisit d'abord le départ puis l'arrivée.
    if (app->mode == MODE_CHEMIN) {
        if (station == NULL) {
            return;
        }

        if (app->station_depart == NULL || (app->station_depart != NULL && app->station_arrivee != NULL)) {
            app->station_depart = station;
            app->station_arrivee = NULL;
            if (app->chemin_courant != NULL) {
                detruire_liste(app->chemin_courant);
                app->chemin_courant = NULL;
            }
            if (station->data.sta.nom != NULL) {
                printf("Station de départ : %s\n", station->data.sta.nom);
            }
        } else if (app->station_arrivee == NULL) {
            app->station_arrivee = station;
            if (station->data.sta.nom != NULL) {
                printf("Station d'arrivée : %s\n", station->data.sta.nom);
            }
            if (app->liste_stations != NULL && app->station_depart != NULL) {
                dijkstra(app->liste_stations, app->station_depart);
                if (app->chemin_courant != NULL) {
                    detruire_liste(app->chemin_courant);
                }
                app->chemin_courant = cherche_chemin(app->station_arrivee);
                if (app->chemin_courant == NULL) {
                    printf("Aucun chemin trouvé.\n");
                } else {
                    printf("Chemin calculé.\n");
                    afficher_chemin_console(app->chemin_courant);
                }
            }
        }
    }
}
