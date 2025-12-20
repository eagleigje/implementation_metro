/*
 * PROJET MAIN3 / METRO
 * Interface graphique SDL2.
 */
#ifndef GUI_H
#define GUI_H

#include "config.h"

/**
 * Fonction qui lance l'interface graphique SDL2.
 * Cette fonction ouvre la fenêtre principale et gère la boucle d'événements.
 * @param config : Configuration chargée (peut être NULL si non disponible).
 * @return 0 si tout s'est bien passé, > 0 sinon.
 */
int gui_lancer(const Une_config *config);

#endif
