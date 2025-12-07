/*
 * PROJET MAIN3 / METRO
 * Tests du lecteur CSV générique.
 */

#include <stdio.h>
#include <string.h>
#include "test_csv_utils.h"
#include "../csv_utils.h"

/**
 * Tests du lecteur CSV générique.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_csv_utils(const char *test_label) {
    int tests_total = 0;
    int tests_ok = 0;

    printf("\033[1;34m%s. Tests du lecteur CSV générique\033[0m\n", test_label);

    // Test X.1: Lecture d'une ligne simple avec 3 champs
    printf("  Test %s.1: Lecture d'une ligne simple avec 3 champs\n", test_label);
    tests_total++;
    char ligne1[] = "Jussieu;2.354086;48.846050";
    Un_lecteur_csv lecteur;
    csv_init_lecteur(&lecteur, ligne1, ';');

    if (csv_champ_suivant(&lecteur)) {
        char *nom = csv_lire_texte(&lecteur);
        if (strcmp(nom, "Jussieu") == 0) {
            if (csv_champ_suivant(&lecteur)) {
                float lon = csv_lire_flottant(&lecteur);
                if (lon > 2.35 && lon < 2.36) {
                    if (csv_champ_suivant(&lecteur)) {
                        float lat = csv_lire_flottant(&lecteur);
                        if (lat > 48.84 && lat < 48.85) {
                            printf("    OK: 3 champs lus correctement\n");
                            tests_ok++;
                        } else {
                            printf("    ERREUR: latitude incorrecte\n");
                        }
                    } else {
                        printf("    ERREUR: échec lecture 3e champ\n");
                    }
                } else {
                    printf("    ERREUR: longitude incorrecte\n");
                }
            } else {
                printf("    ERREUR: échec lecture 2e champ\n");
            }
        } else {
            printf("    ERREUR: nom de station incorrect\n");
        }
    } else {
        printf("    ERREUR: échec lecture 1er champ\n");
    }

    // Test X.2: Gestion des espaces en début et fin de champ
    printf("  Test %s.2: Gestion des espaces en début et fin de champ\n", test_label);
    tests_total++;
    char ligne2[] = " Chatelet ;  2.347  ; 48.858 ";
    csv_init_lecteur(&lecteur, ligne2, ';');

    if (csv_champ_suivant(&lecteur)) {
        char *nom = csv_lire_texte(&lecteur);
        if (strcmp(nom, "Chatelet") == 0) {
            printf("    OK: espaces correctement supprimés\n");
            tests_ok++;
        } else {
            printf("    ERREUR: espaces non supprimés (nom='%s')\n", nom);
        }
    } else {
        printf("    ERREUR: échec lecture champ avec espaces\n");
    }

    // Test X.3: Lecture d'entiers
    printf("  Test %s.3: Lecture d'entiers\n", test_label);
    tests_total++;
    char ligne3[] = "1;14;Paris";
    csv_init_lecteur(&lecteur, ligne3, ';');

    if (csv_champ_suivant(&lecteur)) {
        int num1 = csv_lire_entier(&lecteur);
        if (num1 == 1) {
            if (csv_champ_suivant(&lecteur)) {
                int num2 = csv_lire_entier(&lecteur);
                if (num2 == 14) {
                    printf("    OK: entiers lus correctement\n");
                    tests_ok++;
                } else {
                    printf("    ERREUR: second entier incorrect\n");
                }
            } else {
                printf("    ERREUR: échec lecture 2e champ\n");
            }
        } else {
            printf("    ERREUR: premier entier incorrect\n");
        }
    } else {
        printf("    ERREUR: échec lecture 1er champ\n");
    }

    // Test X.4: Détection de fin de ligne
    printf("  Test %s.4: Détection de fin de ligne\n", test_label);
    tests_total++;
    char ligne4[] = "A;B";
    csv_init_lecteur(&lecteur, ligne4, ';');

    csv_champ_suivant(&lecteur); // A
    csv_champ_suivant(&lecteur); // B
    if (csv_est_fini(&lecteur) == 0) {
        if (csv_champ_suivant(&lecteur) == 0) {
            if (csv_est_fini(&lecteur) == 1) {
                printf("    OK: fin de ligne détectée\n");
                tests_ok++;
            } else {
                printf("    ERREUR: csv_est_fini devrait retourner 1\n");
            }
        } else {
            printf("    ERREUR: csv_champ_suivant devrait retourner 0\n");
        }
    } else {
        printf("    ERREUR: csv_est_fini ne devrait pas être fini après 2 champs\n");
    }

    // Test X.5: Ligne vide
    printf("  Test %s.5: Gestion d'une ligne vide\n", test_label);
    tests_total++;
    char ligne5[] = "";
    csv_init_lecteur(&lecteur, ligne5, ';');

    if (csv_champ_suivant(&lecteur) == 0 && csv_est_fini(&lecteur) == 1) {
        printf("    OK: ligne vide gérée correctement\n");
        tests_ok++;
    } else {
        printf("    ERREUR: ligne vide mal gérée\n");
    }

    // Test X.6: Champ contenant des caractères spéciaux
    printf("  Test %s.6: Champ avec caractères spéciaux (apostrophe)\n", test_label);
    tests_total++;
    char ligne6[] = "Gare d'Austerlitz;2.365;48.842";
    csv_init_lecteur(&lecteur, ligne6, ';');

    if (csv_champ_suivant(&lecteur)) {
        char *nom = csv_lire_texte(&lecteur);
        if (strcmp(nom, "Gare d'Austerlitz") == 0) {
            printf("    OK: apostrophe préservée\n");
            tests_ok++;
        } else {
            printf("    ERREUR: apostrophe mal gérée (nom='%s')\n", nom);
        }
    } else {
        printf("    ERREUR: échec lecture champ avec apostrophe\n");
    }

    // Test X.7: Champ avec parenthèses (cas réel de Paris)
    printf("  Test %s.7: Champ avec parenthèses\n", test_label);
    tests_total++;
    char ligne7[] = "Saint-Paul (Le Marais);2.361;48.855";
    csv_init_lecteur(&lecteur, ligne7, ';');

    if (csv_champ_suivant(&lecteur)) {
        char *nom = csv_lire_texte(&lecteur);
        if (strcmp(nom, "Saint-Paul (Le Marais)") == 0) {
            printf("    OK: parenthèses préservées\n");
            tests_ok++;
        } else {
            printf("    ERREUR: parenthèses mal gérées (nom='%s')\n", nom);
        }
    } else {
        printf("    ERREUR: échec lecture champ avec parenthèses\n");
    }

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
