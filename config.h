/*
 * PROJET MAIN3 / METRO
 * Gestion des configurations du programme.
*/
#ifndef CONFIG_H
#define CONFIG_H

/**
 * Structure définissant une configuration utilisable par le projet.
 */
typedef struct _une_config {
    char *fichier_stations;     ///< Path vers le fichier csv des stations.
    char *fichier_lignes;       ///< Path vers le fichier csv des lignes.
    char *fichier_connexions;   ///< Path vers le fichier csv des connexions.
    float seuil_distance_aqr;   ///< Seuil pour éviter des coordonnées identiques dans AQR.
    float temps_correspondance; ///< Temps moyen de correspondance en minutes.
    float temps_arret;          ///< Temps d'arrêt moyen en station en minutes.
} Une_config;

// Prototypes des fonctions
Une_config *lire_config(const char *nom_fichier);
void afficher_config(const Une_config *config);
void detruire_config(Une_config *config);

#endif
