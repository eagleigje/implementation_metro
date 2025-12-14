/*
 * PROJET MAIN3 / METRO
 * Programme principal - Interface graphique (à implémenter pour Exercice 7).
 */

#include <stdio.h>
#include <string.h>

#include "config.h"

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
    printf("Interface graphique a implementer (Exercice 7)\n");
    printf("Pour lancer les tests de validation, executez: ./tests\n\n");

    // TODO: Implémenter l'interface graphique (Exercice 7)
    // - Design de l'interface
    // - Permettre à l'utilisateur de choisir les fonctionnalités
    // - Affichage du plan du métro
    // - Zoom et déplacement
    // - Sélection de stations
    // - Calcul et affichage du plus court chemin

    // Chargement de la configuration pour préparer les données
    printf("Chargement de la configuration depuis %s...\n", fichier_config);
    Une_config *config = lire_config(fichier_config);
    if (config == NULL) {
        fprintf(stderr, "Erreur: impossible de charger la configuration\n");
        return 1;
    }

    afficher_config(config);

    // Une_ligne *lignes = lire_lignes(config->fichier_lignes);
    // afficher_lignes(lignes);

    // Libération de la mémoire
    detruire_config(config);

    printf("=== FIN ===\n");

    return 0;
}
