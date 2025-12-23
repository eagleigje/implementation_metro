/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 2 : ABR (Arbres Binaires de Recherche).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "test_exercice2.h"
#include "../liste.h"
#include "../truc.h"

/**
 * Fonction de test de recherche de stations dans l'ABR.
 * @param abr_stations : L'ABR des stations.
 */
void test_recherche_abr(Un_nabr *abr_stations) {
    char *stations_test[] = {
        "Jussieu",
        "jussieu",
        "Chatelet",
        "Châtelet Les Halles",
        "Gare du Nord",
        "Station Inconnue",
        "Nation",
        NULL
    };

    int nb_trouvees = 0;
    int nb_total = 0;

    for (int i = 0; stations_test[i] != NULL; i++) {
        nb_total++;
        Un_truc *station = chercher_station(abr_stations, stations_test[i]);
        if (station != NULL) {
            nb_trouvees++;
            printf("  [TROUVE] %s - Position: (%.6f, %.6f)\n",
                   stations_test[i],
                   station->coord.lon,
                   station->coord.lat);
        } else {
            printf("  [NON TROUVE] %s\n", stations_test[i]);
        }
    }
    printf("  Résultat: %d trouvées / %d recherches\n\n", nb_trouvees, nb_total);
}

/**
 * Fonction qui crée une station simple pour les tests de préfixe.
 * @param nom : Nom de la station à créer.
 * @return : Le truc station créé, ou NULL en cas d'échec d'allocation.
 */
static Un_truc *creer_station_test(const char *nom) {
    Une_coord coord = {0.0f, 0.0f};
    Tdata data;
    data.sta.nom = (char *)malloc(strlen(nom) + 1);
    if (data.sta.nom == NULL) {
        return NULL;
    }
    strcpy(data.sta.nom, nom);
    data.sta.tab_con = NULL;
    data.sta.nb_con = 0;
    data.sta.con_pcc = NULL;
    return creer_truc(coord, STA, data, 0.0);
}

/**
 * Fonction de test de recherche par préfixe dans l'ABR.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_recherche_prefixe_abr(const char *test_label) {
    printf("\033[1;34m%s. Tests de recherche par préfixe dans l'ABR\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Construction d'un petit ABR synthétique pour éviter la dépendance aux CSV.
    Un_elem *liste = NULL;
    Un_truc *s1 = creer_station_test("Porte A");
    Un_truc *s2 = creer_station_test("Porte B");
    Un_truc *s3 = creer_station_test("Gare C");
    if (s1 != NULL) liste = inserer_deb_liste(liste, s1);
    if (s2 != NULL) liste = inserer_deb_liste(liste, s2);
    if (s3 != NULL) liste = inserer_deb_liste(liste, s3);

    Un_nabr *abr = construire_abr(liste);

    // Test X.1 : Préfixe trouvé
    tests_total++;
    const char *prefixe_ok = "Porte";
    Un_truc *trouve = chercher_station_prefixe(abr, prefixe_ok);
    if (trouve != NULL && strncasecmp(trouve->data.sta.nom, prefixe_ok, strlen(prefixe_ok)) == 0) {
        printf("  %s.1 OK: préfixe \"%s\" trouvé (%s)\n", test_label, prefixe_ok, trouve->data.sta.nom);
        tests_ok++;
    } else {
        printf("  %s.1 ERREUR: préfixe \"%s\" non trouvé\n", test_label, prefixe_ok);
    }

    // Test X.2 : Préfixe absent
    tests_total++;
    const char *prefixe_ko = "Zoo";
    Un_truc *absent = chercher_station_prefixe(abr, prefixe_ko);
    if (absent == NULL) {
        printf("  %s.2 OK: préfixe \"%s\" absent\n", test_label, prefixe_ko);
        tests_ok++;
    } else {
        printf("  %s.2 ERREUR: préfixe \"%s\" trouvé (%s)\n", test_label, prefixe_ko, absent->data.sta.nom);
    }

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);

    detruire_abr(abr);
    detruire_liste_et_truc(liste);
}

/**
 * Fonction de tests de robustesse de l'ABR.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_robustesse_abr(const char *test_label) {
    printf("\033[1;34m%s. Tests de robustesse pour l'ABR\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1: Recherche dans ABR vide
    printf("  Test %s.1: Recherche dans ABR vide\n", test_label);
    tests_total++;
    Un_truc *resultat_vide = chercher_station(NULL, "Test");
    if (resultat_vide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
    }

    // Test X.2 : Construction ABR à partir d'une liste vide
    printf("  Test %s.2: Construction ABR depuis liste vide\n", test_label);
    tests_total++;
    Un_nabr *abr_vide = construire_abr(NULL);
    if (abr_vide == NULL) {
        printf("    OK: ABR vide retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: ABR devrait être NULL\n");
    }

    // Test X.3 : Recherche avec nom vide
    printf("  Test %s.3: Recherche avec nom vide\n", test_label);
    tests_total++;
    Une_coord coord_abr = {2.35f, 48.85f};
    Tdata data_abr;
    data_abr.sta.nom = (char *)malloc(20);
    strcpy(data_abr.sta.nom, "StationTest");
    data_abr.sta.tab_con = NULL;
    data_abr.sta.nb_con = 0;
    data_abr.sta.con_pcc = NULL;
    Un_truc *truc_abr = creer_truc(coord_abr, STA, data_abr, 48.85);
    Un_nabr *abr_test = creer_nabr(truc_abr);
    Un_truc *resultat_nom_vide = chercher_station(abr_test, "");
    if (resultat_nom_vide == NULL) {
        printf("    OK: nom vide ne trouve rien\n");
        tests_ok++;
    } else {
        printf("    ERREUR: nom vide ne devrait rien trouver\n");
    }
    detruire_liste_et_truc(inserer_deb_liste(NULL, truc_abr));
    detruire_abr(abr_test);

    // Test X.4 : Recherche avec caractères spéciaux
    printf("  Test %s.4: Recherche avec caractères spéciaux\n", test_label);
    tests_total++;
    Une_coord coord_spec = {2.35f, 48.85f};
    Tdata data_spec;
    data_spec.sta.nom = (char *)malloc(30);
    strcpy(data_spec.sta.nom, "Gare d'Austerlitz");
    data_spec.sta.tab_con = NULL;
    data_spec.sta.nb_con = 0;
    data_spec.sta.con_pcc = NULL;
    Un_truc *truc_spec = creer_truc(coord_spec, STA, data_spec, 48.85);
    Un_nabr *abr_spec = creer_nabr(truc_spec);
    Un_truc *resultat_spec = chercher_station(abr_spec, "Gare d'Austerlitz");
    if (resultat_spec != NULL) {
        printf("    OK: station avec apostrophe trouvée\n");
        tests_ok++;
    } else {
        printf("    ERREUR: station avec apostrophe non trouvée\n");
    }
    detruire_liste_et_truc(inserer_deb_liste(NULL, truc_spec));
    detruire_abr(abr_spec);

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
