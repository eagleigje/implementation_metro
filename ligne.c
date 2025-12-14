/*
 *  PROJET MAIN3 / METRO
 *  Gestion des listes de lignes de metro.
*/

#include "ligne.h"
#include "constants.h"
#include "csv_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    strcpy(nouvelle->code, code);
    nouvelle->color = (char *)malloc(strlen(couleur) + 1);
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

    // On lit la première ligne, cela nous donne la tête de liste.
    char ligne[CSV_MAX_LINE_LENGTH];
    if (fgets(ligne, sizeof(ligne), fichier) == NULL) {
        fclose(fichier);
        return NULL;
    }
    Une_ligne *tete = creer_ligne(ligne);

    // Puis, on lit les lignes suivantes jusqu'à la fin du fichier.
    Une_ligne *tmp = tete;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Une_ligne *nouvelle = creer_ligne(ligne);
        tmp->suiv = nouvelle;
        tmp = tmp->suiv;
    }

    fclose(fichier);
    return tete;
}

/**
 * Fonction qui affiche sur STDOUT la liste des lignes.
 * @param ligne : La liste à afficher.
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
