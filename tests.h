/*
 * PROJET MAIN3 / METRO
 * Fichier header pour les tests de validation du projet.
 */

#ifndef TESTS_H
#define TESTS_H

#include "config.h"
#include "liste.h"
#include "abr.h"

// Prototypes des fonctions de test

// Tests existants (Exercices 1-2)
void test_recherche_abr(Un_nabr *abr_stations);
void test_extraction_liste();

// Tests de cas limites et robustesse
void test_cas_limites_liste();      // Tests des cas limites pour les listes
void test_robustesse_abr();          // Tests de robustesse pour l'ABR
void test_config_invalide();         // Tests de configuration invalide
void test_csv_malformes();           // Tests de fichiers CSV malformés
void test_lignes(Une_config *config); // Tests de lecture et affichage des lignes
void test_gestion_memoire();         // Tests de gestion mémoire
void test_connexions(Une_config *config); // Tests de lecture et affichage des connexions

#endif
