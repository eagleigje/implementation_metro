#ifndef CONFIG_H
#define CONFIG_H

typedef struct _une_config {
    char *fichier_stations;
    char *fichier_lignes;
    char *fichier_connexions;
    float seuil_distance_aqr;  // Seuil pour eviter coordonnees identiques dans AQR
    float temps_correspondance; // Temps moyen de correspondance en minutes
    float temps_arret;         // Temps d'arrêt en station en minutes
} Une_config;

// Prototypes des fonctions
Une_config *lire_config(const char *nom_fichier);
void afficher_config(const Une_config *config);
void detruire_config(Une_config *config);

#endif
