/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'algorithme de Dijkstra pour le plus court chemin.
 *
 * ALGORITHME DE DIJKSTRA
 * -----------------------
 * Nous pouvons énoncer l'algorithme de Dijkstra de la façon suivante :
 *
 * 1. Initialiser la valeur de la station d'origine (user_val) à 0.0.
 * 2. Initialiser la valeur de toutes les autres stations à +∞.
 * 3. Constituer Q un ensemble de toutes les stations ordonnées suivant leurs
 *    valeurs croissantes.
 * 4. Tant que Q est non vide en extraire la première station.
 * 5. Pour toutes les connexions partant de cette station, mettre à jour les
 *    valeurs des stations destinations de ces connexions si nécessaire (nouvelle
 *    valeur calculée du temps de parcours inférieure) tout en préservant l'ordre
 *    de l'ensemble Q. Si la valeur d'une station est mise à jour mémoriser la
 *    connexion. Répéter 4.
 */

#include <float.h>
#include <string.h>

#include "dijkstra.h"

#include "ligne.h"
#include "liste.h"
#include "truc.h"

/* Paramètres par défaut (configurables) pour le calcul des temps. */
static float TEMPS_CORRESPONDANCE = 3.0f;
static float TEMPS_ARRET = 0.5f;

/**
 * Fonction qui configure les paramètres globaux utilisés par l'algorithme Dijkstra.
 * @param temps_correspondance : Temps moyen de correspondance en minutes.
 * @param temps_arret : Temps moyen d'arrêt en station en minutes.
 */
void configurer_dijkstra(float temps_correspondance, float temps_arret) {
    if (temps_correspondance >= 0.0f) {
        TEMPS_CORRESPONDANCE = temps_correspondance;
    }
    if (temps_arret >= 0.0f) {
        TEMPS_ARRET = temps_arret;
    }
}

/**
 * Fonction qui initialise les valeurs des stations avant le calcul de Dijkstra.
 * Toutes les stations sont initialisées à une distance infinie (FLT_MAX),
 * sauf la station de départ qui est initialisée à 0.
 * Le champ con_pcc (connexion du plus court chemin) est mis à NULL.
 *
 * @param liste_sta : La liste des stations à initialiser.
 * @param sta_dep : La station de départ (initialisée à distance 0).
 */
static void initialiser_stations(Un_elem *liste_sta, Un_truc *sta_dep) {
    Un_elem *courant = liste_sta;
    // On parcourt toutes les stations de la liste
    while (courant != NULL) {
        if (courant->truc->type == STA) {
            // On initialise la distance à l'infini (station non encore atteinte)
            courant->truc->user_val = FLT_MAX;
            // On initialise la connexion du plus court chemin à NULL
            courant->truc->data.sta.con_pcc = NULL;
        }
        courant = courant->suiv;
    }
    // La station de départ a une distance nulle
    if (sta_dep != NULL) {
        sta_dep->user_val = 0.0f;
        sta_dep->data.sta.con_pcc = NULL;
    }
}

/**
 * Fonction qui construit la file de priorité (liste triée) à partir des stations.
 * Seules les stations (type STA) sont ajoutées à la file.
 * La liste est triée par user_val croissant (distance depuis la source).
 *
 * @param liste_sta : La liste des stations source.
 * @return : La file de priorité construite (liste triée).
 */
static Un_elem *construire_file(Un_elem *liste_sta) {
    Un_elem *file = NULL;
    Un_elem *courant = liste_sta;
    // On parcourt la liste des stations
    while (courant != NULL) {
        if (courant->truc->type == STA) {
            // On insère chaque station dans la file triée par user_val (distance).
            file = inserer_liste_trie(file, courant->truc);
        }
        courant = courant->suiv;
    }
    return file;
}

/**
 * Fonction qui implémente l'algorithme de Dijkstra pour calculer le plus court chemin
 * depuis une station de départ vers toutes les autres stations du réseau.
 *
 * L'algorithme prend en compte :
 * - Le temps de parcours de chaque connexion (stocké dans user_val de la connexion)
 * - Le temps d'arrêt à chaque station
 * - Le temps de correspondance lorsqu'on change de ligne
 *
 * Après exécution, chaque station contient dans user_val le temps minimal pour l'atteindre
 * depuis la station de départ, et dans con_pcc la dernière connexion empruntée pour
 * atteindre cette station (permettant de reconstituer le chemin).
 *
 * @param liste_sta : La liste de toutes les stations du réseau.
 * @param sta_dep : La station de départ du calcul.
 */
