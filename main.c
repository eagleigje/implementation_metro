/*
 * PROJET MAIN3 / METRO
 * Programme principal.
*/

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "liste.h"

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

    // On libère toute la mémoire.
    printf("XX. Libération de la mémoire...\n");
    detruire_liste_et_truc(liste_stations);  // Detruit la liste et les stations
    detruire_config(config);
    printf("Mémoire libérée avec succès!\n\n");

    printf("=== FIN DES TESTS ===\n");

    return 0;
}
