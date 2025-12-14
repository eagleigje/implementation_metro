/*
 * PROJET MAIN3 / METRO
 * Algorithme de Dijkstra pour le calcul du plus court chemin dans le réseau de métro.
 */

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "liste.h"

void dijkstra(Un_elem *liste_sta, Un_truc *sta_dep);
Un_elem *cherche_chemin(Un_truc *sta_arr);
void configurer_dijkstra(float temps_correspondance, float temps_arret);

#endif
