/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 4 : Connexions.
 */

#include <stdio.h>

#include "test_exercice4.h"
#include "../ligne.h"
#include "../liste.h"
#include "../abr.h"
#include "../connexion.h"
#include "../tests.h"

/**
 * Fonction de test de lecture et d'affichage des connexions (Exercice 4).
 * @param config : La configuration du projet contenant le fichier des connexions.
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_connexions(Une_config *config, const char *test_label) {
    int tests_total = 0;
    int tests_ok = 0;

    printf("\033[1;34m%s. Tests de lecture et affichage des connexions\033[0m\n", test_label);

    // Test X.1 : Lecture du fichier connexions.csv
    printf("  Test %s.1: Lecture du fichier connexions.csv\n", test_label);
    tests_total++;

    // On doit d'abord charger les lignes et les stations
    Une_ligne *lignes = lire_lignes(config->fichier_lignes);
    if (lignes == NULL) {
        printf("    ÉCHEC: impossible de charger les lignes\n");
        printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
        return;
    }

    Un_elem *liste_stations = lire_stations(config->fichier_stations);
    if (liste_stations == NULL) {
        printf("    ÉCHEC: impossible de charger les stations\n");
        detruire_lignes(lignes);
        printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
        return;
    }

    Un_nabr *abr_stations = construire_abr(liste_stations);
    if (abr_stations == NULL) {
        printf("    ÉCHEC: impossible de construire l'ABR des stations\n");
        detruire_liste_et_truc(liste_stations);
        detruire_lignes(lignes);
        printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
        return;
    }

    Un_elem *connexions = lire_connexions(config->fichier_connexions, lignes, abr_stations);
    if (connexions == NULL) {
        printf("    ÉCHEC: fichier non trouvé ou vide\n");
    } else {
        printf("    OK: fichier lu avec succès\n");
        tests_ok++;
    }

    // Test X.2 : Vérification du contenu des connexions
    printf("  Test %s.2: Vérification que les connexions contiennent des données\n", test_label);
    tests_total++;
    if (connexions != NULL && connexions->truc != NULL && connexions->truc->type == CON) {
        printf("    OK: les connexions contiennent des données valides\n");
        tests_ok++;
    } else {
        printf("    ÉCHEC: les connexions sont invalides\n");
    }

    // Test X.3 : Affichage des connexions
    printf("  Test %s.3: Affichage des connexions\n", test_label);
    tests_total++;
    if (connexions != NULL) {
        printf("    Connexions chargées:\n");
        ecrire_connexions(stdout, connexions);
        printf("    OK: affichage réussi\n");
        tests_ok++;
    } else {
        printf("    ÉCHEC: aucune connexion à afficher\n");
    }

    // Test X.4 : Vérification du calcul automatique des temps
    printf("  Test %s.4: Vérification que les temps sont calculés (>0)\n", test_label);
    tests_total++;
    if (connexions != NULL && connexions->truc != NULL && connexions->truc->user_val > 0.0) {
        printf("    OK: temps de parcours calculé = %.1f minutes\n", connexions->truc->user_val);
        tests_ok++;
    } else {
        printf("    ÉCHEC: temps de parcours non calculé\n");
    }

    // Test X.5 : Vérification que tab_con est mis à jour dans les stations
    printf("  Test %s.5: Vérification que les stations ont leurs connexions (tab_con)\n", test_label);
    tests_total++;
    if (connexions != NULL && connexions->truc != NULL && connexions->truc->type == CON) {
        // On récupère la station de départ de la première connexion
        Un_truc *sta_dep = connexions->truc->data.con.sta_dep;
        if (sta_dep != NULL && sta_dep->data.sta.nb_con > 0 && sta_dep->data.sta.tab_con != NULL) {
            printf("    OK: station '%s' a %d connexion(s) dans tab_con\n",
                   sta_dep->data.sta.nom, sta_dep->data.sta.nb_con);
            tests_ok++;
        } else {
            printf("    ÉCHEC: tab_con non mis à jour pour la station de départ\n");
        }
    } else {
        printf("    ÉCHEC: impossible de tester (pas de connexions)\n");
    }

    // Test X.6 : Lecture d'un fichier connexions avec erreurs de champs
    printf("  Test %s.6: Lecture fichier connexions invalide (erreurs attendues)\n", test_label);
    tests_total++;
    char chemin_invalides[256];
    snprintf(chemin_invalides, sizeof(chemin_invalides), "%s/connexions_invalides.csv", TESTS_DATA_DIR);
    Un_elem *connexions_invalides = lire_connexions(chemin_invalides, lignes, abr_stations);
    if (connexions_invalides != NULL) {
        int nb_valides = 0;
        Un_elem *tmp_invalides = connexions_invalides;
        while (tmp_invalides != NULL) {
            nb_valides++;
            tmp_invalides = tmp_invalides->suiv;
        }
        if (nb_valides == 1) {
            printf("    OK: 1 connexion valide chargée (avertissements attendus)\n");
            tests_ok++;
        } else {
            printf("    ÉCHEC: %d connexion(s) valides (attendu: 1)\n", nb_valides);
        }
        detruire_liste_et_truc(connexions_invalides);
    } else {
        printf("    ÉCHEC: aucune connexion valide chargée\n");
    }

    // Et on libère la mémoire
    if (connexions != NULL) {
        detruire_liste_et_truc(connexions);
    }
    detruire_abr(abr_stations);
    detruire_liste_et_truc(liste_stations);
    detruire_lignes(lignes);

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);
}
