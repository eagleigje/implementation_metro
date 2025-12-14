/*
 *  PROJET MAIN3 / METRO
 *  Gestion des connexions entre stations.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "connexion.h"
#include "ligne.h"
#include "abr.h"
#include "liste.h"
#include "truc.h"
#include "constantes.h"
#include "csv_utils.h"

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

    // Application de la formule de Haversine : https://fr.wikipedia.org/wiki/Formule_de_haversine
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
static Un_truc *creer_connexion(char *ligne_str, Une_ligne *liste_ligne, Un_nabr *abr_sta) {
    Un_lecteur_csv lecteur;
    csv_init_lecteur(&lecteur, ligne_str, CSV_SEPARATOR);

    // Les champs sont extraits un à un de la ligne en cours du CSV lu.
    // On lit le code de la ligne
    if (!csv_champ_suivant(&lecteur)) return NULL;
    char *code_ptr = csv_lire_texte(&lecteur, LINE_MAX_CODE_SIZE);
    if (code_ptr == NULL) return NULL;  // Champ trop grand
    char code_ligne[LINE_MAX_CODE_SIZE];
    strcpy(code_ligne, code_ptr);

    // On lit le nom de la station de départ
    if (!csv_champ_suivant(&lecteur)) return NULL;
    char *nom_dep_ptr = csv_lire_texte(&lecteur, STATION_NOM_TAILLE_MAX);
    if (nom_dep_ptr == NULL) return NULL;  // Champ trop grand
    char nom_dep[STATION_NOM_TAILLE_MAX];
    strcpy(nom_dep, nom_dep_ptr);

    // On lit le nom de la station d'arrivée
    if (!csv_champ_suivant(&lecteur)) return NULL;
    char *nom_arr_ptr = csv_lire_texte(&lecteur, STATION_NOM_TAILLE_MAX);
    if (nom_arr_ptr == NULL) return NULL;  // Champ trop grand
    char nom_arr[STATION_NOM_TAILLE_MAX];
    strcpy(nom_arr, nom_arr_ptr);

    // On lit le temps de parcours
    if (!csv_champ_suivant(&lecteur)) return NULL;
    float temps = csv_lire_flottant(&lecteur);

    // On doit rechercher la ligne dans la liste afin de vérifier qu'elle existe bien.
    Une_ligne *ligne = chercher_ligne(liste_ligne, code_ligne);
    if (ligne == NULL) {
        fprintf(stderr, "Erreur: ligne '%s' non trouvée\n", code_ligne);
        return NULL;
    }

    // On doit rechercher les stations dans l'ABR afin de vérifier qu'elles existent bien dans l'arbre.
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

    // Si le temps est 0.0, on le calcule à partir de la vitesse et de la distance
    if (temps == 0.0) {
        float distance = calculer_distance(sta_dep->coord, sta_arr->coord);
        temps = calculer_temps(distance, ligne->vitesse);
    }

    // On prépare le truc connexion
    Tdata data;
    data.con.sta_dep = sta_dep;
    data.con.sta_arr = sta_arr;
    data.con.ligne = ligne;

    // Les coordonnées de la connexion sont celles du point milieu
    Une_coord coord_milieu;
    coord_milieu.lon = (sta_dep->coord.lon + sta_arr->coord.lon) / 2.0;
    coord_milieu.lat = (sta_dep->coord.lat + sta_arr->coord.lat) / 2.0;

    // Et on crée le truc avec le temps de parcours comme user_val
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

    // On parcourt toutes les lignes du fichier
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Un_truc *connexion = creer_connexion(ligne, liste_ligne, abr_sta);
        if (connexion != NULL) {
            // On insère en début de liste (pas besoin de tri pour les connexions).
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
