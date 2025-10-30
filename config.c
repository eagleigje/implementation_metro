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
    size_t len = strlen(s) + 1;
    char *new_str = (char *)malloc(len);
    if (new_str != NULL) {
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
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier de configuration %s\n", nom_fichier);
        return NULL;
    }

    Une_config *config = (Une_config *)malloc(sizeof(Une_config));
    if (config == NULL) {
        fprintf(stderr, "Erreur: impossible de charger le fichier de configuration %s\n", nom_fichier);
        fclose(fichier);
        return NULL;
    }

    // Initialiser avec des valeurs par defaut
    config->fichier_stations = NULL;
    config->fichier_lignes = NULL;
    config->fichier_connexions = NULL;
    config->seuil_distance_aqr = 0.00001f;
    config->temps_correspondance = 3.0f;
    config->temps_arret = 0.5f;

    char ligne[256];
    // On lie le fichier de configuration, ligne par ligne.
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        // Enlever le retour a la ligne
        ligne[strcspn(ligne, "\r\n")] = '\0';
        // Trim la ligne
        char *ligne_trim = trim(ligne);
        // Ignorer les lignes vides et les commentaires
        if (ligne_trim[0] == '\0' || ligne_trim[0] == '#' || ligne_trim[0] == ';') {
            continue;
        }
        // Ignorer les sections [section]
        if (ligne_trim[0] == '[') {
            continue;
        }
        // Chercher le signe '='
        char *egal = strchr(ligne_trim, '=');
        if (egal == NULL) {
            continue;
        }
        // Separer la cle et la valeur
        *egal = '\0';
        const char *cle = trim(ligne_trim);
        char *valeur = trim(egal + 1);
        // Enlever les guillemets eventuels autour de la valeur
        if (valeur[0] == '"' || valeur[0] == '\'') {
            valeur++;
            const size_t len = strlen(valeur);
            if (len > 0 && (valeur[len-1] == '"' || valeur[len-1] == '\'')) {
                valeur[len-1] = '\0';
            }
        }

        // Parser les differentes cles
        if (strcmp(cle, "fichier_stations") == 0) {
            if (config->fichier_stations != NULL) free(config->fichier_stations);
            config->fichier_stations = strdup_custom(valeur);
        } else if (strcmp(cle, "fichier_lignes") == 0) {
            if (config->fichier_lignes != NULL) free(config->fichier_lignes);
            config->fichier_lignes = strdup_custom(valeur);
        } else if (strcmp(cle, "fichier_connexions") == 0) {
            if (config->fichier_connexions != NULL) free(config->fichier_connexions);
            config->fichier_connexions = strdup_custom(valeur);
        } else if (strcmp(cle, "seuil_distance_aqr") == 0) {
            config->seuil_distance_aqr = atof(valeur);
        } else if (strcmp(cle, "temps_correspondance") == 0) {
            config->temps_correspondance = atof(valeur);
        } else if (strcmp(cle, "temps_arret") == 0) {
            config->temps_arret = atof(valeur);
        }
    }
    fclose(fichier);

    // Verifier que les fichiers obligatoires sont definis
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
        printf("Configuration NULL\n");
        return;
    }

    printf("=== CONFIGURATION ===\n");
    printf("Fichier stations     : %s\n", config->fichier_stations);
    printf("Fichier lignes       : %s\n", config->fichier_lignes);
    printf("Fichier connexions   : %s\n", config->fichier_connexions);
    printf("Seuil distance AQR   : %.6f\n", config->seuil_distance_aqr);
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
