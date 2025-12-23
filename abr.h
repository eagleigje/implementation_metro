/*
 *  PROJET MAIN3 / METRO
 * Gestion des arbres binaires de recherche.
*/

#ifndef ABR_H
#define ABR_H

#include "liste.h"

// Prototypes des fonctions
Un_nabr *creer_nabr(Un_truc *truc);
Un_nabr *inserer_abr(Un_nabr *abr, Un_nabr *n);
Un_nabr *construire_abr(Un_elem *liste_sta);
void detruire_abr(Un_nabr *abr);
Un_truc *chercher_station(Un_nabr *abr, char *nom);
Un_truc *chercher_station_prefixe(Un_nabr *abr, const char *prefixe);

#endif
