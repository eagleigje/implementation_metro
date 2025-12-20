/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'interface graphique SDL2.
 */

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"
#include "liste.h"
#include "abr.h"
#include "aqrtopo.h"
#include "ligne.h"
#include "connexion.h"
#include "dijkstra.h"

typedef enum {
    ECRAN_MENU = 0,
    ECRAN_PLAN = 1
} Ecran;

typedef enum {
    MODE_SELECTION = 0,
    MODE_CHEMIN = 1
} Mode;

typedef struct {
    SDL_Window *fenetre;
    SDL_Renderer *moteurRendu;
    TTF_Font *petite_police;
    TTF_Font *grande_police;
    Ecran ecran;
    Mode mode;
    float zoom;
    float decalage_x;
    float decalage_y;
    int dernier_clic_x;
    int dernier_clic_y;
    // Données du métro
    Un_elem *liste_stations;
    Un_nabr *abr_stations;
    Une_ligne *lignes;
    Un_noeud *aqr_topo;
} Etat_gui;

/**
 * Fonction qui met à jour le titre de la fenêtre pour afficher le mode courant.
 * @param app : État global de l'application graphique.
 */
void gui_mettre_a_jour_titre(Etat_gui *app) {
    const char *ecran = (app->ecran == ECRAN_MENU) ? "MENU" : "PLAN";
    const char *mode = (app->mode == MODE_SELECTION) ? "SÉLECTION" : "CHEMIN";
    char titre[128];

    snprintf(titre, sizeof(titre), "PROJET METRO 2025 - %s (mode=%s)", ecran, mode);
    SDL_SetWindowTitle(app->fenetre, titre);
}

/**
 * Fonction qui affiche les contrôles dans la console au lancement de la fenêtre.
 */
void gui_afficher_aide(void) {
    printf("Contrôles:\n");
    printf("  MENU: 1=Afficher plan (sélection), 2=Plus court chemin, 3=Quitter\n");
    printf("  PLAN: S=Sélection, P=Plus court chemin, ESC=Menu, Q=Quitter\n");
    printf("        Flèches=Déplacer, +/-=Zoom, Clic=Sélection\n\n");
}

/**
 * Fonction qui charge une police TrueType à partir de chemins connus.
 * @param taille : Taille en points de la police à charger.
 * @return Un pointeur vers la police chargée, ou NULL si aucune n'est disponible.
 */
TTF_Font *gui_charger_police(int taille) {
    const char *chemins[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        NULL
    };

    for (int i = 0; chemins[i] != NULL; i++) {
        TTF_Font *police = TTF_OpenFont(chemins[i], taille);
        if (police != NULL) {
            return police;
        }
    }

    return NULL;
}

/**
 * Fonction qui rend une ligne de texte à l'écran.
 * @param app : État global de l'application graphique.
 * @param police : Police à utiliser.
 * @param texte : Texte à afficher.
 * @param x : Position X.
 * @param y : Position Y.
 * @param couleur : Couleur du texte.
 */
