/*
 * PROJET MAIN3 / METRO
 * Programme de tests pour valider les fonctionnalités implémentées.
 */

#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "liste.h"
#include "abr.h"

/**
 * Fonction principale des tests.
 * @param argc : Le nombre d'arguments passés en paramètre.
 * @param argv : Les arguments de la ligne de commande.
 * @return : 0 si tout s'est bien passé, > 0 sinon.
 */
int main(const int argc, char *argv[]) {
    // Nous utilisons un fichier de configuration par défaut.
    const char *fichier_config = "config.ini";

    // Mais on peut aussi le passer en paramètre.
    if (argc > 1) {
        // S'il y a un paramètre, on s'attend à ce que le premier soit le path vers un fichier de configuration.
        fichier_config = argv[1];
    }

    // Cyan gras (\033[1;36m) pour :
    printf("\033[1;36m=== TESTS - PROJET METRO 2025 ===\033[0m\n\n");

    int sections_total = 0;

    // On charge la configuration du fichier de configuration.
    printf("Chargement de la configuration depuis %s...\n", fichier_config);
    Une_config *config = lire_config(fichier_config);
    if (config == NULL) {
        fprintf(stderr, "Erreur fatale: impossible de charger la configuration\n");
        return 1;
    }
    // On affiche ce que l'on a lu dans le fichier.
    afficher_config(config);

    // Test 1 : Tests du lecteur CSV générique
    test_csv_utils("1");
    sections_total++;

    // Test 2 : Lecture des stations.
    printf("\033[1;34m2. Lecture des stations depuis %s...\033[0m\n", config->fichier_stations);
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
    sections_total++;

    // Test 3 : Affichage des stations.
    printf("\033[1;34m3. Affichage des stations (triées par latitude):\033[0m\n");
    ecrire_liste(stdout, liste_stations);
    printf("\n");
    sections_total++;

    // Test 4 : calcul des zones
    printf("\033[1;34m4. Calcul des limites de la zone:\033[0m\n");
    Une_coord limite_no, limite_se;
    limites_zone(liste_stations, &limite_no, &limite_se);
    printf("Limite Nord-Ouest: lon=%.6f, lat=%.6f\n", limite_no.lon, limite_no.lat);
    printf("Limite Sud-Est: lon=%.6f, lat=%.6f\n\n", limite_se.lon, limite_se.lat);
    sections_total++;

    // Test 5 : Création de l'ABR.
    printf("\033[1;34m5. Création de l'ABR des stations\033[0m\n");
    Un_nabr *abr = construire_abr(liste_stations);
    if (abr == NULL) {
        fprintf(stderr, "Erreur lors de la construction de l'ABR.\n");
        detruire_liste_et_truc(liste_stations);
        detruire_config(config);
        return 1;
    }
    printf("ABR construit avec succès!\n\n");
    sections_total++;

    printf("5.1. Recherche de stations dans l'ABR (case-insensitive)\n");
    test_recherche_abr(abr);
    test_recherche_prefixe_abr("5.2");
    sections_total++;

    // Test 6 : Test des fonctions d'extraction
    test_extraction_liste("6");
    sections_total++;

    // Test 7 : Tests des cas limites pour les listes
    test_cas_limites_liste("7");
    sections_total++;

    // Test 8 : Tests de robustesse pour l'ABR
    test_robustesse_abr("8");
    sections_total++;

    // Test 9 : Tests de configuration invalide
    test_config_invalide("9");
    sections_total++;

    // Test 10 : Tests de fichiers CSV malformés
    test_csv_malformes("10");
    sections_total++;

    // Test 11 : Tests de lecture et affichage des lignes
    test_lignes(config, "11");
    sections_total++;

    // Test 12 : Tests de gestion mémoire
    test_gestion_memoire("12");
    sections_total++;

    // Test 13 : Tests de lecture et affichage des connexions
    test_connexions(config, "13");
    sections_total++;

    // Test 14 : Tests des arbres quaternaires
    test_arbres_quaternaires("14");
    sections_total++;

    // Test 15 : Dijkstra et plus court chemin
    test_dijkstra("15", config);
    sections_total++;

    // On libère toute la mémoire.
    printf("\033[1;34mFinal. Libération de la mémoire...\033[0m\n");
    detruire_liste_et_truc(liste_stations);  // Destruction de la liste et des stations
    detruire_abr(abr); // Destruction de l'ABR.
    detruire_config(config); // Destruction de la configuration de départ.
    printf("Mémoire libérée avec succès!\n\n");

    printf("Résumé des tests : %d section(s) exécutée(s), voir les bilans détaillés.\n\n", sections_total);

    printf("\033[1;36m=== FIN DES TESTS ===\033[0m\n");

    return 0;
}
