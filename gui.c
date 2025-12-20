/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'interface graphique SDL2.
 */

#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"

typedef enum {
    SCREEN_MENU = 0,
    SCREEN_MAP = 1
} Screen;

typedef enum {
    MODE_SELECT = 0,
    MODE_PATH = 1
} Mode;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font_small;
    TTF_Font *font_large;
    Screen screen;
    Mode mode;
    float zoom;
    float pan_x;
    float pan_y;
    int last_click_x;
    int last_click_y;
} AppState;

/**
 * Fonction qui met à jour le titre de la fenêtre pour afficher le mode courant.
 * @param app : État global de l'application graphique.
 */
static void gui_mettre_a_jour_titre(AppState *app) {
    const char *screen = (app->screen == SCREEN_MENU) ? "MENU" : "MAP";
    const char *mode = (app->mode == MODE_SELECT) ? "SELECT" : "PATH";
    char title[128];

    snprintf(title, sizeof(title), "PROJET METRO 2025 - %s (mode=%s)", screen, mode);
    SDL_SetWindowTitle(app->window, title);
}

/**
 * Fonction qui affiche les contrôles dans la console au lancement de la fenêtre.
 */
static void gui_afficher_aide(void) {
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
static TTF_Font *gui_charger_police(int taille) {
    const char *paths[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/dejavu/DejaVuSans.ttf",
        NULL
    };

    for (int i = 0; paths[i] != NULL; i++) {
        TTF_Font *font = TTF_OpenFont(paths[i], taille);
        if (font != NULL) {
            return font;
        }
    }

    return NULL;
}

/**
 * Fonction qui rend une ligne de texte à l'écran.
 * @param app : État global de l'application graphique.
 * @param font : Police à utiliser.
 * @param text : Texte à afficher.
 * @param x : Position X.
 * @param y : Position Y.
 * @param color : Couleur du texte.
 */
static void gui_rendu_texte(AppState *app, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    if (font == NULL || text == NULL) {
        return;
    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    if (surface == NULL) {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    if (texture != NULL) {
        SDL_Rect dst = {x, y, surface->w, surface->h};
        SDL_RenderCopy(app->renderer, texture, NULL, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

/**
 * Fonction qui dessine l'écran, le menu principal.
 * @param app : État global de l'application graphique.
 * @param width : Largeur de la fenêtre.
 * @param height : Hauteur de la fenêtre.
 */
static void gui_dessiner_menu(AppState *app, int width, int height) {
    SDL_SetRenderDrawColor(app->renderer, 18, 24, 32, 255);
    SDL_RenderClear(app->renderer);

    SDL_Rect card1 = {width / 2 - 220, height / 2 - 140, 440, 70};
    SDL_Rect card2 = {width / 2 - 220, height / 2 - 50, 440, 70};
    SDL_Rect card3 = {width / 2 - 220, height / 2 + 40, 440, 70};

    SDL_SetRenderDrawColor(app->renderer, 52, 120, 196, 255);
    SDL_RenderFillRect(app->renderer, &card1);
    SDL_SetRenderDrawColor(app->renderer, 70, 156, 120, 255);
    SDL_RenderFillRect(app->renderer, &card2);
    SDL_SetRenderDrawColor(app->renderer, 160, 78, 78, 255);
    SDL_RenderFillRect(app->renderer, &card3);

    SDL_SetRenderDrawColor(app->renderer, 240, 240, 240, 255);
    SDL_RenderDrawRect(app->renderer, &card1);
    SDL_RenderDrawRect(app->renderer, &card2);
    SDL_RenderDrawRect(app->renderer, &card3);

    SDL_Color text_color = {245, 245, 245, 255};
    gui_rendu_texte(app, app->font_large, "PROJET METRO 2025", width / 2 - 150, height / 2 - 210, text_color);
    gui_rendu_texte(app, app->font_small, "1 - Afficher plan (sélection)", card1.x + 18, card1.y + 20, text_color);
    gui_rendu_texte(app, app->font_small, "2 - Plus court chemin", card2.x + 18, card2.y + 20, text_color);
    gui_rendu_texte(app, app->font_small, "3 - Quitter", card3.x + 18, card3.y + 20, text_color);
}

/**
 * Fonction qui dessine une vue de plan simple avec une grille et un panneau d'information.
 * @param app : État global de l'application graphique.
 * @param width : Largeur de la fenêtre.
 * @param height : Hauteur de la fenêtre.
 */
static void gui_dessiner_plan(AppState *app, int width, int height) {
    SDL_SetRenderDrawColor(app->renderer, 245, 240, 228, 255);
    SDL_RenderClear(app->renderer);

    SDL_SetRenderDrawColor(app->renderer, 210, 210, 210, 255);
    for (int x = 0; x < width; x += 40) {
        SDL_RenderDrawLine(app->renderer, x, 0, x, height);
    }
    for (int y = 0; y < height; y += 40) {
        SDL_RenderDrawLine(app->renderer, 0, y, width, y);
    }

    SDL_Rect panneau_info = {12, 12, 220, 60};
    SDL_SetRenderDrawColor(app->renderer, 30, 30, 30, 200);
    SDL_RenderFillRect(app->renderer, &panneau_info);
    SDL_SetRenderDrawColor(app->renderer, 240, 240, 240, 255);
    SDL_RenderDrawRect(app->renderer, &panneau_info);

    SDL_Color hud_color = {240, 240, 240, 255};
    if (app->mode == MODE_SELECT) {
        gui_rendu_texte(app, app->font_small, "Mode : SÉLECTION", panneau_info.x + 8, panneau_info.y + 8, hud_color);
    } else {
        gui_rendu_texte(app, app->font_small, "Mode : PLUS COURT CHEMIN", panneau_info.x + 8, panneau_info.y + 8, hud_color);
    }
    gui_rendu_texte(app, app->font_small, "S/P mode  -  ESC menu  -  Q quitter", panneau_info.x + 8, panneau_info.y + 32, hud_color);

    if (app->last_click_x >= 0 && app->last_click_y >= 0) {
        SDL_Rect cursor = {app->last_click_x - 4, app->last_click_y - 4, 8, 8};
        SDL_SetRenderDrawColor(app->renderer, 200, 60, 60, 255);
        SDL_RenderFillRect(app->renderer, &cursor);
    }
}

/**
 * Fonction qui lance la fenêtre principale et la boucle d'événements SDL2.
 * @param config : Configuration chargée (peut être NULL si non disponible).
 * @return 0 si tout s'est bien passé, > 0 sinon.
 */
int gui_lancer(const Une_config *config) {
    (void)config;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    AppState app = {
        .window = NULL,
        .renderer = NULL,
        .font_small = NULL,
        .font_large = NULL,
        .screen = SCREEN_MENU,
        .mode = MODE_SELECT,
        .zoom = 1.0f,
        .pan_x = 0.0f,
        .pan_y = 0.0f,
        .last_click_x = -1,
        .last_click_y = -1
    };

    app.window = SDL_CreateWindow(
        "PROJET METRO 2025",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GUI_LARGEUR_MIN,
        GUI_HAUTEUR_MIN,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (app.window == NULL) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (app.renderer == NULL) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_SetWindowMinimumSize(app.window, GUI_LARGEUR_MIN, GUI_HAUTEUR_MIN);

    app.font_small = gui_charger_police(18);
    app.font_large = gui_charger_police(26);
    if (app.font_small == NULL || app.font_large == NULL) {
        fprintf(stderr, "Attention: aucune police TTF n'a pu être chargée\n");
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
                if (app.screen == SCREEN_MENU) {
                    if (key == SDLK_1) {
                        app.screen = SCREEN_MAP;
                        app.mode = MODE_SELECT;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_2) {
                        app.screen = SCREEN_MAP;
                        app.mode = MODE_PATH;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_3 || key == SDLK_q) {
                        running = false;
                    }
                } else if (app.screen == SCREEN_MAP) {
                    if (key == SDLK_ESCAPE) {
                        app.screen = SCREEN_MENU;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_s) {
                        app.mode = MODE_SELECT;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_p) {
                        app.mode = MODE_PATH;
                        gui_mettre_a_jour_titre(&app);
                    } else if (key == SDLK_q) {
                        running = false;
                    } else if (key == SDLK_PLUS || key == SDLK_KP_PLUS) {
                        app.zoom *= 1.1f;
                    } else if (key == SDLK_MINUS || key == SDLK_KP_MINUS) {
                        app.zoom *= 0.9f;
                    } else if (key == SDLK_LEFT) {
                        app.pan_x -= 10.0f;
                    } else if (key == SDLK_RIGHT) {
                        app.pan_x += 10.0f;
                    } else if (key == SDLK_UP) {
                        app.pan_y -= 10.0f;
                    } else if (key == SDLK_DOWN) {
                        app.pan_y += 10.0f;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && app.screen == SCREEN_MAP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    app.last_click_x = event.button.x;
                    app.last_click_y = event.button.y;
                }
            }
        }

        int width = 0;
        int height = 0;
        SDL_GetWindowSize(app.window, &width, &height);
        if (app.screen == SCREEN_MENU) {
            gui_dessiner_menu(&app, width, height);
        } else {
            gui_dessiner_plan(&app, width, height);
        }
        SDL_RenderPresent(app.renderer);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    if (app.font_small != NULL) {
        TTF_CloseFont(app.font_small);
    }
    if (app.font_large != NULL) {
        TTF_CloseFont(app.font_large);
    }
    TTF_Quit();
    SDL_Quit();

    return 0;
}
