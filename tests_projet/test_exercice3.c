/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 3 : Lignes de métro.
 */

#include <stdio.h>
#include "test_exercice3.h"
#include "../ligne.h"

void test_lignes(Une_config *config, const char *test_label) {
    printf("\033[1;34m%s. Tests de lecture et affichage des lignes\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1: Lecture du fichier lignes depuis la configuration
    printf("  Test %s.1: Lecture du fichier %s\n", test_label, config->fichier_lignes);
    tests_total++;
    Une_ligne *lignes = lire_lignes(config->fichier_lignes);
    if (lignes != NULL) {
        int nb_lignes = 0;
        Une_ligne *temp = lignes;
        while (temp != NULL) {
            nb_lignes++;
            temp = temp->suiv;
        }
        printf("    OK: %d lignes chargées\n", nb_lignes);
        tests_ok++;
    } else {
        printf("    ERREUR: impossible de charger les lignes\n");
    }

    // Test X.2: Affichage des lignes
    if (lignes != NULL) {
        printf("  Test %s.2: Affichage des lignes\n", test_label);
        tests_total++;
        printf("    Format: code ; vitesse ; intervalle ; couleur\n");
        afficher_lignes(lignes);
        printf("    OK: affichage réussi\n");
        tests_ok++;
    }

    // Test X.3: Recherche d'une ligne existante
    if (lignes != NULL) {
        printf("  Test %s.3: Recherche ligne 1\n", test_label);
        tests_total++;
        Une_ligne *ligne_m1 = chercher_ligne(lignes, "1");
        if (ligne_m1 != NULL) {
            printf("    OK: ligne 1 trouvée (vitesse=%.1f, intervalle=%.1f, couleur=%s)\n",
                   ligne_m1->vitesse, ligne_m1->intervalle, ligne_m1->color);
            tests_ok++;
        } else {
            printf("    ERREUR: ligne 1 non trouvée\n");
        }
    }

    // Test X.4: Recherche d'une ligne inexistante
    if (lignes != NULL) {
        printf("  Test %s.4: Recherche ligne inexistante\n", test_label);
        tests_total++;
        Une_ligne *ligne_inconnue = chercher_ligne(lignes, "M99");
        if (ligne_inconnue == NULL) {
            printf("    OK: ligne M99 non trouvée (comportement attendu)\n");
            tests_ok++;
        } else {
            printf("    ERREUR: ligne M99 trouvée alors qu'elle ne devrait pas exister\n");
        }
    }

    // Test X.5: Recherche sensible à la casse
    if (lignes != NULL) {
        printf("  Test %s.5: Recherche sensible à la casse\n", test_label);
        tests_total++;
        Une_ligne *ligne_min = chercher_ligne(lignes, "m1");
        if (ligne_min == NULL) {
            printf("    OK: recherche sensible à la casse (m1 != M1)\n");
            tests_ok++;
        } else {
            printf("    ATTENTION: recherche insensible à la casse\n");
        }
    }

    // Test X.6: Lecture d'un fichier lignes inexistant
    printf("  Test %s.6: Lecture fichier lignes inexistant\n", test_label);
    tests_total++;
    Une_ligne *lignes_invalide = lire_lignes("lignes_inexistantes_12345.csv");
    if (lignes_invalide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
        detruire_lignes(lignes_invalide);
    }

    // Libération de la mémoire
    if (lignes != NULL) {
        detruire_lignes(lignes);
    }

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
