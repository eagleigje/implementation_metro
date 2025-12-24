/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 1 : Listes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_exercice1.h"
#include "../liste.h"
#include "../truc.h"

/**
 * Fonction de tests des fonctions d'extraction de liste.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_extraction_liste(const char *test_label) {
    printf("\033[1;34m%s. Test des fonctions d'extraction de liste\033[0m\n", test_label);

    // Création d'une liste de test avec quelques stations fictives
    Un_elem *liste_test = NULL;

    // Création de 5 stations de test
    for (int i = 1; i <= 5; i++) {
        Une_coord coord = {2.0f + i * 0.1f, 48.0f + i * 0.1f};
        Tdata data;
        char nom[50];
        sprintf(nom, "Station Test %d", i);
        data.sta.nom = (char *)malloc(strlen(nom) + 1);
        strcpy(data.sta.nom, nom);
        data.sta.tab_con = NULL;
        data.sta.nb_con = 0;
        data.sta.con_pcc = NULL;

        Un_truc *truc = creer_truc(coord, STA, data, 48.0f + i * 0.1f);
        liste_test = inserer_liste_trie(liste_test, truc);
    }

    // On compte les élèments initiaux
    int nb_initial = 0;
    Un_elem *temp = liste_test;
    while (temp != NULL) {
        nb_initial++;
        temp = temp->suiv;
    }
    printf("  Liste initiale: %d élèments\n", nb_initial);

    // Test 1: Extraction du premier élèment
    printf("  Test 1: Extraction du premier élèment\n");
    Un_truc *premier = extraire_deb_liste(&liste_test);
    if (premier != NULL) {
        printf("    Extrait: %s\n", premier->data.sta.nom);
        detruire_truc(premier);
    } else {
        printf("    ERREUR: extraction a échoué\n");
    }

    // On compte après la première extraction
    int nb_apres_1 = 0;
    temp = liste_test;
    while (temp != NULL) {
        nb_apres_1++;
        temp = temp->suiv;
    }
    printf("    Elèments restants: %d\n", nb_apres_1);

    // Test 2 : Extraction d'un élèment au milieu
    printf("  Test 2: Extraction d'un élèment au milieu\n");
    if (liste_test != NULL && liste_test->suiv != NULL) {
        Un_truc *milieu = liste_test->suiv->truc; // Deuxieme element
        char nom_milieu[100];
        strcpy(nom_milieu, milieu->data.sta.nom);

        Un_truc *extrait = extraire_liste(&liste_test, milieu);
        if (extrait != NULL) {
            printf("    Extrait: %s\n", nom_milieu);
            detruire_truc(extrait);
        } else {
            printf("    ERREUR: extraction a échoué\n");
        }
    }

    // On compte après une deuxième extraction
    int nb_apres_2 = 0;
    temp = liste_test;
    while (temp != NULL) {
        nb_apres_2++;
        temp = temp->suiv;
    }
    printf("    Elèments restants: %d\n", nb_apres_2);

    // Test 3: Extraction d'un element inexistant
    printf("  Test 3: Extraction d'un élèment inexistant\n");
    Une_coord coord_fake = {99.0f, 99.0f};
    Tdata data_fake;
    data_fake.sta.nom = (char *)malloc(20);
    strcpy(data_fake.sta.nom, "Fausse Station");
    data_fake.sta.tab_con = NULL;
    data_fake.sta.nb_con = 0;
    data_fake.sta.con_pcc = NULL;
    Un_truc *truc_fake = creer_truc(coord_fake, STA, data_fake, 99.0f);

    Un_truc *inexistant = extraire_liste(&liste_test, truc_fake);
    if (inexistant == NULL) {
        printf("    OK: élèment inexistant non trouvé (comportement attendu)\n");
    } else {
        printf("    ERREUR: élèment inexistant trouvé ?!\n");
    }
    detruire_truc(truc_fake);

    // Verification finale
    printf("  Bilan: %d -> %d -> %d élèments\n", nb_initial, nb_apres_1, nb_apres_2);
    printf("  Tests d'extraction terminés!\n\n");

    // Libération de la liste de test
    detruire_liste_et_truc(liste_test);
}

/**
 * Fonction de tests des cas limites pour les listes.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_cas_limites_liste(const char *test_label) {
    printf("\033[1;34m%s. Tests des cas limites pour les listes\033[0m\n", test_label);
    int tests_ok = 0;
    int tests_total = 0;

    // Test X.1 : Insertion dans une liste vide
    printf("  Test %s.1: Insertion dans une liste vide\n", test_label);
    tests_total++;
    Une_coord coord1 = {2.35f, 48.85f};
    Tdata data1;
    data1.sta.nom = (char *)malloc(20);
    strcpy(data1.sta.nom, "Station Vide");
    data1.sta.tab_con = NULL;
    data1.sta.nb_con = 0;
    data1.sta.con_pcc = NULL;
    Un_truc *truc1 = creer_truc(coord1, STA, data1, 48.85);
    Un_elem *liste_vide = NULL;
    liste_vide = inserer_liste_trie(liste_vide, truc1);
    if (liste_vide != NULL && liste_vide->truc == truc1 && liste_vide->suiv == NULL) {
        printf("    OK: insertion réussie\n");
        tests_ok++;
    } else {
        printf("    ERREUR: insertion échouée\n");
    }
    detruire_liste_et_truc(liste_vide);

    // Test X.2 : Extraction sur liste vide
    printf("  Test %s.2: Extraction sur liste vide\n", test_label);
    tests_total++;
    Un_elem *liste_nulle = NULL;
    Un_truc *extrait = extraire_deb_liste(&liste_nulle);
    if (extrait == NULL && liste_nulle == NULL) {
        printf("    OK: extraction retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: comportement inattendu\n");
    }

    // Test X.3 : Limites de zone sur liste vide
    printf("  Test %s.3: Calcul limites sur liste vide\n", test_label);
    tests_total++;
    Une_coord lim_no, lim_se;
    limites_zone(NULL, &lim_no, &lim_se);
    printf("    OK: fonction ne se plante pas\n");
    tests_ok++;

    // Test X.4 : Insertion en début avec inserer_deb_liste
    printf("  Test %s.4: Insertion en début de liste\n", test_label);
    tests_total++;
    Un_elem *liste_deb = NULL;
    Une_coord coord_deb = {2.36f, 48.86f};
    Tdata data_deb;
    data_deb.sta.nom = (char *)malloc(20);
    strcpy(data_deb.sta.nom, "Debut");
    data_deb.sta.tab_con = NULL;
    data_deb.sta.nb_con = 0;
    data_deb.sta.con_pcc = NULL;
    Un_truc *truc_deb = creer_truc(coord_deb, STA, data_deb, 48.86);
    liste_deb = inserer_deb_liste(liste_deb, truc_deb);
    if (liste_deb != NULL && liste_deb->truc == truc_deb) {
        printf("    OK: insertion en début réussie\n");
        tests_ok++;
    } else {
        printf("    ERREUR: insertion en début échouée\n");
    }
    detruire_liste_et_truc(liste_deb);

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
