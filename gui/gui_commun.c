/*
 * PROJET MAIN3 / METRO
 * Implémentation des fonctions communes de l'interface graphique SDL2.
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"
#include "gui_commun.h"

/**
 * Fonction qui met à jour le titre de la fenêtre pour afficher le mode courant.
 * @param app : État global de l'application graphique.
 */
void gui_mettre_a_jour_titre(Etat_affichage *app) {
    // On détermine le texte à afficher selon l'écran et le mode actifs
    const char *ecran = (app->ecran == ECRAN_MENU) ? "MENU" : "PLAN";
    const char *mode = (app->mode == MODE_SELECTION) ? "SÉLECTION" : "CHEMIN";
    char titre[128];

    // On construit le titre et on l'affiche dans la barre de fenêtre
    snprintf(titre, sizeof(titre), "PROJET METRO 2025 - %s (mode=%s)", ecran, mode);
    SDL_SetWindowTitle(app->fenetre, titre);
}

/**
 * Fonction qui affiche les contrôles dans la console au lancement de la fenêtre.
 */
void gui_afficher_aide(void) {
    printf("Contrôles:\n");
    printf("  MENU: 1=Afficher plan (sélection), 2=Plus court chemin, 3=Quitter\n");
    printf("        Ou cliquer sur les cartes avec la souris\n");
    printf("  PLAN: S=Sélection, P=Plus court chemin, F=Rechercher, R=Réinitialiser, ESC=Menu, Q=Quitter\n");
    printf("        Flèches=Déplacer, +/-=Zoom, Molette=Zoom, Clic=Sélection\n");
    printf("        Clic droit + glisser = Déplacement\n\n");
}

/**
 * Fonction qui charge une police TrueType à partir de chemins connus.
 * @param taille : Taille en points de la police à charger.
 * @return : Un pointeur vers la police chargée, ou NULL si aucune n'est disponible.
 */
TTF_Font *gui_charger_police(int taille) {
    // Liste des chemins système connus où chercher la police DejaVu Sans
    const char *chemins[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        NULL
    };

    // On essaie chaque chemin jusqu'à trouver une police disponible
    for (int i = 0; chemins[i] != NULL; i++) {
        TTF_Font *police = TTF_OpenFont(chemins[i], taille);
        if (police != NULL) {
            return police;
        }
    }

    // Aucune police trouvée
    return NULL;
}

/**
 * Fonction qui imprime une ligne de texte à l'écran.
 * @param app : État global de l'application graphique.
 * @param police : Police à utiliser.
 * @param texte : Texte à afficher.
 * @param x : Position X.
 * @param y : Position Y.
 * @param couleur : Couleur du texte.
 */
void gui_rendu_texte(Etat_affichage *app, TTF_Font *police, const char *texte, int x, int y, SDL_Color couleur) {
    // Vérification des paramètres
    if (police == NULL || texte == NULL) {
        return;
    }

    // On génère une surface avec le texte rendu en UTF-8 avec antialiasing.
    SDL_Surface *surface = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (surface == NULL) {
        return;
    }

    // On convertit la surface en texture pour l'affichage GPU
    SDL_Texture *texture = SDL_CreateTextureFromSurface(app->moteurRendu, surface);
    if (texture != NULL) {
        // On définit la zone de destination avec la taille de la surface
        SDL_Rect dest = {x, y, surface->w, surface->h};
        SDL_RenderCopy(app->moteurRendu, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
    }
    // On libère la surface qui n'est plus nécessaire
    SDL_FreeSurface(surface);
}
