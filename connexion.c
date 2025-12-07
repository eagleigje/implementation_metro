/*
 *  PROJET MAIN3 / METRO
 *  Gestion des connexions entre stations.
*/

#include "connexion.h"
#include "ligne.h"
#include "abr.h"
#include "liste.h"
#include "truc.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STATION_NAME_SIZE 100 // todo : tout déplacer dans les constantes, il y a des doublons.

/**
 * Fonction qui calcule la distance entre deux coordonnées (formule de Haversine).
 * @param coord1 : Première coordonnée.
 * @param coord2 : Deuxième coordonnée.
 * @return : La distance en kilomètres.
 */
static float calculer_distance(Une_coord coord1, Une_coord coord2) {
    const float RAYON_TERRE = 6371.0; // Rayon moyen de la Terre en km

    // Conversion des degrés en radians
    float lat1_rad = coord1.lat * M_PI / 180.0;
    float lat2_rad = coord2.lat * M_PI / 180.0;
    float delta_lat = (coord2.lat - coord1.lat) * M_PI / 180.0;
    float delta_lon = (coord2.lon - coord1.lon) * M_PI / 180.0;

    // Formule de Haversine
    float a = sin(delta_lat / 2.0) * sin(delta_lat / 2.0) +
              cos(lat1_rad) * cos(lat2_rad) *
              sin(delta_lon / 2.0) * sin(delta_lon / 2.0);
    float c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return RAYON_TERRE * c;
}

/**
 * Fonction qui calcule le temps de parcours en minutes entre deux stations.
 * @param distance : La distance en km.
 * @param vitesse : La vitesse en km/h.
 * @return : Le temps de parcours en minutes.
 */
static float calculer_temps(float distance, float vitesse) {
    if (vitesse <= 0.0) {
        return 0.0;
    }
    // Temps = distance / vitesse, converti en minutes
    return (distance / vitesse) * 60.0;
}

/**
 * Fonction qui lit une ligne du fichier CSV des connexions et crée une connexion.
 * @param ligne_str : La ligne au format texte à gérer.
 * @param liste_ligne : La liste des lignes de métro.
 * @param abr_sta : L'arbre binaire de recherche des stations.
 * @return : Pointeur vers le nouveau truc connexion ou NULL en cas d'erreur.
 */
