/*
 * PROJET MAIN3 / METRO
 * Programme principal.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    for (int i = 0; stations_test[i] != NULL; i++) {
        Un_truc *station = chercher_station(abr_stations, stations_test[i]);
        if (station != NULL) {
            printf("  [TROUVE] %s - Position: (%.6f, %.6f)\n",
                   stations_test[i],
                   station->coord.lon,
                   station->coord.lat);
        } else {
            printf("  [NON TROUVE] %s\n", stations_test[i]);
        }
    }
    printf("\n");
}

/**
 * Fonction principale du projet.
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

    printf("=== PROJET METRO 2025 ===\n\n");

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
    printf("Stations chargées avec succès!\n\n");

    // Test 2 : Affichage des stations.
    printf("2. Affichage des stations (triées par latitude):\n");
    ecrire_liste(stdout, liste_stations);
    printf("\n");

    // Test 3 : calcul des zones
    printf("3. Calcul des limites de la zone:\n");
    Une_coord limite_no, limite_se;
    limites_zone(liste_stations, &limite_no, &limite_se);
    printf("Limite Nord-Ouest: lon=%.6f, lat=%.6f\n", limite_no.lon, limite_no.lat);
    printf("Limite Sud-Est: lon=%.6f, lat=%.6f\n\n", limite_se.lon, limite_se.lat);

    // Test 4 : Création de l'ABR.
    printf("4. Création de l'ABR des stations\n");
    Un_nabr *abr = construire_abr(liste_stations);
    if (abr == NULL) {
        fprintf(stderr, "Erreur lors de la construction de l'ABR.\n");
        detruire_liste_et_truc(liste_stations);
        detruire_config(config);
        return 1;
    }
    printf("ABR construit avec succes!\n\n");

    printf("4.1. Recherche de stations dans l'ABR\n");
    test_recherche_abr(abr);
    printf("\n");

    // On libère toute la mémoire.
    printf("XX. Libération de la mémoire...\n");
    detruire_liste_et_truc(liste_stations);  // Destruction de la liste et des stations
    detruire_abr(abr); // Destruction de l'ABR.
    detruire_config(config); // Destruction de la configuration de départ.
    printf("Mémoire libérée avec succès!\n\n");

    printf("=== FIN DES TESTS ===\n");

    return 0;
}
