/*
 * PROJET MAIN3 / METRO
 * Fonctions utilitaires génériques.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "outils.h"

/**
 * Fonction qui supprime les espaces au début et à la fin d'une chaîne de caractères.
 * La chaîne est directement modifiée en place.
 * Si des espaces en début sont trouvés, le contenu est déplacé au début du buffer.
 * @param str : La chaîne que l'on veut traiter.
 * @return : Le pointeur vers la chaîne (inchangé, toujours le début du buffer).
 */
char *trim(char *str) {
    if (str == NULL) {
        return NULL;
    }

    // On cherche le premier caractère non-espace
    char *debut = str;
    while (isspace((unsigned char)*debut)) debut++;

    // Si la chaîne est vide ou ne contient que des espaces
    if (*debut == 0) {
        str[0] = '\0';
        return str;
    }

    // On cherche la fin de la chaîne (dernier caractère non-espace)
    char *fin = debut + strlen(debut) - 1;
    while (fin > debut && isspace((unsigned char)*fin)) fin--;

    // On termine la chaîne après le dernier caractère non-espace
    *(fin + 1) = '\0';

    // Si on a des espaces en début, on déplace le contenu au début du buffer
    if (debut != str) {
        memmove(str, debut, strlen(debut) + 1);  // +1 pour le '\0'
    }

    return str;
}

/**
 * Fonction qui duplique une chaîne de caractères.
 * Alloue de la mémoire pour une copie de la chaîne.
 * @param s : La chaîne qui doit être dupliquée.
 * @return : La chaîne dupliquée ou NULL en cas d'erreur d'allocation.
 */
char *strdup_custom(const char *s) {
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
