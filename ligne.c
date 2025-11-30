/*
 *  PROJET MAIN3 / METRO
 *  Gestion des listes de lignes de metro.
*/

#include "ligne.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Fonction qui créé un maillon d'une ligne
 * @param ligne_str : La ligne au format texte à gérer
 * @return : Pointeur vers la nouvelle ligne ou NULL en cas d'erreur.
 */
Une_ligne *creer_ligne(char ligne_str[CSV_MAX_LINE_LENGTH]) {
    char code[CODE_SIZE], color[COLOR_SIZE];
    float vitesse, intervalle;

    // On pointe en début de la ligne récupérée.
    char *ptr = ligne_str;

    // On saute les espaces éventuels.
    while (*ptr == ' ') ptr++;
    int i = 0;
    // On parcourt la ligne jusqu'à tomber sur le séparateur, être en fin de ligne ou dépasser la taille max du champ.
    while (*ptr != CSV_SEPARATOR && *ptr != '\0' && i < CODE_SIZE - 1) {
        code[i++] = *ptr++;
    }
    code[i] = '\0';
    // Enlever les espaces de fin
    while (i > 0 && code[i-1] == ' ') code[--i] = '\0';

    // Si nous ne sommes pas sur un séparateur, la ligne dans le fichier est incorrecte.
    if (*ptr != CSV_SEPARATOR) return NULL;
    // Sinon, on passe le séparateur.
    ptr++;

    // Lire la vitesse
    // On saute les espaces en début de chaîne.
    while (*ptr == ' ') ptr++;
    // On récupère le champ
    if (sscanf(ptr, "%f", &vitesse) != 1) return NULL;
    // et on continue jusqu'au prochain séparateur ou la fin de ligne.
    while (*ptr != CSV_SEPARATOR && *ptr != '\0') ptr++;

    // Si nous ne sommes pas sur un séparateur, la ligne dans le fichier est incorrecte.
    if (*ptr != CSV_SEPARATOR) return NULL;
    // Sinon, on passe le séparateur.
    ptr++;

    // On cherche à lire l'intervalle avec la méthode que précédemment.
    while (*ptr == ' ') ptr++;
    if (sscanf(ptr, "%f", &intervalle) != 1) return NULL;
    while (*ptr != CSV_SEPARATOR && *ptr != '\0') ptr++;
    if (*ptr != CSV_SEPARATOR) return NULL;
    ptr++;

    // On cherche maintenant à lire la couleur qui doit être en fin de ligne
    while (*ptr == ' ') ptr++;
    i = 0;
    while (*ptr != '\0' && *ptr != '\n' && i < COLOR_SIZE - 1) {
        color[i++] = *ptr++;
    }
    color[i] = '\0';
    // Enlever les espaces de fin
    while (i > 0 && color[i-1] == ' ') color[--i] = '\0';

    // On crée maintenant la ligne pour l'ajouter à la liste chaînée.
    Une_ligne *nouvelle = (Une_ligne *)malloc(sizeof(Une_ligne));
    nouvelle->code = (char *)malloc(strlen(code) + 1);
    strcpy(nouvelle->code, code);
    nouvelle->color = (char *)malloc(strlen(color) + 1);
    strcpy(nouvelle->color, color);
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
    if (fgets(ligne, sizeof(ligne), fichier) == NULL) return NULL;
    Une_ligne *tete = creer_ligne(ligne);

    // Puis, on lit les lignes suivantes jusqu'à la fin du fichier.
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        Une_ligne *nouvelle = creer_ligne(ligne);
        nouvelle->suiv = tete;
        tete= nouvelle;
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
