/*
 *  PROJET MAIN3 / METRO
 * Définitions des constantes utilisées dans tout le programme.
*/

#ifndef PROJET_METRO_MAIN3_CONSTANTES_H
#define PROJET_METRO_MAIN3_CONSTANTES_H

// Constantes de gestion des CSV
#define CSV_SEPARATOR ';'               // Définition du séparateur par défaut des fichiers csv
#define CSV_TAMPON_TAILLE 512
// Les stations
#define CSV_MAX_STATIONS_LENGTH 512     // Taille maximale d'une ligne dans un fichier csv de stations.
#define STATION_NOM_TAILLE_MAX 256      // Taille maximale du nom d'une station, y compris le \0.
// Les lignes
#define CSV_MAX_LINE_LENGTH 256         // Taille maximale d'une ligne dans les fichiers CSV des lignes de métro.
#define LINE_MAX_CODE_SIZE 5        // La taille max du code d'une ligne de métro y compris le \0.
#define LINE_COLOR_SIZE 8           // La taille de la couleur d'une ligne exemple #808080, y compris le \0.
// Les connexions
#define CSV_MAX_CONNECTIONS_LENGTH 512  // Taille maximale d'une ligne dans un fichier csv de connexions.

#endif //PROJET_METRO_MAIN3_CONSTANTES_H
