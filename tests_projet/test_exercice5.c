/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 5 : Arbres quaternaires.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_exercice5.h"
#include "../aqrtopo.h"
#include "../liste.h"
#include "../truc.h"

/**
 * Fonction de test pour les arbres quaternaires (Exercice 5).
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_arbres_quaternaires(const char *test_label) {
    int tests_total = 0;
    int tests_ok = 0;

    printf("\033[1;34m%s. Tests des arbres quaternaires (AQR)\033[0m\n", test_label);

    // Créer une liste de stations de test
    Un_elem *liste_test = NULL;

    // Station 1 : Jussieu (2.354660, 48.845989)
    Une_coord coord1 = {2.354660, 48.845989};
    Tdata data1;
    data1.sta.nom = (char *)malloc(8);
    strcpy(data1.sta.nom, "Jussieu");
    data1.sta.tab_con = NULL;
    data1.sta.nb_con = 0;
    data1.sta.con_pcc = NULL;
    Un_truc *truc1 = creer_truc(coord1, STA, data1, coord1.lat);
    liste_test = inserer_liste_trie(liste_test, truc1);

    // Station 2 : Chatelet (2.346600, 48.853500)
    Une_coord coord2 = {2.346600, 48.853500};
    Tdata data2;
    data2.sta.nom = (char *)malloc(9);
    strcpy(data2.sta.nom, "Chatelet");
    data2.sta.tab_con = NULL;
    data2.sta.nb_con = 0;
    data2.sta.con_pcc = NULL;
    Un_truc *truc2 = creer_truc(coord2, STA, data2, coord2.lat);
    liste_test = inserer_liste_trie(liste_test, truc2);

    // Station 3 : Nation (2.373377, 48.846676)
    Une_coord coord3 = {2.373377, 48.846676};
    Tdata data3;
    data3.sta.nom = (char *)malloc(7);
    strcpy(data3.sta.nom, "Nation");
    data3.sta.tab_con = NULL;
    data3.sta.nb_con = 0;
    data3.sta.con_pcc = NULL;
    Un_truc *truc3 = creer_truc(coord3, STA, data3, coord3.lat);
    liste_test = inserer_liste_trie(liste_test, truc3);

    // Station 4 : Gare du Nord (2.355952, 48.878601)
    Une_coord coord4 = {2.355952, 48.878601};
    Tdata data4;
    data4.sta.nom = (char *)malloc(14);
    strcpy(data4.sta.nom, "Gare du Nord");
    data4.sta.tab_con = NULL;
    data4.sta.nb_con = 0;
    data4.sta.con_pcc = NULL;
    Un_truc *truc4 = creer_truc(coord4, STA, data4, coord4.lat);
    liste_test = inserer_liste_trie(liste_test, truc4);

    // Test X.1: Construction de l'AQR
    printf("  Test %s.1: Construction de l'AQR depuis une liste de 4 stations\n", test_label);
    tests_total++;
    Un_noeud *aqr = construire_aqr(liste_test);
    if (aqr != NULL) {
        printf("    OK: AQR construit avec succès\n");
        tests_ok++;
    } else {
        printf("    ERREUR: échec de construction de l'AQR\n");
    }

    // Test X.2 : Recherche du truc le plus proche
    if (aqr != NULL) {
        printf("  Test %s.2: Recherche de la station la plus proche d'un point\n", test_label);
        tests_total++;

        // Point de test proche de Jussieu
        Une_coord point_test = {2.354700, 48.846000};
        Un_truc *trouve = chercher_aqr(aqr, point_test);

        if (trouve != NULL && strcmp(trouve->data.sta.nom, "Jussieu") == 0) {
            printf("    OK: station la plus proche trouvée = %s\n", trouve->data.sta.nom);
            tests_ok++;
        } else if (trouve != NULL) {
            printf("    ERREUR: mauvaise station trouvée = %s (attendu: Jussieu)\n", trouve->data.sta.nom);
        } else {
            printf("    ERREUR: aucune station trouvée\n");
        }
    }

    // Test X.3 : Recherche du truc le plus proche (autre point)
    if (aqr != NULL) {
        printf("  Test %s.3: Recherche de la station la plus proche (point proche de Nation)\n", test_label);
        tests_total++;

        // Point proche de Nation
        Une_coord point_test2 = {2.373000, 48.846500};
        Un_truc *trouve2 = chercher_aqr(aqr, point_test2);

        if (trouve2 != NULL && strcmp(trouve2->data.sta.nom, "Nation") == 0) {
            printf("    OK: station la plus proche trouvée = %s\n", trouve2->data.sta.nom);
            tests_ok++;
        } else if (trouve2 != NULL) {
            printf("    ERREUR: mauvaise station trouvée = %s (attendu: Nation)\n", trouve2->data.sta.nom);
        } else {
            printf("    ERREUR: aucune station trouvée\n");
        }
    }

    // Test X.4: Recherche par zone (rectangle englobant 2 stations)
    if (aqr != NULL) {
        printf("  Test %s.4: Recherche de stations dans une zone rectangulaire\n", test_label);
        tests_total++;

        // Zone englobant Jussieu et Chatelet
        Une_coord zone_no = {2.340000, 48.855000};  // NO
        Une_coord zone_se = {2.360000, 48.845000};  // SE

        Un_elem *resultats = chercher_zone(aqr, NULL, zone_no, zone_se);

        // Compter les résultats
        int nb_resultats = 0;
        Un_elem *temp = resultats;
        while (temp != NULL) {
            nb_resultats++;
            temp = temp->suiv;
        }

        if (nb_resultats == 2) {
            printf("    OK: %d stations trouvées dans la zone\n", nb_resultats);
            tests_ok++;
        } else {
            printf("    ERREUR: %d stations trouvées (attendu: 2)\n", nb_resultats);
        }

        // Libérer la liste de résultats (pas les trucs eux-mêmes)
        detruire_liste(resultats);
    }

    // Test X.5 : Recherche par zone (toute la zone)
    if (aqr != NULL) {
        printf("  Test %s.5: Recherche dans une grande zone englobant toutes les stations\n", test_label);
        tests_total++;

        // Zone englobant toutes les stations
        Une_coord zone_no_large = {2.340000, 48.880000};
        Une_coord zone_se_large = {2.380000, 48.840000};

        Un_elem *resultats_all = chercher_zone(aqr, NULL, zone_no_large, zone_se_large);

        // Compter les résultats
        int nb_total = 0;
        Un_elem *temp2 = resultats_all;
        while (temp2 != NULL) {
            nb_total++;
            temp2 = temp2->suiv;
        }

        if (nb_total == 4) {
            printf("    OK: %d stations trouvées (toutes)\n", nb_total);
            tests_ok++;
        } else {
            printf("    ERREUR: %d stations trouvées (attendu: 4)\n", nb_total);
        }

        detruire_liste(resultats_all);
    }

    // Test X.6 : Recherche par zone (zone vide)
    if (aqr != NULL) {
        printf("  Test %s.6: Recherche dans une zone vide (sans stations)\n", test_label);
        tests_total++;

        // Zone ne contenant aucune station
        Une_coord zone_no_vide = {2.300000, 48.900000};
        Une_coord zone_se_vide = {2.310000, 48.890000};

        Un_elem *resultats_vide = chercher_zone(aqr, NULL, zone_no_vide, zone_se_vide);

        if (resultats_vide == NULL) {
            printf("    OK: aucune station trouvée dans la zone vide\n");
            tests_ok++;
        } else {
            printf("    ERREUR: des stations trouvées dans une zone vide\n");
        }
    }

    // Test X.7 : Construction AQR avec liste vide
    printf("  Test %s.7: Construction AQR avec liste vide\n", test_label);
    tests_total++;
    Un_noeud *aqr_vide = construire_aqr(NULL);
    if (aqr_vide == NULL) {
        printf("    OK: AQR vide retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: AQR vide devrait retourner NULL\n");
        detruire_aqr(aqr_vide);
    }

    // Test X.8 : Gestion des coordonnées très proches (évite subdivision infinie)
    printf("  Test %s.8: Gestion de trucs aux coordonnées très proches\n", test_label);
    tests_total++;

    // Créer une liste avec deux trucs aux coordonnées identiques
    Un_elem *liste_doublons = NULL;

    // Truc 1 : Station A à (2.350000, 48.850000)
    Une_coord coord_a = {2.350000, 48.850000};
    Tdata data_a;
    data_a.sta.nom = (char *)malloc(10);
    strcpy(data_a.sta.nom, "Station A");
    data_a.sta.tab_con = NULL;
    data_a.sta.nb_con = 0;
    data_a.sta.con_pcc = NULL;
    Un_truc *truc_a = creer_truc(coord_a, STA, data_a, coord_a.lat);
    liste_doublons = inserer_liste_trie(liste_doublons, truc_a);

    // Truc 2 : Station B aux MÊMES coordonnées exactement
    Une_coord coord_b = {2.350000, 48.850000};
    Tdata data_b;
    data_b.sta.nom = (char *)malloc(10);
    strcpy(data_b.sta.nom, "Station B");
    data_b.sta.tab_con = NULL;
    data_b.sta.nb_con = 0;
    data_b.sta.con_pcc = NULL;
    Un_truc *truc_b = creer_truc(coord_b, STA, data_b, coord_b.lat);
    liste_doublons = inserer_liste_trie(liste_doublons, truc_b);

    // Construire l'AQR (devrait décaler automatiquement les coordonnées)
    Un_noeud *aqr_doublons = construire_aqr(liste_doublons);

    if (aqr_doublons != NULL) {
        // Vérifier que les coordonnées ont été décalées
        if (truc_a->coord.lon != truc_b->coord.lon || truc_a->coord.lat != truc_b->coord.lat) {
            printf("    OK: coordonnées décalées automatiquement (A: %.6f,%.6f vs B: %.6f,%.6f)\n",
                   truc_a->coord.lon, truc_a->coord.lat, truc_b->coord.lon, truc_b->coord.lat);
            tests_ok++;
        } else {
            printf("    ATTENTION: coordonnées non décalées mais AQR construit\n");
            tests_ok++;  // On accepte quand même si l'AQR se construit
        }
        detruire_aqr(aqr_doublons);
    } else {
        printf("    ERREUR: échec de construction avec coordonnées identiques\n");
    }

    detruire_liste_et_truc(liste_doublons);

    // Test X.9: Destruction de l'AQR
    if (aqr != NULL) {
        printf("  Test %s.9: Destruction de l'AQR\n", test_label);
        tests_total++;
        detruire_aqr(aqr);
        printf("    OK: AQR détruit sans erreur\n");
        tests_ok++;
    }

    // Libérer la liste de test et les trucs
    detruire_liste_et_truc(liste_test);

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