static Un_truc *creer_connexion(char ligne_str[CSV_MAX_CONNECTIONS_LENGTH], Une_ligne *liste_ligne, Un_nabr *abr_sta) {
    char code_ligne[LINE_MAX_CODE_SIZE];
    char nom_dep[STATION_NAME_SIZE];
    char nom_arr[STATION_NAME_SIZE];
    float temps;

    // On pointe en début de la ligne récupérée.
    char *ptr = ligne_str;

    // Ignorer les lignes vides ou les commentaires
    while (*ptr == ' ') ptr++;
    if (*ptr == '\0' || *ptr == '\n' || *ptr == '#') {
        return NULL;
    }

    // Lecture du code de la ligne
    int i = 0;
    while (*ptr != CSV_SEPARATOR && *ptr != '\0' && i < LINE_MAX_CODE_SIZE - 1) {
        code_ligne[i++] = *ptr++;
    }
    code_ligne[i] = '\0';
    // Enlever les espaces de fin
    while (i > 0 && code_ligne[i-1] == ' ') code_ligne[--i] = '\0';

    // Si nous ne sommes pas sur un séparateur, la ligne dans le fichier est incorrecte.
    if (*ptr != CSV_SEPARATOR) return NULL;
    ptr++;

    // Lecture du nom de la station de départ
    while (*ptr == ' ') ptr++;
    i = 0;
    while (*ptr != CSV_SEPARATOR && *ptr != '\0' && i < STATION_NAME_SIZE - 1) {
        nom_dep[i++] = *ptr++;
    }
    nom_dep[i] = '\0';
    // Enlever les espaces de fin
    while (i > 0 && nom_dep[i-1] == ' ') nom_dep[--i] = '\0';

    if (*ptr != CSV_SEPARATOR) return NULL;
    ptr++;

    // Lecture du nom de la station d'arrivée
    while (*ptr == ' ') ptr++;
    i = 0;
    while (*ptr != CSV_SEPARATOR && *ptr != '\0' && i < STATION_NAME_SIZE - 1) {
        nom_arr[i++] = *ptr++;
    }
    nom_arr[i] = '\0';
    // Enlever les espaces de fin
    while (i > 0 && nom_arr[i-1] == ' ') nom_arr[--i] = '\0';

    if (*ptr != CSV_SEPARATOR) return NULL;
    ptr++;

    // Lecture du temps de parcours
    while (*ptr == ' ') ptr++;
    if (sscanf(ptr, "%f", &temps) != 1) return NULL;

    // Rechercher la ligne dans la liste
    Une_ligne *ligne = chercher_ligne(liste_ligne, code_ligne);
    if (ligne == NULL) {
        fprintf(stderr, "Erreur: ligne '%s' non trouvée\n", code_ligne);
        return NULL;
    }

    // Rechercher les stations dans l'ABR
    Un_truc *sta_dep = chercher_station(abr_sta, nom_dep);
    if (sta_dep == NULL) {
        fprintf(stderr, "Erreur: station de départ '%s' non trouvée\n", nom_dep);
        return NULL;
    }

    Un_truc *sta_arr = chercher_station(abr_sta, nom_arr);
    if (sta_arr == NULL) {
        fprintf(stderr, "Erreur: station d'arrivée '%s' non trouvée\n", nom_arr);
        return NULL;
    }

    // Si le temps est 0.0, le calculer à partir de la vitesse et de la distance
    if (temps == 0.0) {
        float distance = calculer_distance(sta_dep->coord, sta_arr->coord);
        temps = calculer_temps(distance, ligne->vitesse);
    }

    // Créer le truc connexion
    Tdata data;
    data.con.sta_dep = sta_dep;
    data.con.sta_arr = sta_arr;
    data.con.ligne = ligne;

    // Les coordonnées de la connexion sont celles du point milieu
    Une_coord coord_milieu;
    coord_milieu.lon = (sta_dep->coord.lon + sta_arr->coord.lon) / 2.0;
    coord_milieu.lat = (sta_dep->coord.lat + sta_arr->coord.lat) / 2.0;

    // Créer le truc avec le temps de parcours comme user_val
    Un_truc *connexion = creer_truc(coord_milieu, CON, data, temps);

    return connexion;
}

/**
 * Fonction qui lit le fichier CSV des connexions et construit la liste de connexions.
 * @param nom_fichier : Le fichier CSV des connexions, à lire.
 * @param liste_ligne : La liste des lignes de métro.
 * @param abr_sta : L'arbre binaire de recherche des stations.
 * @return : Renvoi la liste des connexions, ou NULL en cas d'erreur.
 */
Un_elem *lire_connexions(char *nom_fichier, Une_ligne *liste_ligne, Un_nabr *abr_sta) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }

    Un_elem *liste_con = NULL;
    char ligne[CSV_MAX_CONNECTIONS_LENGTH];

    // Lire toutes les lignes du fichier
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Un_truc *connexion = creer_connexion(ligne, liste_ligne, abr_sta);
        if (connexion != NULL) {
            // Insérer en début de liste (pas besoin de tri pour les connexions)
            liste_con = inserer_deb_liste(liste_con, connexion);
        }
    }

    fclose(fichier);
    return liste_con;
}

/**
 * Fonction qui affiche sur un flux la liste des connexions.
 * @param flux : Le flux de sortie (stdout généralement).
 * @param liste_con : La liste des connexions à afficher.
 */
void ecrire_connexions(FILE *flux, Un_elem *liste_con) {
    Un_elem *ptr = liste_con;

    while (ptr != NULL) {
        if (ptr->truc->type == CON) {
            Une_connexion *con = &(ptr->truc->data.con);
            fprintf(flux, "%s ; %s ; %s ; %.1f\n",
                    con->ligne->code,
                    con->sta_dep->data.sta.nom,
                    con->sta_arr->data.sta.nom,
                    ptr->truc->user_val);
        }
        ptr = ptr->suiv;
    }
}
