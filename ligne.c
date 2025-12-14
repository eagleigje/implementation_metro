/*
 *  PROJET MAIN3 / METRO
 *  Gestion des listes de lignes de metro.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ligne.h"
#include "constantes.h"
#include "csv_utils.h"
#include "outils.h"

/**
 * Fonction qui créé un maillon d'une ligne
 * @param ligne_str : La ligne au format texte à gérer
 * @return : Pointeur vers la nouvelle ligne ou NULL en cas d'erreur.
 */
static Une_ligne *creer_ligne(char *ligne_str) {
    Un_lecteur_csv lecteur;

    // On initialise le lecteur CSV
    csv_init_lecteur(&lecteur, ligne_str, CSV_SEPARATOR);

    // On lit les champs un à un.
    // Lecture du code de la ligne
    if (!csv_champ_suivant(&lecteur)) return NULL;
    char *code_ptr = csv_lire_texte(&lecteur, LINE_MAX_CODE_SIZE);
    if (code_ptr == NULL) return NULL;  // Champ trop grand
    char code[LINE_MAX_CODE_SIZE];
    strcpy(code, code_ptr);  // On doit copier les champs textes, sinon ils sont écrasés par les suivants.

    // La vitesse de la ligne
    if (!csv_champ_suivant(&lecteur)) return NULL;
    float vitesse = csv_lire_flottant(&lecteur);

    // L'intervalle entre deux rames sur la ligne.
    if (!csv_champ_suivant(&lecteur)) return NULL;
    float intervalle = csv_lire_flottant(&lecteur);

    // La couleur de ligne.
    if (!csv_champ_suivant(&lecteur)) return NULL;
    char *couleur_ptr = csv_lire_texte(&lecteur, LINE_COLOR_SIZE);
    if (couleur_ptr == NULL) return NULL;  // Champ trop grand
    char couleur[LINE_COLOR_SIZE];
    strcpy(couleur, couleur_ptr);

    // On peut créer la nouvelle ligne
    Une_ligne *nouvelle = (Une_ligne *)malloc(sizeof(Une_ligne));
    if (nouvelle == NULL) return NULL;

    nouvelle->code = (char *)malloc(strlen(code) + 1);
    if (nouvelle->code == NULL) {
        free(nouvelle);
        return NULL;
    }
    strcpy(nouvelle->code, code);

    nouvelle->color = (char *)malloc(strlen(couleur) + 1);
    if (nouvelle->color == NULL) {
        free(nouvelle->code);
        free(nouvelle);
        return NULL;
    }
    strcpy(nouvelle->color, couleur);

    nouvelle->vitesse = vitesse;
    nouvelle->intervalle = intervalle;
    nouvelle->suiv = NULL;
    return nouvelle;
}

/**
 * Fonction qui parcourt le fichier .csv fourni en argument et construit la liste de Une_ligne correspondante.
 * @param nom_fichier : Le fichier CSV des lignes, à lire.
 * @return : Renvoi la liste des lignes, ou NULL en cas d'erreur.
 */
Une_ligne *lire_lignes(char *nom_fichier) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le fichier %s\n", nom_fichier);
        return NULL;
    }

    Une_ligne *tete = NULL;
    Une_ligne *tmp = NULL;
    char ligne[CSV_MAX_LINE_LENGTH];
    int ligne_num = 0;

    // On lit toutes les lignes du fichier
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        ligne_num++;
        // On supprime le retour à la ligne
        ligne[strcspn(ligne, "\r\n")] = '\0';

        // On nettoie les espaces en début et fin
        trim(ligne);

        // On ignore les lignes vides et les commentaires (commençant par #)
        if (ligne[0] == '\0' || ligne[0] == '#') {
            continue;
        }

        Une_ligne *nouvelle = creer_ligne(ligne);
        if (nouvelle == NULL) {
            // On trace l'échec et on continue
            fprintf(stderr, "Avertissement: échec de création de la ligne %d: %s\n",
                    ligne_num, ligne);
            continue;
        }

        // Si c'est la première ligne valide, elle devient la tête
        if (tete == NULL) {
            tete = nouvelle;
            tmp = tete;
        } else {
            // Sinon on l'ajoute à la fin
            tmp->suiv = nouvelle;
            tmp = nouvelle;
        }
    }

    fclose(fichier);
    return tete;
}

/**
 * Fonction qui affiche sur STDOUT la liste des lignes.
 * @param ligne : La liste des lignes à afficher.
 */
void afficher_lignes(Une_ligne *ligne) {
    Une_ligne *ptr = ligne;

    while (ptr != NULL) {
        printf("%s ; %.1f ; %.1f ; %s\n",
               ptr->code,
               ptr->vitesse,
               ptr->intervalle,
               ptr->color);
        ptr = ptr->suiv;
    }
}

/**
 * Fonction qui libère la mémoire d'une liste de lignes.
 * @param ligne : La liste de lignes à libérer.
 */
void detruire_lignes(Une_ligne *ligne) {
    Une_ligne *ptr = ligne;
    Une_ligne *suivant;

    while (ptr != NULL) {
        suivant = ptr->suiv;
        if (ptr->code != NULL) {
            free(ptr->code);
        }
        if (ptr->color != NULL) {
            free(ptr->color);
        }
        free(ptr);
        ptr = suivant;
    }
}

/**
 * Fonction qui recherche une station en fonction du code passé en paramètre.
 * @param ligne : La liste de lignes à parcourir.
 * @param code : La chaîne de caractère à rechercher.
 * @return : Soit la ligne si elle est trouvée sinon NULL.
 */
Une_ligne *chercher_ligne(Une_ligne *ligne, char *code) {
    Une_ligne *courant = ligne;

    while (courant != NULL) {
        // Cette recherche est sensible à la casse.
        if (strcmp(courant->code, code) == 0) {
            return courant;
        }
        courant = courant->suiv;
    }

    return NULL;
}
