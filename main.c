#include <stdio.h>
#include <stdlib.h>
#include "config.h"

int main(int argc, char *argv[]) {
    // Nous utilisons un fichier de configuration par défaut.
    const char *fichier_config = "config.ini";

    // Mais on peut aussi le passer en paramètre.
    if (argc > 1) {
        fichier_config = argv[1];
    }

    printf("=== PROJET METRO 2025 ===\n\n");

    // Charger la configuration
    printf("Chargement de la configuration depuis %s...\n", fichier_config);
    Une_config *config = lire_config(fichier_config);
    if (config == NULL) {
        fprintf(stderr, "Erreur fatale: impossible de charger la configuration\n");
        return 1;
    }
    afficher_config(config);

    printf("=== FIN DES TESTS ===\n");

    return 0;
}
