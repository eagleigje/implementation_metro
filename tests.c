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
    printf("  Resultat: %d trouvees / %d recherches\n\n", nb_trouvees, nb_total);
}

/**
 * Fonction qui teste les fonctions d'extraction de liste.
 */
void test_extraction_liste() {
    printf("5. Test des fonctions d'extraction de liste\n");

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
    printf("  Liste initiale: %d elements\n", nb_initial);

    // Test 1: Extraction du premier element
    printf("  Test 1: Extraction du premier element\n");
    Un_truc *premier = extraire_deb_liste(&liste_test);
    if (premier != NULL) {
        printf("    Extrait: %s\n", premier->data.sta.nom);
        detruire_truc(premier);
    } else {
        printf("    ERREUR: extraction a echoue\n");
    }

    // Compter apres premiere extraction
    int nb_apres_1 = 0;
    temp = liste_test;
    while (temp != NULL) {
        nb_apres_1++;
        temp = temp->suiv;
    }
    printf("    Elements restants: %d\n", nb_apres_1);

    // Test 2: Extraction d'un element au milieu
    printf("  Test 2: Extraction d'un element au milieu\n");
    if (liste_test != NULL && liste_test->suiv != NULL) {
        Un_truc *milieu = liste_test->suiv->truc; // Deuxieme element
        char nom_milieu[100];
        strcpy(nom_milieu, milieu->data.sta.nom);

        Un_truc *extrait = extraire_liste(&liste_test, milieu);
        if (extrait != NULL) {
            printf("    Extrait: %s\n", nom_milieu);
            detruire_truc(extrait);
        } else {
            printf("    ERREUR: extraction a echoue\n");
        }
    }

    // Compter apres deuxieme extraction
    int nb_apres_2 = 0;
    temp = liste_test;
    while (temp != NULL) {
        nb_apres_2++;
        temp = temp->suiv;
    }
    printf("    Elements restants: %d\n", nb_apres_2);

    // Test 3: Extraction d'un element inexistant
    printf("  Test 3: Extraction d'un element inexistant\n");
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
        printf("    OK: element inexistant non trouve (comportement attendu)\n");
    } else {
        printf("    ERREUR: element inexistant trouve ?!\n");
    }
    detruire_truc(truc_fake);

    // Verification finale
    printf("  Bilan: %d -> %d -> %d elements\n", nb_initial, nb_apres_1, nb_apres_2);
    printf("  Tests d'extraction termines!\n\n");

    // Liberation de la liste de test
    detruire_liste_et_truc(liste_test);
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

    printf("=== TESTS - PROJET METRO 2025 ===\n\n");

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
    printf("1. Lecture des stations depuis %s...\n", config->fichier_stations);
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
    printf("Stations chargees avec succes! (%d stations)\n\n", nb_stations);

    // Test 2 : Affichage des stations.
    printf("2. Affichage des stations (triees par latitude):\n");
    ecrire_liste(stdout, liste_stations);
    printf("\n");

    // Test 3 : calcul des zones
    printf("3. Calcul des limites de la zone:\n");
    Une_coord limite_no, limite_se;
    limites_zone(liste_stations, &limite_no, &limite_se);
    printf("Limite Nord-Ouest: lon=%.6f, lat=%.6f\n", limite_no.lon, limite_no.lat);
    printf("Limite Sud-Est: lon=%.6f, lat=%.6f\n\n", limite_se.lon, limite_se.lat);

    // Test 4 : Création de l'ABR.
    printf("4. Creation de l'ABR des stations\n");
    Un_nabr *abr = construire_abr(liste_stations);
    if (abr == NULL) {
        fprintf(stderr, "Erreur lors de la construction de l'ABR.\n");
        detruire_liste_et_truc(liste_stations);
        detruire_config(config);
        return 1;
    }
    printf("ABR construit avec succes!\n\n");

    printf("4.1. Recherche de stations dans l'ABR (case-insensitive)\n");
    test_recherche_abr(abr);

    // Test 5 : Test des fonctions d'extraction
    test_extraction_liste();

    // On libère toute la mémoire.
    printf("XX. Liberation de la memoire...\n");
    detruire_liste_et_truc(liste_stations);  // Destruction de la liste et des stations
    detruire_abr(abr); // Destruction de l'ABR.
    detruire_config(config); // Destruction de la configuration de départ.
    printf("Memoire liberee avec succes!\n\n");

    printf("=== FIN DES TESTS ===\n");

    return 0;
}
