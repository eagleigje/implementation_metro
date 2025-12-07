/*
 * PROJET MAIN3 / METRO
 *  Librairie qui définie une station.
 *  Une station est définie par son nom et l'ensemble des connexions qu'elle offre pour rejoindre d'autres stations.
*/

#ifndef STATION_H
#define STATION_H

/**
* Une station est définie par son nom et l'ensemble des connexions qu'elle offre pour rejoindre d'autres stations.
 */
typedef struct _une_station {
    char *nom;                  ///< Le nom de la station.
    struct _un_truc **tab_con;  ///< Tableau des connexions.
    unsigned int nb_con;        ///< Nombre de connexions.
    struct _un_truc *con_pcc;   ///< Connexion du plus court chemin
} Une_station;

#endif
