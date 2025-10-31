/*
 * PROJET MAIN3 / METRO
 * La localisation des objets est réalisée en utilisant leurs latitudes et longitudes exprimées en degrés sous forme
 * décimale (les orientations nord et est correspondent aux valeurs positives).
*/

#ifndef COORD_H
#define COORD_H

/**
 * Coordonnées d'un truc.
*/
typedef struct _une_coord {
    float lon; ///< Longitude décimale.
    float lat; ///< Latitude décimale.
} Une_coord;

#endif