void dijkstra(Un_elem *liste_sta, Un_truc *sta_dep) {
    // Vérification des paramètres
    if (liste_sta == NULL || sta_dep == NULL) {
        return;
    }

    // On initialise toutes les stations (distance infinie sauf station de départ).
    initialiser_stations(liste_sta, sta_dep);
    // On construit la file de priorité triée par distance croissante
    Un_elem *file = construire_file(liste_sta);

    // Algorithme de Dijkstra : tant qu'il reste des stations à traiter
    while (file != NULL) {
        // On extrait la station avec la plus petite distance (tête de la file triée).
        Un_truc *u = extraire_deb_liste(&file);
        if (u == NULL || u->data.sta.nb_con == 0) {
            continue;
        }

        // On examine toutes les connexions sortantes de cette station
        for (unsigned int i = 0; i < u->data.sta.nb_con; i++) {
            Un_truc *con = u->data.sta.tab_con[i];
            if (con == NULL || con->type != CON) {
                continue;
            }
            // On récupère la station d'arrivée de cette connexion
            Un_truc *v = con->data.con.sta_arr;
            if (v == NULL || v->type != STA) {
                continue;
            }

            // On calcule le coût de cette connexion : temps de parcours + temps d'arrêt
            float cout = con->user_val + TEMPS_ARRET;
            // Si on change de ligne, on ajoute le temps de correspondance
            if (u->data.sta.con_pcc != NULL) {
                char *ligne_prec = u->data.sta.con_pcc->data.con.ligne->code;
                char *ligne_new = con->data.con.ligne->code;
                if (ligne_prec != NULL && ligne_new != NULL && strcmp(ligne_prec, ligne_new) != 0) {
                    cout += TEMPS_CORRESPONDANCE;
                }
            }

            // On calcule la distance alternative pour atteindre v en passant par u
            float alt = u->user_val + cout;
            // Si ce nouveau chemin est plus court que le chemin actuel vers v
            if (alt < v->user_val) {
                // On met à jour la distance minimale vers v
                v->user_val = alt;
                // On mémorise la connexion empruntée pour atteindre v
                v->data.sta.con_pcc = con;
                // On retire v de la file (sa distance a changé)
                extraire_liste(&file, v);
                // On réinsère v dans la file triée à sa nouvelle position
                file = inserer_liste_trie(file, v);
            }
        }
    }
}

/**
 * Fonction qui reconstruit le chemin depuis la station de départ jusqu'à une station d'arrivée,
 * en remontant les connexions stockées dans con_pcc.
 *
 * Cette fonction doit être appelée APRÈS l'exécution de dijkstra().
 * Elle utilise le champ con_pcc de chaque station pour remonter de l'arrivée vers le départ.
 *
 * @param sta_arr : La station d'arrivée dont on veut reconstituer le chemin.
 * @return : Une liste chaînée de connexions (Un_elem) représentant le chemin à emprunter,
 *           dans l'ordre (de la station de départ vers la station d'arrivée),
 *           ou NULL si aucun chemin n'existe ou si la station est invalide.
 */
Un_elem *cherche_chemin(Un_truc *sta_arr) {
    // Vérification de la station d'arrivée
    if (sta_arr == NULL || sta_arr->type != STA) {
        return NULL;
    }

    Un_elem *chemin = NULL;
    Un_truc *courant = sta_arr;

    // On remonte le chemin depuis l'arrivée jusqu'au départ
    // en suivant les connexions stockées dans con_pcc
    while (courant != NULL && courant->data.sta.con_pcc != NULL) {
        Un_truc *con = courant->data.sta.con_pcc;
        // On insère en début de liste pour avoir le chemin dans le bon ordre
        chemin = inserer_deb_liste(chemin, con);
        // On passe à la station de départ de cette connexion
        courant = con->data.con.sta_dep;
    }

    return chemin;
}
