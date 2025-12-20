/*
 * PROJET MAIN3 / METRO
 * Programme principal - Interface graphique (à implémenter pour Exercice 7).
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "gui.h"

/**
 * Fonction principale du projet.
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

    if (gui_lancer(config) != 0) {
        fprintf(stderr, "Erreur : l'interface graphique s'est terminée avec une erreur\n");
    }

    // Libération de la mémoire
    detruire_config(config);

    printf("=== FIN ===\n");

    return 0;
}
