/*
 * PROJET MAIN3 / METRO
 * Tests de l'Exercice 6 : Plus court chemin (Dijkstra).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_exercice6.h"
#include "../dijkstra.h"
#include "../config.h"
#include "../truc.h"
#include "../liste.h"
#include "../ligne.h"

/**
 * Fonction qui crée une ligne minimale pour les tests.
 * Seul le code de la ligne est renseigné, les autres champs sont à NULL/0.
 *
 * @param code : Le code de la ligne (ex: "L1", "M1").
 * @return : Pointeur vers la ligne créée, ou NULL en cas d'échec d'allocation.
 */
static Une_ligne *creer_ligne_simple(const char *code) {
    Une_ligne *ligne = (Une_ligne *)malloc(sizeof(Une_ligne));
    if (ligne == NULL) return NULL;
    ligne->code = (char *)malloc(strlen(code) + 1);
    if (ligne->code == NULL) {
        free(ligne);
        return NULL;
    }
    strcpy(ligne->code, code);
    ligne->color = NULL;
    ligne->vitesse = 0.0f;
    ligne->intervalle = 0.0f;
    ligne->suiv = NULL;
    return ligne;
}

/**
 * Fonction qui libère une ligne créée par creer_ligne_simple().
 *
 * @param ligne : La ligne qui doit être libérée.
 */
static void detruire_ligne_simple(Une_ligne *ligne) {
    if (ligne == NULL) return;
    if (ligne->code != NULL) free(ligne->code);
    free(ligne);
}

/**
 * Fonction qui crée une station (truc de type STA) simple pour les tests.
 * La station est créée avec des coordonnées (0,0) et sans connexions.
 *
 * @param nom : Le nom de la station.
 * @return : Pointeur vers le truc station créé, ou NULL en cas d'échec d'allocation.
 */
static Un_truc *creer_station_simple(const char *nom) {
    Une_coord coord = {0.0f, 0.0f};
    Tdata data;
    data.sta.nom = (char *)malloc(strlen(nom) + 1);
    if (data.sta.nom == NULL) return NULL;
    strcpy(data.sta.nom, nom);
    data.sta.tab_con = NULL;
    data.sta.nb_con = 0;
    data.sta.con_pcc = NULL;
    return creer_truc(coord, STA, data, 0.0);
}

/**
 * Fonction qui ajoute une connexion dirigée entre deux stations.
 * La connexion est créée et automatiquement ajoutée au tableau tab_con de la station de départ.
 *
 * @param dep : La station de départ.
 * @param arr : La station d'arrivée.
 * @param ligne : La ligne de métro associée à cette connexion.
 * @param temps : Le temps de parcours de la connexion (en minutes).
 * @return : Pointeur vers le truc connexion créé, ou NULL en cas d'échec.
 */
static Un_truc *ajouter_connexion(Un_truc *dep, Un_truc *arr, Une_ligne *ligne, float temps) {
    Tdata data_con;
    data_con.con.sta_dep = dep;
    data_con.con.sta_arr = arr;
    data_con.con.ligne = ligne;
    Une_coord coord = {0.0f, 0.0f};
    Un_truc *con = creer_truc(coord, CON, data_con, temps);
    if (con == NULL) return NULL;

    dep->data.sta.nb_con++;
    Un_truc **nouveau = (Un_truc **)realloc(dep->data.sta.tab_con, dep->data.sta.nb_con * sizeof(Un_truc *));
    if (nouveau == NULL) {
        dep->data.sta.nb_con--;
        detruire_truc(con);
        return NULL;
    }
    dep->data.sta.tab_con = nouveau;
    dep->data.sta.tab_con[dep->data.sta.nb_con - 1] = con;
    return con;
}

/**
 * Fonction de test pour l'algorithme de Dijkstra (Exercice 6).
 * Teste le calcul du plus court chemin avec prise en compte :
 * - Du temps de parcours des connexions
 * - Du temps d'arrêt à chaque station
 * - De la pénalité de correspondance lors d'un changement de ligne
 *
 * @param test_label : Le numéro/label du test à afficher.
 */
