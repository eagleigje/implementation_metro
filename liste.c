/*
 * PROJET MAIN3 / METRO
 * Librairie permettant la manipulation de listes simplement chaînées de trucs.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "liste.h"
#include "csv_utils.h"
#include "constantes.h"

/**
 * Fonction qui insère "un truc" dans la liste. La liste est considérée déjà triée. On ajoute le nouvel élèment
 * directement au bon endroit en fonction de "user_val".
 * @param liste : La liste dans laquelle ajouter le truc.
 * @param truc : Le truc que l'on veut ajouter à la liste.
 * @return : Renvoi la liste modifiée.
 */
Un_elem *inserer_liste_trie(Un_elem *liste, Un_truc *truc) {
    // Allocation de la mémoire pour l'élément.
    Un_elem *nouveau = (Un_elem *)malloc(sizeof(Un_elem));
    if (nouveau == NULL) {
        // En cas d'erreur d'allocation, on renvoie juste la liste non modifiée.
        return liste;
    }
    
    nouveau->truc = truc;
    nouveau->suiv = NULL;
    
    // Si la liste est vide ou si le nouvel element doit etre en tête
    if (liste == NULL || truc->user_val < liste->truc->user_val) {
        nouveau->suiv = liste;
        return nouveau;
    }
    
    // On parcourt la liste pour trouver la position où insérer le nouvel élèment.
    Un_elem *courant = liste;
    while (courant->suiv != NULL && courant->suiv->truc->user_val < truc->user_val) {
        courant = courant->suiv;
    }
    // Et on recrée le chaînage.
    nouveau->suiv = courant->suiv;
    courant->suiv = nouveau;
    
    return liste;
}

/**
 * Fonction d'insertion d'un nouvel élèment en début de liste.
 * @param liste : La liste dans laquelle ajouter le truc.
 * @param truc : Le truc que l'on veut ajouter à la liste.
 * @return : Renvoi la liste modifiée.
 */
Un_elem *inserer_deb_liste(Un_elem *liste, Un_truc *truc) {
    Un_elem *nouveau = (Un_elem *)malloc(sizeof(Un_elem));
    if (nouveau == NULL) {
        return liste;
    }
    
    nouveau->truc = truc;
    nouveau->suiv = liste;
    
    return nouveau;
}

/**
 * Fonction qui affiche dans flux, la liste dans son intégralité.
 * @param flux : Le flux de sortie (stdout théoriquement).
 * @param liste : La liste donnée que l'on doit afficher.
 */
void ecrire_liste(FILE *flux, Un_elem *liste) {
    Un_elem *courant = liste;
    
    while (courant != NULL) {
        if (courant->truc->type == STA) {
            fprintf(flux, "%.6f ; %.6f ; %s\n", 
                    courant->truc->coord.lon,
                    courant->truc->coord.lat,
                    courant->truc->data.sta.nom);
        }
        courant = courant->suiv;
    }
}

/**
 * Fonction de libération de la mémoire pour une liste donnée.
 * @param liste : La liste que l'on souhaite libérer.
 */
void detruire_liste(Un_elem *liste) {
    Un_elem *courant = liste;
    Un_elem *suivant;

    // On parcourt la liste, et on libère la mémoire au fur et à mesure.
    while (courant != NULL) {
        suivant = courant->suiv;
        free(courant);
        courant = suivant;
    }
}

/**
 * Fonction qui supprime en mémoire la liste et les trucs associés.
 * @param liste : La liste que l'on souhaite libérer.
 */
void detruire_liste_et_truc(Un_elem *liste) {
    // todo : La fonction précédente ne supprime pas les trucs mais à part ça est la même que celle-ci.
    Un_elem *courant = liste;
    Un_elem *suivant;

    // On parcourt la liste, et on libère la mémoire au fur et à mesure.
    while (courant != NULL) {
        suivant = courant->suiv;
        detruire_truc(courant->truc);
        free(courant);
        courant = suivant;
    }
}

/**
 * Fonction qui permet de déterminer les longitudes et latitudes minimales et maximales des stations.
 * @param liste : La liste des stations.
 * @param limite_no : La longitude minimale et la latitude maximale.
 * @param limite_se : La longitude maximale et à la latitude minimale.
 */