void gui_rendu_texte(Etat_gui *app, TTF_Font *police, const char *texte, int x, int y, SDL_Color couleur) {
    if (police == NULL || texte == NULL) {
        return;
    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended(police, texte, couleur);
    if (surface == NULL) {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(app->moteurRendu, surface);
    if (texture != NULL) {
        SDL_Rect dest = {x, y, surface->w, surface->h};
        SDL_RenderCopy(app->moteurRendu, texture, NULL, &dest);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

/**
 * Fonction qui dessine l'écran, le menu principal.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_dessiner_menu(Etat_gui *app, int largeur, int hauteur) {
    SDL_SetRenderDrawColor(app->moteurRendu, 18, 24, 32, 255);
    SDL_RenderClear(app->moteurRendu);

    SDL_Rect carte1 = {largeur / 2 - 220, hauteur / 2 - 140, 440, 70};
    SDL_Rect carte2 = {largeur / 2 - 220, hauteur / 2 - 50, 440, 70};
    SDL_Rect carte3 = {largeur / 2 - 220, hauteur / 2 + 40, 440, 70};

    SDL_SetRenderDrawColor(app->moteurRendu, 52, 120, 196, 255);
    SDL_RenderFillRect(app->moteurRendu, &carte1);
    SDL_SetRenderDrawColor(app->moteurRendu, 70, 156, 120, 255);
    SDL_RenderFillRect(app->moteurRendu, &carte2);
    SDL_SetRenderDrawColor(app->moteurRendu, 160, 78, 78, 255);
    SDL_RenderFillRect(app->moteurRendu, &carte3);

    SDL_SetRenderDrawColor(app->moteurRendu, 240, 240, 240, 255);
    SDL_RenderDrawRect(app->moteurRendu, &carte1);
    SDL_RenderDrawRect(app->moteurRendu, &carte2);
    SDL_RenderDrawRect(app->moteurRendu, &carte3);

    SDL_Color couleur_texte = {245, 245, 245, 255};
    gui_rendu_texte(app, app->grande_police, "PROJET METRO 2025", largeur / 2 - 150, hauteur / 2 - 210, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "1 - Afficher plan (sélection)", carte1.x + 18, carte1.y + 20, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "2 - Plus court chemin", carte2.x + 18, carte2.y + 20, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "3 - Quitter", carte3.x + 18, carte3.y + 20, couleur_texte);
}

/**
 * Fonction qui dessine une vue de plan simple avec une grille et un panneau d'information.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_dessiner_plan(Etat_gui *app, int largeur, int hauteur) {
    SDL_SetRenderDrawColor(app->moteurRendu, 245, 240, 228, 255);
    SDL_RenderClear(app->moteurRendu);

    SDL_SetRenderDrawColor(app->moteurRendu, 210, 210, 210, 255);
    for (int x = 0; x < largeur; x += 40) {
        SDL_RenderDrawLine(app->moteurRendu, x, 0, x, hauteur);
    }
    for (int y = 0; y < hauteur; y += 40) {
        SDL_RenderDrawLine(app->moteurRendu, 0, y, largeur, y);
    }

    SDL_Rect panneau_info = {12, 12, 220, 60};
    SDL_SetRenderDrawColor(app->moteurRendu, 30, 30, 30, 200);
    SDL_RenderFillRect(app->moteurRendu, &panneau_info);
    SDL_SetRenderDrawColor(app->moteurRendu, 240, 240, 240, 255);
    SDL_RenderDrawRect(app->moteurRendu, &panneau_info);

    SDL_Color couleur_hud = {240, 240, 240, 255};
    if (app->mode == MODE_SELECTION) {
        gui_rendu_texte(app, app->petite_police, "Mode : SÉLECTION", panneau_info.x + 8, panneau_info.y + 8, couleur_hud);
    } else {
        gui_rendu_texte(app, app->petite_police, "Mode : PLUS COURT CHEMIN", panneau_info.x + 8, panneau_info.y + 8, couleur_hud);
    }
    gui_rendu_texte(app, app->petite_police, "S/P mode  -  ESC menu  -  Q quitter", panneau_info.x + 8, panneau_info.y + 32, couleur_hud);

    if (app->dernier_clic_x >= 0 && app->dernier_clic_y >= 0) {
        SDL_Rect curseur = {app->dernier_clic_x - 4, app->dernier_clic_y - 4, 8, 8};
        SDL_SetRenderDrawColor(app->moteurRendu, 200, 60, 60, 255);
        SDL_RenderFillRect(app->moteurRendu, &curseur);
    }
}

/**
 * Fonction qui lance la fenêtre principale et la boucle d'événements SDL2.
 * @param config : Configuration chargée (peut être NULL si non disponible).
 * @return 0 si tout s'est bien passé, > 0 sinon.
 */
int gui_lancer(const Une_config *config) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    Etat_gui app = {
        .fenetre = NULL,
        .moteurRendu = NULL,
        .petite_police = NULL,
        .grande_police = NULL,
        .ecran = ECRAN_MENU,
        .mode = MODE_SELECTION,
        .zoom = 1.0f,
        .decalage_x = 0.0f,
        .decalage_y = 0.0f,
        .dernier_clic_x = -1,
        .dernier_clic_y = -1,
        .liste_stations = NULL,
        .abr_stations = NULL,
        .lignes = NULL,
        .aqr_topo = NULL
    };

    app.fenetre = SDL_CreateWindow(
        "PROJET METRO 2025",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GUI_LARGEUR_MIN,
        GUI_HAUTEUR_MIN,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (app.fenetre == NULL) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    app.moteurRendu = SDL_CreateRenderer(app.fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (app.moteurRendu == NULL) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.fenetre);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowMinimumSize(app.fenetre, GUI_LARGEUR_MIN, GUI_HAUTEUR_MIN);

    app.petite_police = gui_charger_police(18);
    app.grande_police = gui_charger_police(26);
    if (app.petite_police == NULL || app.grande_police == NULL) {
        fprintf(stderr, "Attention: aucune police TTF n'a pu être chargée\n");
    }

    // Chargement des données du métro
    if (config != NULL) {
        printf("Chargement des stations depuis %s...\n", config->fichier_stations);
        app.liste_stations = lire_stations(config->fichier_stations);
        if (app.liste_stations == NULL) {
            fprintf(stderr, "Erreur: impossible de charger les stations\n");
        } else {
            // Construction de l'ABR pour la recherche rapide par nom
            printf("Construction de l'ABR des stations...\n");
            app.abr_stations = construire_abr(app.liste_stations);
            if (app.abr_stations == NULL) {
                fprintf(stderr, "Erreur: impossible de construire l'ABR\n");
            }

            // Construction de l'arbre quaternaire pour la recherche spatiale
            printf("Construction de l'arbre quaternaire...\n");
            app.aqr_topo = construire_aqr(app.liste_stations);
            if (app.aqr_topo == NULL) {
                fprintf(stderr, "Erreur: impossible de construire l'AQR\n");
            }
        }

        // Chargement des lignes de métro
        printf("Chargement des lignes depuis %s...\n", config->fichier_lignes);
        app.lignes = lire_lignes(config->fichier_lignes);
        if (app.lignes == NULL) {
            fprintf(stderr, "Erreur: impossible de charger les lignes\n");
        }

        // Chargement des connexions (nécessite stations et lignes)
        if (app.abr_stations != NULL && app.lignes != NULL) {
            printf("Chargement des connexions depuis %s...\n", config->fichier_connexions);
            Un_elem *liste_connexions = lire_connexions(config->fichier_connexions, app.lignes, app.abr_stations);
            if (liste_connexions == NULL) {
                fprintf(stderr, "Erreur: impossible de charger les connexions\n");
            } else {
                // Les connexions sont ajoutées automatiquement aux stations par lire_connexions
                // On peut libérer la liste des connexions, car elles sont référencées dans les stations.
                detruire_liste(liste_connexions);
            }
        }

        // Configuration de l'algorithme de Dijkstra
        configurer_dijkstra(config->temps_correspondance, config->temps_arret);
        printf("Dijkstra configuré (correspondance=%.1f min, arrêt=%.1f min)\n",
               config->temps_correspondance, config->temps_arret);
    }

    gui_mettre_a_jour_titre(&app);
    gui_afficher_aide();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                if (app.ecran == ECRAN_MENU) {
                    if (key == SDLK_1) {
                        app.ecran = ECRAN_PLAN;
                        app.mode = MODE_SELECTION;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_2) {
                        app.ecran = ECRAN_PLAN;
                        app.mode = MODE_CHEMIN;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_3 || key == SDLK_q) {
                        running = false;
                    }
                } else if (app.ecran == ECRAN_PLAN) {
                    if (key == SDLK_ESCAPE) {
                        app.ecran = ECRAN_MENU;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_s) {
                        app.mode = MODE_SELECTION;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_p) {
                        app.mode = MODE_CHEMIN;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_q) {
                        running = false;
                    } else if (key == SDLK_PLUS || key == SDLK_KP_PLUS) {
                        app.zoom *= 1.1f;
                    } else if (key == SDLK_MINUS || key == SDLK_KP_MINUS) {
                        app.zoom *= 0.9f;
                    } else if (key == SDLK_LEFT) {
                        app.decalage_x -= 10.0f;
                    } else if (key == SDLK_RIGHT) {
                        app.decalage_x += 10.0f;
                    } else if (key == SDLK_UP) {
                        app.decalage_y -= 10.0f;
                    } else if (key == SDLK_DOWN) {
                        app.decalage_y += 10.0f;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && app.ecran == ECRAN_PLAN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    app.dernier_clic_x = event.button.x;
                    app.dernier_clic_y = event.button.y;
                }
            }
        }

        int width = 0;
        int height = 0;
        SDL_GetWindowSize(app.fenetre, &width, &height);
        if (app.ecran == ECRAN_MENU) {
            gui_dessiner_menu(&app, width, height);
        } else {
            gui_dessiner_plan(&app, width, height);
        }
        SDL_RenderPresent(app.moteurRendu);
    }

    // Libération de la mémoire des données du métro
    if (app.aqr_topo != NULL) {
        detruire_aqr(app.aqr_topo);
    }
    if (app.abr_stations != NULL) {
        detruire_abr(app.abr_stations);
    }
    if (app.lignes != NULL) {
        detruire_lignes(app.lignes);
    }
    if (app.liste_stations != NULL) {
        detruire_liste_et_truc(app.liste_stations);
    }

    // Libération des ressources SDL
    SDL_DestroyRenderer(app.moteurRendu);
    SDL_DestroyWindow(app.fenetre);
    if (app.petite_police != NULL) {
        TTF_CloseFont(app.petite_police);
    }
    if (app.grande_police != NULL) {
        TTF_CloseFont(app.grande_police);
    }
    TTF_Quit();
    SDL_Quit();

    return 0;
}
