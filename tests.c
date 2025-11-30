/*
 * PROJET MAIN3 / METRO
 * Programme de tests pour valider les fonctionnalités implémentées.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tests.h"
#include "config.h"
#include "liste.h"
#include "abr.h"
#include "ligne.h"

/**
 * Fonction qui permet de tester la recherche des noms de stations.
 * @param abr_stations : ABR des stations.
 */
void test_recherche_abr(Un_nabr *abr_stations) {
    char *stations_test[] = {
        "Jussieu",
        "jussieu",
        "Chatelet",
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
 * Fonction qui teste les fonctions d'extraction de liste.
 */
void test_extraction_liste() {
    printf("\033[1;34m5. Test des fonctions d'extraction de liste\033[0m\n");

    // Creation d'une liste de test avec quelques stations fictives
    Un_elem *liste_test = NULL;

    // Creation de 5 stations de test
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

    // Compter les elements initiaux
    int nb_initial = 0;
    Un_elem *temp = liste_test;
    while (temp != NULL) {
        nb_initial++;
        temp = temp->suiv;
    }
    printf("  Liste initiale: %d élèments\n", nb_initial);

    // Test 1: Extraction du premier element
    printf("  Test 1: Extraction du premier élèment\n");
    Un_truc *premier = extraire_deb_liste(&liste_test);
    if (premier != NULL) {
        printf("    Extrait: %s\n", premier->data.sta.nom);
        detruire_truc(premier);
    } else {
        printf("    ERREUR: extraction a échoué\n");
    }

    // Compter apres premiere extraction
    int nb_apres_1 = 0;
    temp = liste_test;
    while (temp != NULL) {
        nb_apres_1++;
        temp = temp->suiv;
    }
    printf("    Elèments restants: %d\n", nb_apres_1);

    // Test 2: Extraction d'un element au milieu
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

    // Compter apres deuxieme extraction
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

    // Liberation de la liste de test
    detruire_liste_et_truc(liste_test);
}

/**
 * Fonction qui teste les cas limites des listes.
 */
void test_cas_limites_liste() {
    printf("\033[1;34m6. Tests des cas limites pour les listes\033[0m\n");
    int tests_ok = 0;
    int tests_total = 0;

    // Test 6.1: Insertion dans une liste vide
    printf("  Test 6.1: Insertion dans une liste vide\n");
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

    // Test 6.2: Extraction sur liste vide
    printf("  Test 6.2: Extraction sur liste vide\n");
    tests_total++;
    Un_elem *liste_nulle = NULL;
    Un_truc *extrait = extraire_deb_liste(&liste_nulle);
    if (extrait == NULL && liste_nulle == NULL) {
        printf("    OK: extraction retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: comportement inattendu\n");
    }

    // Test 6.3: Limites de zone sur liste vide
    printf("  Test 6.3: Calcul limites sur liste vide\n");
    tests_total++;
    Une_coord lim_no, lim_se;
    limites_zone(NULL, &lim_no, &lim_se);
    printf("    OK: fonction ne se plante pas\n");
    tests_ok++;

    // Test 6.4: Insertion en début avec inserer_deb_liste
    printf("  Test 6.4: Insertion en début de liste\n");
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

/**
 * Fonction qui teste la robustesse de l'ABR.
 */
void test_robustesse_abr() {
    printf("\033[1;34m7. Tests de robustesse pour l'ABR\033[0m\n");
    int tests_ok = 0;
    int tests_total = 0;

    // Test 7.1: Recherche dans ABR vide
    printf("  Test 7.1: Recherche dans ABR vide\n");
    tests_total++;
    Un_truc *resultat_vide = chercher_station(NULL, "Test");
    if (resultat_vide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
    }

    // Test 7.2: Construction ABR depuis liste vide
    printf("  Test 7.2: Construction ABR depuis liste vide\n");
    tests_total++;
    Un_nabr *abr_vide = construire_abr(NULL);
    if (abr_vide == NULL) {
        printf("    OK: ABR vide retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: ABR devrait être NULL\n");
    }

    // Test 7.3: Recherche avec nom vide
    printf("  Test 7.3: Recherche avec nom vide\n");
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

    // Test 7.4: Recherche avec caractères spéciaux
    printf("  Test 7.4: Recherche avec caractères spéciaux\n");
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

/**
 * Fonction qui teste la gestion des configurations invalides.
 */
void test_config_invalide() {
    printf("\033[1;34m8. Tests de configuration invalide\033[0m\n");
    int tests_ok = 0;
    int tests_total = 0;

    // Test 8.1: Fichier de configuration inexistant
    printf("  Test 8.1: Fichier config inexistant\n");
    tests_total++;
    Une_config *config_invalide = lire_config("fichier_inexistant_12345.ini");
    if (config_invalide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
        detruire_config(config_invalide);
    }

    // Test 8.2: Destruction de config NULL
    printf("  Test 8.2: Destruction config NULL\n");
    tests_total++;
    detruire_config(NULL);
    printf("    OK: pas de crash\n");
    tests_ok++;

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}

/**
 * Fonction qui teste la lecture de fichiers CSV malformés.
 */
void test_csv_malformes() {
    printf("\033[1;34m9. Tests de fichiers CSV malformés\033[0m\n");
    int tests_ok = 0;
    int tests_total = 0;

    // Test 9.1: Fichier stations inexistant
    printf("  Test 9.1: Fichier stations inexistant\n");
    tests_total++;
    Un_elem *liste_invalide = lire_stations("stations_inexistantes_12345.csv");
    if (liste_invalide == NULL) {
        printf("    OK: retourne NULL\n");
        tests_ok++;
    } else {
        printf("    ERREUR: devrait retourner NULL\n");
        detruire_liste_et_truc(liste_invalide);
    }

    // Test 9.2: Fichier CSV vide
    printf("  Test 9.2: Fichier CSV vide\n");
    tests_total++;
    Un_elem *liste_vide = lire_stations("test_data/stations_vide.csv");
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

    // Test 9.3: Fichier CSV avec lignes malformées
    printf("  Test 9.3: Fichier CSV avec lignes malformées\n");
    tests_total++;
    Un_elem *liste_malformee = lire_stations("test_data/stations_malformees.csv");
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
 * Fonction qui teste la lecture et l'affichage des lignes de métro.
 */
void test_lignes() {
    printf("\033[1;34m10. Tests de lecture et affichage des lignes\033[0m\n");
    int tests_ok = 0;
    int tests_total = 0;

    // Test 10.1: Lecture du fichier lignes.csv
    printf("  Test 10.1: Lecture du fichier lignes.csv\n");
    tests_total++;
    Une_ligne *lignes = lire_lignes("lignes.csv");
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

    // Test 10.2: Affichage des lignes
    if (lignes != NULL) {
        printf("  Test 10.2: Affichage des lignes\n");
        tests_total++;
        printf("    Format: code ; vitesse ; intervalle ; couleur\n");
        afficher_lignes(lignes);
        printf("    OK: affichage réussi\n");
        tests_ok++;
    }

    // Test 10.3: Recherche d'une ligne existante
    if (lignes != NULL) {
        printf("  Test 10.3: Recherche ligne M1\n");
        tests_total++;
        Une_ligne *ligne_m1 = chercher_ligne(lignes, "M1");
        if (ligne_m1 != NULL) {
            printf("    OK: ligne M1 trouvée (vitesse=%.1f, intervalle=%.1f, couleur=%s)\n",
                   ligne_m1->vitesse, ligne_m1->intervalle, ligne_m1->color);
            tests_ok++;
        } else {
            printf("    ERREUR: ligne M1 non trouvée\n");
        }
    }

    // Test 10.4: Recherche d'une ligne inexistante
    if (lignes != NULL) {
        printf("  Test 10.4: Recherche ligne inexistante\n");
        tests_total++;
        Une_ligne *ligne_inconnue = chercher_ligne(lignes, "M99");
        if (ligne_inconnue == NULL) {
            printf("    OK: ligne M99 non trouvée (comportement attendu)\n");
            tests_ok++;
        } else {
            printf("    ERREUR: ligne M99 trouvée alors qu'elle ne devrait pas exister\n");
        }
    }

    // Test 10.5: Recherche sensible à la casse
    if (lignes != NULL) {
        printf("  Test 10.5: Recherche sensible à la casse\n");
        tests_total++;
        Une_ligne *ligne_min = chercher_ligne(lignes, "m1");
        if (ligne_min == NULL) {
            printf("    OK: recherche sensible à la casse (m1 != M1)\n");
            tests_ok++;
        } else {
            printf("    ATTENTION: recherche insensible à la casse\n");
        }
    }

    // Test 10.6: Lecture d'un fichier lignes inexistant
    printf("  Test 10.6: Lecture fichier lignes inexistant\n");
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

/**
 * Fonction qui teste la gestion mémoire avec destruction de structures.
 */
void test_gestion_memoire() {
    printf("\033[1;34m11. Tests de gestion mémoire\033[0m\n");
    int tests_ok = 0;
    int tests_total = 0;

    // Test 11.1: Destruction de truc NULL
    printf("  Test 11.1: Destruction truc NULL\n");
    tests_total++;
    detruire_truc(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    // Test 11.2: Destruction de liste vide
    printf("  Test 11.2: Destruction liste vide\n");
    tests_total++;
    detruire_liste(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    // Test 11.3: Destruction de liste_et_truc vide
    printf("  Test 11.3: Destruction liste_et_truc vide\n");
    tests_total++;
    detruire_liste_et_truc(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    // Test 11.4: Destruction ABR vide
    printf("  Test 11.4: Destruction ABR vide\n");
    tests_total++;
    detruire_abr(NULL);
    printf("    OK: pas de plantage\n");
    tests_ok++;

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}

/**
 * Fonction principale des tests.
 * @param argc : Le nombre d'arguments passés en paramètre.
 * @param argv : Les arguments de la ligne de commande.
 * @return : 0 si tout c'est bien passé, >0 sinon.
 */
int main(const int argc, char *argv[]) {
    // Nous utilisons un fichier de configuration par défaut.
    const char *fichier_config = "config.ini";

    // Mais on peut aussi le passer en paramètre.
    if (argc > 1) {
        // S'il y a un paramètre, on s'attend à ce que le premier soit un path vers un fichier de configuration.
        fichier_config = argv[1];
    }

    // Cyan gras (\033[1;36m) pour :
    printf("\033[1;36m=== TESTS - PROJET METRO 2025 ===\033[0m\n\n");

    // On charge la configuration du fichier de configuration.
    printf("Chargement de la configuration depuis %s...\n", fichier_config);
    Une_config *config = lire_config(fichier_config);
    if (config == NULL) {
        fprintf(stderr, "Erreur fatale: impossible de charger la configuration\n");
        return 1;
    }
    // On affiche ce que l'on a lu dans le fichier.
    afficher_config(config);

    // Test 1 : Lecture des stations.
    //- Bleu gras (\033[1;34m) pour tous les titres de sections :
    printf("\033[1;34m1. Lecture des stations depuis %s...\033[0m\n", config->fichier_stations);
    Un_elem *liste_stations = lire_stations(config->fichier_stations);
    if (liste_stations == NULL) {
        fprintf(stderr, "Erreur lors de la lecture des stations\n");
        detruire_config(config);
        return 1;
    }

    // Compter le nombre de stations chargées
    int nb_stations = 0;
    Un_elem *temp = liste_stations;
    while (temp != NULL) {
        nb_stations++;
        temp = temp->suiv;
    }
    printf("Stations chargées avec succès! (%d stations)\n\n", nb_stations);

    // Test 2 : Affichage des stations.
    printf("\033[1;34m2. Affichage des stations (triées par latitude):\033[0m\n");
    ecrire_liste(stdout, liste_stations);
    printf("\n");

    // Test 3 : calcul des zones
    printf("\033[1;34m3. Calcul des limites de la zone:\033[0m\n");
    Une_coord limite_no, limite_se;
    limites_zone(liste_stations, &limite_no, &limite_se);
    printf("Limite Nord-Ouest: lon=%.6f, lat=%.6f\n", limite_no.lon, limite_no.lat);
    printf("Limite Sud-Est: lon=%.6f, lat=%.6f\n\n", limite_se.lon, limite_se.lat);

    // Test 4 : Création de l'ABR.
    printf("\033[1;34m4. Création de l'ABR des stations\033[0m\n");
    Un_nabr *abr = construire_abr(liste_stations);
    if (abr == NULL) {
        fprintf(stderr, "Erreur lors de la construction de l'ABR.\n");
        detruire_liste_et_truc(liste_stations);
        detruire_config(config);
        return 1;
    }
    printf("ABR construit avec succès!\n\n");

    printf("4.1. Recherche de stations dans l'ABR (case-insensitive)\n");
    test_recherche_abr(abr);

    // Test 5 : Test des fonctions d'extraction
    test_extraction_liste();

    // Test 6 : Tests des cas limites pour les listes
    test_cas_limites_liste();

    // Test 7 : Tests de robustesse pour l'ABR
    test_robustesse_abr();

    // Test 8 : Tests de configuration invalide
    test_config_invalide();

    // Test 9 : Tests de fichiers CSV malformés
    test_csv_malformes();

    // Test 10 : Tests de lecture et affichage des lignes
    test_lignes();

    // Test 11 : Tests de gestion mémoire
    test_gestion_memoire();

    // On libère toute la mémoire.
    printf("XX. Libération de la mémoire...\n");
    detruire_liste_et_truc(liste_stations);  // Destruction de la liste et des stations
    detruire_abr(abr); // Destruction de l'ABR.
    detruire_config(config); // Destruction de la configuration de départ.
    printf("Mémoire libérée avec succès!\n\n");

    printf("\033[1;36m=== FIN DES TESTS ===\033[0m\n");

    return 0;
}
