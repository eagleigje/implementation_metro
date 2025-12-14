/*
 * PROJET MAIN3 / METRO
 * Gestion des configurations du programme.
*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Fonction utilitaire qui enlève les espaces au début et à la fin d'une chaîne de caractères.
 * @param str : La chaîne que l'on veut traiter.
 * @return : La chaîne modifiée.
 */
static char *trim(char *str)
{
    // On supprime les espaces au début de chaîne.
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) {
        return str;
    }
    // On supprime les espaces en fin de chaîne.
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    // Et on termine la chaîne correctement.
    *(end + 1) = '\0';

    return str;
}

/**
 * Fonction utilitaire pour dupliquer une chaîne de caractères.
 * @param s : La chaîne qui doit être dupliquée.
 * @return La chaîne dupliquée ou NULL en cas d'erreur.
 */
static char *strdup_custom(const char *s) {
    // Calcul de la taille de la nouvelle chaîne.
    size_t len = strlen(s) + 1;
    // Allocation de la nouvelle chaîne.
    char *new_str = (char *)malloc(len);
    if (new_str != NULL) {
        // Recopie de la chaîne d'origine dans la nouvelle.
        memcpy(new_str, s, len);
    }
    return new_str;
}

/**
 * Fonction qui permet de lire un fichier de configuration afin de pouvoir faire varier les différents paramètres.
 * Le format du fichier est une sorte de .ini.
 * @param nom_fichier : Le fichier à ouvrir.
 * @return La configuration récupérée dans le fichier ou NULL en cas d'erreur.
 */
Une_config *lire_config(const char *nom_fichier) {
    // Ouverture du fichier de configuration, en lecture seule.
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier de configuration %s\n", nom_fichier);
        return NULL;
    }

    // Allocation de la mémoire pour la configuration.
    Une_config *config = (Une_config *)malloc(sizeof(Une_config));
    if (config == NULL) {
        fprintf(stderr, "Erreur: impossible de charger le fichier de configuration %s\n", nom_fichier);
        fclose(fichier);
        return NULL;
    }

    // Initialisation avec des valeurs par défaut pour traiter les valeurs absentes plus loin.
    config->fichier_stations = NULL;
    config->fichier_lignes = NULL;
    config->fichier_connexions = NULL;
    config->temps_correspondance = 3.0f;
    config->temps_arret = 0.5f;

    char ligne[256];
    // Lecture du fichier de configuration, ligne par ligne.
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        // Enlever le retour à la ligne.
        ligne[strcspn(ligne, "\r\n")] = '\0';
        // Suppression des espaces en début et fin de chaîne.
        char *ligne_trim = trim(ligne);
        // On ignore les lignes vides et les commentaires, en se basant sur le premier caractère de la chaîne.
        if (ligne_trim[0] == '\0' || ligne_trim[0] == '#' || ligne_trim[0] == ';') {
            continue;
        }
        // On ignore les sections [section] car celle ne sont là que pour aider à remplir le fichier de configuration.
        if (ligne_trim[0] == '[') {
            continue;
        }
        // On cherche le signe '='.
        char *egal = strchr(ligne_trim, '=');
        if (egal == NULL) {
            continue;
        }
        // On sépare la clé de la valeur, grâce à '='.
        *egal = '\0';
        const char *cle = trim(ligne_trim); // La clé est à gauche du signe '='.
        char *valeur = trim(egal + 1);
        // On supprime les guillemets eventuels autour de la valeur, tout en la récupérant.
        if (valeur[0] == '"' || valeur[0] == '\'') {
            valeur++;
            const size_t len = strlen(valeur);
            if (len > 0 && (valeur[len-1] == '"' || valeur[len-1] == '\'')) {
                valeur[len-1] = '\0'; // La valeur à droite du signe '='.
            }
        }

        // Et enfin, on récupère les differentes clés pour les stocker dans la structure config.
        if (strcmp(cle, "fichier_stations") == 0) {
            if (config->fichier_stations != NULL) free(config->fichier_stations);
            config->fichier_stations = strdup_custom(valeur);
        } else if (strcmp(cle, "fichier_lignes") == 0) {
            if (config->fichier_lignes != NULL) free(config->fichier_lignes);
            config->fichier_lignes = strdup_custom(valeur);
        } else if (strcmp(cle, "fichier_connexions") == 0) {
            if (config->fichier_connexions != NULL) free(config->fichier_connexions);
            config->fichier_connexions = strdup_custom(valeur);
        } else if (strcmp(cle, "temps_correspondance") == 0) {
            config->temps_correspondance = atof(valeur);
        } else if (strcmp(cle, "temps_arret") == 0) {
            config->temps_arret = atof(valeur);
        }
    }
    fclose(fichier);

    // Verifier que les fichiers obligatoires au projet sont bien définis.
    // Sinon, on donne des valeurs par défaut.
    if (config->fichier_stations == NULL) {
        config->fichier_stations = strdup_custom("stations.csv");
    }
    if (config->fichier_lignes == NULL) {
        config->fichier_lignes = strdup_custom("lignes.csv");
    }
    if (config->fichier_connexions == NULL) {
        config->fichier_connexions = strdup_custom("connexions.csv");
    }

    return config;
}

/**
 * Fonction qui imprime simplement la configuration chargée, à l'écran.
 * @param config : La configuration à afficher.
 */
void afficher_config(const Une_config *config) {
    if (config == NULL) {
        printf("La configuration est vide.\n");
        return;
    }

    printf("=== CONFIGURATION ===\n");
    printf("Fichier stations     : %s\n", config->fichier_stations);
    printf("Fichier lignes       : %s\n", config->fichier_lignes);
    printf("Fichier connexions   : %s\n", config->fichier_connexions);
    printf("Temps correspondance : %.1f minutes\n", config->temps_correspondance);
    printf("Temps arret          : %.1f minutes\n", config->temps_arret);
    printf("=====================\n\n");
}

/**
 * Fonction qui libère la mémoire de la configuration chargée.
 * @param config : La configuration qui doit être libérée.
 */
void detruire_config(Une_config *config) {
    if (config == NULL) {
        return;
    }

    if (config->fichier_stations != NULL) {
        free(config->fichier_stations);
    }
    if (config->fichier_lignes != NULL) {
        free(config->fichier_lignes);
    }
    if (config->fichier_connexions != NULL) {
        free(config->fichier_connexions);
    }

    free(config);
}
