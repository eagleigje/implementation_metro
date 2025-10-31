/*
 * PROJET MAIN3 / METRO
 * Librairie gérant les "trucs".
 * Les stations et les connexions sont analogues et contiennent pour une part le même type d’informations
 * (coordonnées, valeur), ils sont regroupés sous la forme d’une structure unique nommée truc et définie ici.
*/

#ifndef TRUC_H
#define TRUC_H

#include "coord.h"
#include "station.h"
#include "connexion.h"

/**
 * "Enumérateur", STA (station) ou CON (connexion).
 */
typedef enum _ttype {STA, CON} Ttype;

/**
 * Type qui stocke, soit une station, soit une connexion.
 */
typedef union _data {
    Une_station sta;    ///< Informations concernant une station.
    Une_connexion con;  ///< Informations concernant une connexion.
} Tdata;

/**
 * Un_truc stocke les informations des stations ou connexions utilisées partout dans le projet et arbres.
 */
typedef struct _un_truc {
    Une_coord coord;    ///< Coordonnées du truc.
    Ttype type;         ///< Type du truc, STA ou CON.
    Tdata data;         ///< Les informations de la station ou de la connexion.
    float user_val;     ///< La distance pour plus court chemin.
} Un_truc;

// Prototypes des fonctions
Un_truc *creer_truc(Une_coord coord, Ttype type, Tdata data, double uv);
void detruire_truc(Un_truc *truc);

#endif
