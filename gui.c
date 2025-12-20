/*
 * PROJET MAIN3 / METRO
 * Implementation de l'interface graphique SDL2.
 */
#include "gui.h"

#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

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
    Screen screen;
    Mode mode;
    float zoom;
    float pan_x;
    float pan_y;
    int last_click_x;
    int last_click_y;
} AppState;

/**
 * Met a jour le titre de la fenetre pour afficher le mode courant.
 * @param app Etat global de l'application graphique.
 */
static void gui_mettre_a_jour_titre(AppState *app) {
    const char *screen = (app->screen == SCREEN_MENU) ? "MENU" : "MAP";
    const char *mode = (app->mode == MODE_SELECT) ? "SELECT" : "PATH";
    char title[128];

    snprintf(title, sizeof(title), "PROJET METRO 2025 - %s (mode=%s)", screen, mode);
    SDL_SetWindowTitle(app->window, title);
}

/**
 * Affiche les controles dans la console.
 */
static void gui_afficher_aide(void) {
    printf("Controles:\n");
    printf("  MENU: 1=Afficher plan (selection), 2=Plus court chemin, 3=Quitter\n");
    printf("  PLAN: S=Selection, P=Plus court chemin, ESC=Menu, Q=Quitter\n");
    printf("        Fleches=Deplacer, +/-=Zoom, Click=Selection\n\n");
}

/**
 * Dessine l'ecran de menu principal.
 * @param app Etat global de l'application graphique.
 * @param width Largeur de la fenetre.
 * @param height Hauteur de la fenetre.
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
}

/**
 * Dessine une vue de plan simple avec une grille et un HUD minimal.
 * @param app Etat global de l'application graphique.
 * @param width Largeur de la fenetre.
 * @param height Hauteur de la fenetre.
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

    SDL_Rect hud = {12, 12, 220, 60};
    SDL_SetRenderDrawColor(app->renderer, 30, 30, 30, 200);
    SDL_RenderFillRect(app->renderer, &hud);
    SDL_SetRenderDrawColor(app->renderer, 240, 240, 240, 255);
    SDL_RenderDrawRect(app->renderer, &hud);

    if (app->last_click_x >= 0 && app->last_click_y >= 0) {
        SDL_Rect cursor = {app->last_click_x - 4, app->last_click_y - 4, 8, 8};
        SDL_SetRenderDrawColor(app->renderer, 200, 60, 60, 255);
        SDL_RenderFillRect(app->renderer, &cursor);
    }
}

/**
 * Lance la fenetre principale et la boucle d'evenements SDL2.
 * @param config Configuration chargee (peut etre NULL si non disponible).
 * @return 0 si tout s'est bien passe, > 0 sinon.
 */
int gui_lancer(const Une_config *config) {
    (void)config;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    AppState app = {
        .window = NULL,
        .renderer = NULL,
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
        1000,
        700,
        SDL_WINDOW_SHOWN
    );
    if (app.window == NULL) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (app.renderer == NULL) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        return 1;
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
    SDL_Quit();

    return 0;
}
