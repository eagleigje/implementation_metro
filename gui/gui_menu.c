/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'écran menu de l'interface graphique SDL2.
 */

#include <SDL2/SDL.h>

#include "gui_menu.h"
#include "gui_commun.h"

/**
 * Fonction qui dessine l'écran du menu principal.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_menu_dessiner(Etat_affichage *app, int largeur, int hauteur) {
    // On efface l'écran avec la couleur de fond.
    Couleur fond = COULEUR_FOND_MENU;
    SDL_SetRenderDrawColor(app->moteurRendu, fond.r, fond.g, fond.b, fond.a);
    SDL_RenderClear(app->moteurRendu);

    // On définit les positions des trois cartes du menu, centrées à l'écran
    SDL_Rect carte1 = {largeur / 2 - MENU_LARGEUR_CARTE / 2, hauteur / 2 + MENU_OFFSET_CARTE_1_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};
    SDL_Rect carte2 = {largeur / 2 - MENU_LARGEUR_CARTE / 2, hauteur / 2 + MENU_OFFSET_CARTE_2_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};
    SDL_Rect carte3 = {largeur / 2 - MENU_LARGEUR_CARTE / 2, hauteur / 2 + MENU_OFFSET_CARTE_3_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};

    // On dessine les cartes avec des couleurs différentes pour chaque option
    Couleur c1 = COULEUR_BOUTON_MENU_1;
    SDL_SetRenderDrawColor(app->moteurRendu, c1.r, c1.g, c1.b, c1.a);
    SDL_RenderFillRect(app->moteurRendu, &carte1);
    Couleur c2 = COULEUR_BOUTON_MENU_2;
    SDL_SetRenderDrawColor(app->moteurRendu, c2.r, c2.g, c2.b, c2.a);
    SDL_RenderFillRect(app->moteurRendu, &carte2);
    Couleur c3 = COULEUR_BOUTON_MENU_3;
    SDL_SetRenderDrawColor(app->moteurRendu, c3.r, c3.g, c3.b, c3.a);
    SDL_RenderFillRect(app->moteurRendu, &carte3);

    // On ajoute les contours des cartes en blanc
    Couleur contour = COULEUR_CONTOUR_CARTE;
    SDL_SetRenderDrawColor(app->moteurRendu, contour.r, contour.g, contour.b, contour.a);
    SDL_RenderDrawRect(app->moteurRendu, &carte1);
    SDL_RenderDrawRect(app->moteurRendu, &carte2);
    SDL_RenderDrawRect(app->moteurRendu, &carte3);

    // On affiche les textes du menu
    SDL_Color couleur_texte = COULEUR_SDL(COULEUR_TEXTE_MENU);
    gui_rendu_texte(app, app->grande_police, TITRE_GUI, largeur / 2 - 150, hauteur / 2 + MENU_OFFSET_TITRE_Y, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "1 - Afficher plan (sélection)", carte1.x + MENU_MARGE_TEXTE_X, carte1.y + MENU_MARGE_TEXTE_Y, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "2 - Plus court chemin", carte2.x + MENU_MARGE_TEXTE_X, carte2.y + MENU_MARGE_TEXTE_Y, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "3 - Quitter", carte3.x + MENU_MARGE_TEXTE_X, carte3.y + MENU_MARGE_TEXTE_Y, couleur_texte);
}

/**
 * Fonction qui gère les événements clavier dans le menu.
 * @param app : État global de l'application graphique.
 * @param key : Code de la touche pressée.
 * @param running : Pointeur vers le drapeau d'exécution de la boucle principale.
 */
void gui_menu_gerer_clavier(Etat_affichage *app, SDL_Keycode key, bool *running) {
    // On est dans le menu : on accepte les touches 1, 2, 3 ou Q.
    if (key == SDLK_1) {
        app->ecran = ECRAN_PLAN;
        app->mode = MODE_SELECTION;
        gui_mettre_a_jour_titre(app);
    } else if (key == SDLK_2) {
        app->ecran = ECRAN_PLAN;
        app->mode = MODE_CHEMIN;
        gui_mettre_a_jour_titre(app);
    } else if (key == SDLK_3 || key == SDLK_q) {
        *running = false;
    }
}

/**
 * Fonction qui gère les clics souris dans le menu.
 * @param app : État global de l'application graphique.
 * @param x : Position X du clic.
 * @param y : Position Y du clic.
 * @param running : Pointeur vers le flag d'exécution de la boucle principale.
 */
void gui_menu_gerer_clic(Etat_affichage *app, int x, int y, bool *running) {
    // On récupère la taille de la fenêtre pour recalculer les positions des cartes
    int width, height;
    SDL_GetWindowSize(app->fenetre, &width, &height);

    // On recalcule les positions des cartes (comme dans gui_menu_dessiner).
    SDL_Rect carte1 = {width / 2 - MENU_LARGEUR_CARTE / 2, height / 2 + MENU_OFFSET_CARTE_1_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};
    SDL_Rect carte2 = {width / 2 - MENU_LARGEUR_CARTE / 2, height / 2 + MENU_OFFSET_CARTE_2_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};
    SDL_Rect carte3 = {width / 2 - MENU_LARGEUR_CARTE / 2, height / 2 + MENU_OFFSET_CARTE_3_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};

    SDL_Point point = {x, y};

    if (SDL_PointInRect(&point, &carte1)) {
        // Clic sur carte 1 : Afficher plan (sélection)
        app->ecran = ECRAN_PLAN;
        app->mode = MODE_SELECTION;
        gui_mettre_a_jour_titre(app);
    } else if (SDL_PointInRect(&point, &carte2)) {
        // Clic sur carte 2 : Plus court chemin
        app->ecran = ECRAN_PLAN;
        app->mode = MODE_CHEMIN;
        gui_mettre_a_jour_titre(app);
    } else if (SDL_PointInRect(&point, &carte3)) {
        // Clic sur carte 3 : Quitter
        *running = false;
    }
}
