/*
 * PROJET MAIN3 / METRO
 * Interface graphique SDL2.
 */
#ifndef GUI_H
#define GUI_H

#include "config.h"

/**
 * Lance l'interface graphique SDL2.
 * Cette fonction ouvre la fenetre principale et gere la boucle d'evenements.
 * @param config Configuration chargee (peut etre NULL si non disponible).
 * @return 0 si tout s'est bien passe, > 0 sinon.
 */
int gui_lancer(const Une_config *config);

#endif
