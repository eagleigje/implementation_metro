/*
 * PROJET MAIN3 / METRO
 * Programme principal - Interface graphique (à implémenter pour Exercice 7).
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "gui/gui.h"

/**
 * Fonction qui affiche l'aide d'utilisation du programme.
 */
static void afficher_aide_metro(void) {
    printf("Usage : ./metro [fichier.ini]\n");
    printf("Options :\n");
    printf("  -h, --help  Afficher cette aide et quitter\n");
    printf("\nExemples :\n");
    printf("  ./metro\n");
    printf("  ./metro config_paris.ini\n");
}

/**
 * Fonction principale du projet.
 * @param argc : Le nombre d'arguments passés en paramètre.
 * @param argv : Les arguments de la ligne de commande.
 * @return : 0 si tout s'est bien passé, > 0 sinon.
 */
int main(const int argc, char *argv[]) {
    // Nous utilisons un fichier de configuration par défaut.
    const char *fichier_config = "config.ini";

    int config_definie = 0;

    // Lecture minimale des arguments.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            afficher_aide_metro();
            return 0;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Erreur : option inconnue %s\n", argv[i]);
            afficher_aide_metro();
            return 1;
        } else if (!config_definie) {
            fichier_config = argv[i];
            config_definie = 1;
        } else {
            fprintf(stderr, "Erreur : argument inattendu %s\n", argv[i]);
            afficher_aide_metro();
            return 1;
        }
    }

    printf("=== PROJET METRO 2025 ===\n\n");
    printf("Interface graphique SDL2 (Exercice 7)\n");
    printf("Pour lancer les tests de validation, exécutez : ./tests\n\n");

    // Chargement de la configuration pour préparer les données
    printf("Chargement de la configuration depuis %s...\n", fichier_config);
    Une_config *config = lire_config(fichier_config);
    if (config == NULL) {
        fprintf(stderr, "Erreur: impossible de charger la configuration\n");
        return 1;
    }

    afficher_config(config);

    printf("Note : SDL2 et SDL2_ttf doivent être installés pour l'interface graphique.\n");

    if (lancer_gui(config) != 0) {
        fprintf(stderr, "Erreur : l'interface graphique s'est terminée avec une erreur\n");
    }

    // Libération de la mémoire
    detruire_config(config);

    printf("=== FIN ===\n");

    return 0;
}