void test_dijkstra(const char *test_label, Une_config *config) {
    printf("\033[1;34m%s. Tests de l'algorithme de Dijkstra (plus court chemin)\033[0m\n", test_label);
    int tests_total = 0;
    int tests_ok = 0;

    // Paramètre les temps à partir du fichier .ini si disponible, sinon valeurs par défaut.
    if (config != NULL) {
        configurer_dijkstra(config->temps_correspondance, config->temps_arret);
    } else {
        configurer_dijkstra(3.0f, 0.5f);
    }

    // Création des lignes
    Une_ligne *l1 = creer_ligne_simple("L1");
    Une_ligne *l2 = creer_ligne_simple("L2");

    // Création des stations
    Un_truc *sA = creer_station_simple("A");
    Un_truc *sB = creer_station_simple("B");
    Un_truc *sC = creer_station_simple("C");

    // Création des connexions
    Un_truc *cAB = ajouter_connexion(sA, sB, l1, 2.0f);  // A -> B (ligne L1, 2 min)
    Un_truc *cBC = ajouter_connexion(sB, sC, l1, 3.0f);  // B -> C (même ligne, 3 min)
    Un_truc *cAC = ajouter_connexion(sA, sC, l2, 10.0f); // A -> C (autre ligne, 10 min)

    // Construction de la liste de stations pour Dijkstra
    Un_elem *liste_sta = NULL;
    liste_sta = inserer_deb_liste(liste_sta, sA);
    liste_sta = inserer_deb_liste(liste_sta, sB);
    liste_sta = inserer_deb_liste(liste_sta, sC);

    // Paramétrage des temps pour les tests
    configurer_dijkstra(3.0f, 0.5f);

    // Test 1 : plus court chemin sans correspondance (A -> C via B sur L1)
    tests_total++;
    dijkstra(liste_sta, sA);
    float temps_attendu = 2.0f + 0.5f + 3.0f + 0.5f; // arrêts aux deux connexions, pas de correspondance
    if (sC->user_val < temps_attendu + 0.01f && sC->user_val > temps_attendu - 0.01f) {
        printf("  %s.1 OK: temps calculé A->C = %.2f min (attendu %.2f)\n", test_label, sC->user_val, temps_attendu);
        tests_ok++;
    } else {
        printf("  %s.1 ERREUR: temps A->C = %.2f (attendu %.2f)\n", test_label, sC->user_val, temps_attendu);
    }

    // Test 2 : vérification du chemin reconstitué (A -> B -> C)
    tests_total++;
    Un_elem *chemin = cherche_chemin(sC);
    if (chemin != NULL && chemin->truc == cAB && chemin->suiv != NULL && chemin->suiv->truc == cBC) {
        printf("  %s.2 OK: chemin reconstitué A->B->C\n", test_label);
        tests_ok++;
    } else {
        printf("  %s.2 ERREUR: chemin incorrect\n", test_label);
    }
    detruire_liste(chemin);

    // Test 3 : pénalité de correspondance (chemin direct A -> C sur L2 plus rapide si on augmente L1)
    tests_total++;
    // On rend la connexion A->C très courte pour forcer un choix avec correspondance
    cAC->user_val = 1.0f;
    // On ajoute une correspondance possible pour A -> B -> C avec changement L1 -> L2 (en modifiant ligne de B->C).
    cBC->data.con.ligne = l2;
    dijkstra(liste_sta, sA);
    if (sC->data.sta.con_pcc == cAC) {
        printf("  %s.3 OK: chemin direct préféré avec pénalité de correspondance\n", test_label);
        tests_ok++;
    } else {
        printf("  %s.3 ERREUR: correspondance non pénalisée comme attendu\n", test_label);
    }

    printf("  Bilan: %d/%d tests réussis\n\n", tests_ok, tests_total);

    // Libération mémoire
    detruire_truc(cAB);
    detruire_truc(cBC);
    detruire_truc(cAC);
    detruire_truc(sA);
    detruire_truc(sB);
    detruire_truc(sC);
    detruire_ligne_simple(l1);
    detruire_ligne_simple(l2);
    detruire_liste(liste_sta);
}
