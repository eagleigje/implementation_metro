#include "liste.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Un_elem *inserer_liste_trie(Un_elem *liste, Un_truc *truc) {
    Un_elem *nouveau = (Un_elem *)malloc(sizeof(Un_elem));
    if (nouveau == NULL) {
        return liste;
    }
    
    nouveau->truc = truc;
    nouveau->suiv = NULL;
    
    // Si la liste est vide ou si le nouvel element doit etre en tête
    if (liste == NULL || truc->user_val < liste->truc->user_val) {
        nouveau->suiv = liste;
        return nouveau;
    }
    
    // Parcourir la liste pour trouver la position
    Un_elem *courant = liste;
    while (courant->suiv != NULL && courant->suiv->truc->user_val < truc->user_val) {
        courant = courant->suiv;
    }
    
    nouveau->suiv = courant->suiv;
    courant->suiv = nouveau;
    
    return liste;
}

Un_elem *inserer_deb_liste(Un_elem *liste, Un_truc *truc) {
    Un_elem *nouveau = (Un_elem *)malloc(sizeof(Un_elem));
    if (nouveau == NULL) {
        return liste;
    }
    
    nouveau->truc = truc;
    nouveau->suiv = liste;
    
    return nouveau;
}

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

void detruire_liste(Un_elem *liste) {
    Un_elem *courant = liste;
    Un_elem *suivant;
    
    while (courant != NULL) {
        suivant = courant->suiv;
        free(courant);
        courant = suivant;
    }
}

void detruire_liste_et_truc(Un_elem *liste) {
    Un_elem *courant = liste;
    Un_elem *suivant;
    
    while (courant != NULL) {
        suivant = courant->suiv;
        detruire_truc(courant->truc);
        free(courant);
        courant = suivant;
    }
}

void limites_zone(Un_elem *liste, Une_coord *limite_no, Une_coord *limite_se) {
    if (liste == NULL) {
        return;
    }
    
    // Initialiser avec les coordonnees du premier element
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

Un_elem *lire_stations(char *nom_fichier) {
    // Fonction qui lit le fichier en paramètre et créé une liste chaînée triée en fonction
    // de la latitude de la station.
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }
    
    Un_elem *liste = NULL;
    char ligne[256];
    
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        float lon, lat;
        
        // Remplacer les ';' par des '\0' pour delimiter les champs
        char *ptr = ligne;
        char *debut_nom = NULL;
        
        // Parser longitude
        if (sscanf(ptr, "%f", &lon) != 1) continue;
        
        // Trouver le premier ';'
        while (*ptr != ';' && *ptr != '\0') ptr++;
        if (*ptr == '\0') continue;
        ptr++; // Passer le ';'
        
        // Parser latitude
        while (*ptr == ' ') ptr++; // Ignorer les espaces
        if (sscanf(ptr, "%f", &lat) != 1) continue;
        
        // Trouver le deuxieme ';'
        while (*ptr != ';' && *ptr != '\0') ptr++;
        if (*ptr == '\0') continue;
        ptr++; // Passer le ';'
        
        // Parser nom
        while (*ptr == ' ') ptr++; // Ignorer les espaces
        debut_nom = ptr;
        
        // Enlever le '\n' final
        char *fin = debut_nom;
        while (*fin != '\0' && *fin != '\n') fin++;
        *fin = '\0';
        
        // Creer le truc station
        Une_coord coord = {lon, lat};
        Tdata data;
        data.sta.nom = (char *)malloc(strlen(debut_nom) + 1);
        strcpy(data.sta.nom, debut_nom);
        data.sta.tab_con = NULL;
        data.sta.nb_con = 0;
        data.sta.con_pcc = NULL;

        // Création de la nouvelle station avec :
        // ses coordonnées, ses données et user_value qui vaut la latitude pour trier sur le champ.
        Un_truc *truc = creer_truc(coord, STA, data, lat);
        // On Insère la nouvelle station, directement triée grâce au champ user_value
        liste = inserer_liste_trie(liste, truc);
    }
    
    fclose(fichier);
    return liste;
}

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

Un_truc *extraire_liste(Un_elem **liste, Un_truc *truc) {
    if (liste == NULL || *liste == NULL || truc == NULL) {
        return NULL;
    }
    
    // Si c'est le premier element
    if ((*liste)->truc == truc) {
        return extraire_deb_liste(liste);
    }
    
    // Chercher l'element
    Un_elem *courant = *liste;
    while (courant->suiv != NULL && courant->suiv->truc != truc) {
        courant = courant->suiv;
    }
    
    if (courant->suiv == NULL) {
        return NULL; // Element non trouve
    }
    
    Un_elem *a_extraire = courant->suiv;
    courant->suiv = a_extraire->suiv;
    Un_truc *truc_extrait = a_extraire->truc;
    free(a_extraire);
    
    return truc_extrait;
}
