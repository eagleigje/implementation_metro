/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 3 : Lignes de métro.
 */

#include <stdio.h>
#include <string.h>

#include "test_exercice3.h"
#include "../ligne.h"
#include "../tests.h"

/**
 * Fonction de test pour la lecture et la gestion des lignes de métro (Exercice 3).
 * Teste la lecture depuis un fichier CSV, l'affichage, la recherche et la robustesse.
 * @param config : La configuration contenant le chemin vers le fichier lignes.csv.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_lignes(Une_config *config, const char *test_label) {
    printf("\033[1;34m%s. Tests de lecture et affichage des lignes\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1 : Lecture du fichier lignes depuis la configuration
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
        printf("    %d Lignes chargées avec succès\n", nb_lignes);
        tests_ok++;
    } else {
        printf("    ERREUR: impossible de charger les lignes\n");
    }

    // Test X.2 : Affichage des lignes
    if (lignes != NULL) {
        printf("  Test %s.2: Affichage des lignes\n", test_label);
        tests_total++;
        printf("    Format: code ; vitesse ; intervalle ; couleur\n");
        afficher_lignes(lignes);
        printf("    OK: affichage réussi\n");
        tests_ok++;
    }

    // Test X.3 : Recherche d'une ligne existante
    if (lignes != NULL) {
        printf("  Test %s.3: Recherche ligne existante (première ligne du fichier)\n", test_label);
        tests_total++;
        // On teste avec le code de la première ligne chargée (test dynamique).
        char *code_test = lignes->code;
        Une_ligne *ligne_trouvee = chercher_ligne(lignes, code_test);
        if (ligne_trouvee != NULL) {
            printf("    OK: ligne '%s' trouvée (vitesse=%.1f, intervalle=%.1f, couleur=%s)\n",
                   code_test, ligne_trouvee->vitesse, ligne_trouvee->intervalle, ligne_trouvee->color);
            tests_ok++;
        } else {
            printf("    ERREUR: ligne '%s' non trouvée\n", code_test);
        }
    }

    // Test X.4 : Recherche d'une ligne inexistante
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

    // Test X.5 : Recherche insensible à la casse
    if (lignes != NULL) {
        printf("  Test %s.5: Recherche insensible à la casse\n", test_label);
        tests_total++;
        // On teste avec une version en minuscule du code de la première ligne
        char code_minuscule[10];
        strcpy(code_minuscule, lignes->code);
        // On convertit en minuscule
        for (int i = 0; code_minuscule[i]; i++) {
            if (code_minuscule[i] >= 'A' && code_minuscule[i] <= 'Z') {
                code_minuscule[i] = code_minuscule[i] + ('a' - 'A');
            }
        }
        Une_ligne *ligne_min = chercher_ligne(lignes, code_minuscule);
        if (ligne_min != NULL) {
            printf("    OK: recherche insensible à la casse (%s == %s)\n", code_minuscule, lignes->code);
            tests_ok++;
        } else {
            printf("    ERREUR: recherche devrait être insensible à la casse\n");
        }
    }

    // Test X.6 : Lecture d'un fichier lignes inexistant
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

    // Test X.7 : Lecture d'un fichier lignes avec erreurs de champs
    printf("  Test %s.7: Lecture fichier lignes invalide (erreurs attendues)\n", test_label);
    tests_total++;
    char chemin_invalides[256];
    snprintf(chemin_invalides, sizeof(chemin_invalides), "%s/lignes_invalides.csv", TESTS_DATA_DIR);
    Une_ligne *lignes_invalides = lire_lignes(chemin_invalides);
    if (lignes_invalides != NULL) {
        int nb_valides = 0;
        Une_ligne *tmp_invalides = lignes_invalides;
        while (tmp_invalides != NULL) {
            nb_valides++;
            tmp_invalides = tmp_invalides->suiv;
        }
        if (nb_valides == 1) {
            printf("    OK: 1 ligne valide chargée (avertissements attendus)\n");
            tests_ok++;
        } else {
            printf("    ERREUR: %d ligne(s) valides (attendu: 1)\n", nb_valides);
        }
        detruire_lignes(lignes_invalides);
    } else {
        printf("    ERREUR: aucune ligne valide chargée\n");
    }

    // Et on libère la mémoire
    if (lignes != NULL) {
        detruire_lignes(lignes);
    }

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