void limites_zone(Un_elem *liste, Une_coord *limite_no, Une_coord *limite_se) {
    if (liste == NULL) {
        return;
    }
    
    // On initialise avec les coordonnées du premier élèment.
    // no = Nord Ouest
    // se = Sud Est
    limite_no->lon = liste->truc->coord.lon;
    limite_no->lat = liste->truc->coord.lat;
    limite_se->lon = liste->truc->coord.lon;
    limite_se->lat = liste->truc->coord.lat;
    
    Un_elem *courant = liste->suiv;
    
    while (courant != NULL) {
        // Longitude minimale (NO)
        if (courant->truc->coord.lon < limite_no->lon) {
            limite_no->lon = courant->truc->coord.lon;
        }
        // Longitude maximale (SE)
        if (courant->truc->coord.lon > limite_se->lon) {
            limite_se->lon = courant->truc->coord.lon;
        }
        // Latitude maximale (NO)
        if (courant->truc->coord.lat > limite_no->lat) {
            limite_no->lat = courant->truc->coord.lat;
        }
        // Latitude minimale (SE)
        if (courant->truc->coord.lat < limite_se->lat) {
            limite_se->lat = courant->truc->coord.lat;
        }
        
        courant = courant->suiv;
    }
}

/**
 * Fonction qui lit le fichier CSV passé en paramètre et renvoi la liste triée des stations.
 * @param nom_fichier : Le fichier à lire.
 * @return : La liste triée, en fonction de la latitude, des stations du fichier.
 */
Un_elem *lire_stations(char *nom_fichier) {
    // Ouverture du fichier en lecture seule.
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }

    Un_elem *liste = NULL;
    char ligne[CSV_MAX_STATIONS_LENGTH];

    // Pour chaque ligne trouvée dans le fichier
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Un_lecteur_csv lecteur;
        csv_init_lecteur(&lecteur, ligne, CSV_SEPARATOR);

        // On lit la longitude
        if (!csv_champ_suivant(&lecteur)) continue;
        float lon = csv_lire_flottant(&lecteur);

        // La latitude
        if (!csv_champ_suivant(&lecteur)) continue;
        float lat = csv_lire_flottant(&lecteur);

        // On récupère le nom de la station
        if (!csv_champ_suivant(&lecteur)) continue;
        char *nom_ptr = csv_lire_texte(&lecteur, STATION_NOM_TAILLE_MAX);
        if (nom_ptr == NULL) continue;  // Champ trop grand, on ignore cette ligne
        char nom[STATION_NOM_TAILLE_MAX];
        strcpy(nom, nom_ptr);

        // On prépare le truc station
        Une_coord coord = {lon, lat};
        Tdata data;
        data.sta.nom = (char *)malloc(strlen(nom) + 1);
        strcpy(data.sta.nom, nom);
        data.sta.tab_con = NULL;
        data.sta.nb_con = 0;
        data.sta.con_pcc = NULL;

        // Et on crée la nouvelle station avec :
        // ses coordonnées, ses données et user_value qui vaut la latitude pour trier sur ce champ.
        Un_truc *truc = creer_truc(coord, STA, data, lat);
        // On insère la nouvelle station, directement triée grâce au champ user_value
        liste = inserer_liste_trie(liste, truc);
    }

    fclose(fichier);
    return liste;
}

/**
 * Fonction qui extrait le premier élèment de la liste passée en paramètre. On supprime l'élément extrait de la liste.
 * @param liste : La liste à parcourir.
 * @return : Le premier élément de la liste OU NULL si la liste est vide.
 */
Un_truc *extraire_deb_liste(Un_elem **liste) {
    if (liste == NULL || *liste == NULL) {
        return NULL;
    }
    
    Un_elem *premier = *liste;
    Un_truc *truc = premier->truc;
    *liste = premier->suiv;
    free(premier);
    
    return truc;
}

/**
 * Fonction qui extrait un élèment de la liste passée en paramètre. On supprime l'élément extrait de la liste.
 * @param liste : La liste à parcourir.
 * @param truc : Le truc à extraire.
 * @return : Le premier élément de la liste OU NULL si la liste est vide ou si l'élément n'est pas trouvé.
 */
Un_truc *extraire_liste(Un_elem **liste, Un_truc *truc) {
    if (liste == NULL || *liste == NULL || truc == NULL) {
        return NULL;
    }
    
    // Si c'est le premier élément de la liste.
    if ((*liste)->truc == truc) {
        return extraire_deb_liste(liste);
    }
    
    // Sinon, on cherche l'élément dans la liste.
    Un_elem *courant = *liste;
    while (courant->suiv != NULL && courant->suiv->truc != truc) {
        courant = courant->suiv;
    }
    
    if (courant->suiv == NULL) {
        return NULL; // L'élément n'a pas été trouvé.
    }

    // On récupère l'élément, et on le supprime de la liste chaînée.
    Un_elem *a_extraire = courant->suiv;
    courant->suiv = a_extraire->suiv;
    Un_truc *truc_extrait = a_extraire->truc;
    free(a_extraire);
    
    return truc_extrait;
}
