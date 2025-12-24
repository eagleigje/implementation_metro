/*
 * PROJET MAIN3 / METRO
 * Tests divers : Configuration, CSV malformés, gestion mémoire.
 */

#include <stdio.h>

#include "test_autres.h"
#include "../config.h"
#include "../liste.h"
#include "../abr.h"
#include "../tests.h"

/**
 * Fonction de tests de gestion des configurations invalides.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_config_invalide(const char *test_label) {
    printf("\033[1;34m%s. Tests de configuration invalide\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1 : Fichier de configuration inexistant
    printf("  Test %s.1: Fichier config inexistant\n", test_label);
    tests_total++;
    Une_config *config_invalide = lire_config("fichier_inexistant_12345.ini");
    if (config_invalide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
        detruire_config(config_invalide);
    }

    // Test X.2: Destruction de config NULL
    printf("  Test %s.2: Destruction config NULL\n", test_label);
    tests_total++;
    detruire_config(NULL);
    printf("    OK: pas de crash\n");
    tests_ok++;

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}

/**
 * Fonction de tests de lecture de fichiers CSV malformés.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_csv_malformes(const char *test_label) {
    printf("\033[1;34m%s. Tests de fichiers CSV malformés\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1: Fichier stations inexistant
    printf("  Test %s.1: Fichier stations inexistant\n", test_label);
    tests_total++;
    Un_elem *liste_invalide = lire_stations("stations_inexistantes_12345.csv");
    if (liste_invalide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
        detruire_liste_et_truc(liste_invalide);
    }

    // Test X.2: Fichier CSV vide
    printf("  Test %s.2: Fichier CSV vide\n", test_label);
    tests_total++;
    char chemin_vide[256];
    snprintf(chemin_vide, sizeof(chemin_vide), "%s/stations_vide.csv", TESTS_DATA_DIR);
    Un_elem *liste_vide = lire_stations(chemin_vide);
    if (liste_vide == NULL) {
        printf("    OK: fichier vide retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ATTENTION: fichier vide retourne une liste\n");
        int nb = 0;
        Un_elem *temp = liste_vide;
        while (temp != NULL) {
            nb++;
            temp = temp->suiv;
        }
        printf("    (%d élèments trouvés)\n", nb);
        if (nb == 0) {
            tests_ok++;
        }
        detruire_liste_et_truc(liste_vide);
    }

    // Test X.3 : Fichier CSV avec lignes malformées
    printf("  Test %s.3: Fichier CSV avec lignes malformées\n", test_label);
    tests_total++;
    char chemin_malformee[256];
    snprintf(chemin_malformee, sizeof(chemin_malformee), "%s/stations_malformees.csv", TESTS_DATA_DIR);
    Un_elem *liste_malformee = lire_stations(chemin_malformee);
    if (liste_malformee != NULL) {
        int nb_stations = 0;
        Un_elem *temp = liste_malformee;
        while (temp != NULL) {
            nb_stations++;
            temp = temp->suiv;
        }
        printf("    OK: %d stations valides chargées (lignes invalides ignorées)\n", nb_stations);
        tests_ok++;
        detruire_liste_et_truc(liste_malformee);
    } else {
        printf("    ATTENTION: aucune station chargée\n");
    }

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}

/**
 * Fonction de tests de la gestion mémoire avec destruction de structures.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_gestion_memoire(const char *test_label) {
    printf("\033[1;34m%s. Tests de gestion mémoire\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1: Destruction de truc NULL
    printf("  Test %s.1: Destruction truc NULL\n", test_label);
    tests_total++;
    detruire_truc(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    // Test X.2 : Destruction de liste vide
    printf("  Test %s.2: Destruction liste vide\n", test_label);
    tests_total++;
    detruire_liste(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    // Test X.3 : Destruction de liste_et_truc vide
    printf("  Test %s.3: Destruction liste_et_truc vide\n", test_label);
    tests_total++;
    detruire_liste_et_truc(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    // Test X.4: Destruction ABR vide
    printf("  Test %s.4: Destruction ABR vide\n", test_label);
    tests_total++;
    detruire_abr(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
