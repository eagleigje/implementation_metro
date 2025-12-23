/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'interface graphique SDL2 - Boucle principale.
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"
#include "gui_commun.h"
#include "gui_menu.h"
#include "gui_plan.h"
#include "../connexion.h"
#include "../dijkstra.h"
#include "../liste.h"
#include "../abr.h"
#include "../ligne.h"

/**
 * Fonction qui lance la fenêtre principale et la boucle d'événements SDL2.
 * @param config : Configuration chargée (peut être NULL si non disponible).
 * @return : 0 si tout s'est bien passé, > 0 sinon.
 */
int lancer_gui(const Une_config *config) {
    // Initialisation du sous-système vidéo de SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    // Initialisation de SDL2_ttf pour le rendu de texte
    if (TTF_Init() != 0) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialisation de la structure d'état de l'application
    Etat_affichage app = {
        .fenetre = NULL,
        .moteurRendu = NULL,
        .petite_police = NULL,
        .grande_police = NULL,
        .ecran = ECRAN_MENU,        // On démarre sur le menu principal
        .mode = MODE_SELECTION,     // Mode sélection par défaut
        .zoom = 1.0f,               // Zoom initial à 1:1
        .decalage_x = 0.0f,         // Pas de déplacement initial
        .decalage_y = 0.0f,
        .marge_plan = PLAN_MARGE_PLAN,
        .souris_x = -1,             // Position souris inconnue au départ
        .souris_y = -1,
        .souris_prec_x = -1,
        .souris_prec_y = -1,
        .deplacement_actif = 0,
        .dernier_clic_x = -1,       // Pas de clic enregistré (-1 = invalide)
        .dernier_clic_y = -1,
        .station_selectionnee = NULL,
        .station_depart = NULL,
        .station_arrivee = NULL,
        .chemin_courant = NULL,
        .recherche_active = 0,      // Recherche inactive au départ
        .recherche_ignorer_prochain = 0,
        .recherche_texte = "",      // Texte de recherche vide
        .recherche_resultat = NULL, // Aucun résultat de recherche
        .liste_stations = NULL,     // Données du métro non chargées
        .liste_connexions = NULL,
        .abr_stations = NULL,
        .lignes = NULL,
        .aqr_topo = NULL
    };

    // Création de la fenêtre principale, centrée à l'écran
    app.fenetre = SDL_CreateWindow(
        "PROJET METRO 2025",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GUI_LARGEUR_MIN,
        GUI_HAUTEUR_MIN,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE  // Fenêtre visible et redimensionnable
    );
    if (app.fenetre == NULL) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Création du moteur de rendu accéléré avec vsync
    app.moteurRendu = SDL_CreateRenderer(app.fenetre, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (app.moteurRendu == NULL) {
        fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(app.fenetre);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Active la gestion de la transparence des panneaux.
    if (SDL_SetRenderDrawBlendMode(app.moteurRendu, SDL_BLENDMODE_BLEND) != 0) {
        fprintf(stderr, "Avertissement : impossible d'activer le mode de fusion SDL\n");
    }

    // On impose une taille minimale pour la fenêtre
    SDL_SetWindowMinimumSize(app.fenetre, GUI_LARGEUR_MIN, GUI_HAUTEUR_MIN);

    // Chargement des polices TTF pour l'affichage de texte
    app.petite_police = gui_charger_police(TAILLE_PETITE_POLICE);
    app.grande_police = gui_charger_police(TAILLE_GRANDE_POLICE);
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
            app.liste_connexions = lire_connexions(config->fichier_connexions, app.lignes, app.abr_stations);
            if (app.liste_connexions == NULL) {
                fprintf(stderr, "Erreur: impossible de charger les connexions\n");
            }
        }

        // Configuration de l'algorithme de Dijkstra
        configurer_dijkstra(config->temps_correspondance, config->temps_arret);
        printf("Dijkstra configuré (correspondance=%.1f min, arrêt=%.1f min)\n",
               config->temps_correspondance, config->temps_arret);
    }

    // Initialisation finale de l'interface
    gui_mettre_a_jour_titre(&app);
    gui_afficher_aide();

    // Boucle principale de l'application
    bool running = true;
    while (running) {
        SDL_Event event;
        // On traite tous les événements en attente (clavier, souris, fermeture, etc.)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // L'utilisateur a fermé la fenêtre
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                // Dispatch des événements clavier vers les modules appropriés
                if (app.ecran == ECRAN_MENU) {
                    gui_menu_gerer_clavier(&app, key, &running);
                } else if (app.ecran == ECRAN_PLAN) {
                    gui_plan_gerer_clavier(&app, key, &running);
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Dispatch des clics souris vers les modules appropriés
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (app.ecran == ECRAN_MENU) {
                        gui_menu_gerer_clic(&app, event.button.x, event.button.y, &running);
                    } else if (app.ecran == ECRAN_PLAN) {
                        gui_plan_gerer_clic(&app, event.button.x, event.button.y);
                    }
                } else if (event.button.button == SDL_BUTTON_RIGHT && app.ecran == ECRAN_PLAN) {
                    app.deplacement_actif = 1;
                    app.souris_prec_x = event.button.x;
                    app.souris_prec_y = event.button.y;
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    app.deplacement_actif = 0;
                }
            } else if (event.type == SDL_MOUSEMOTION) {
                app.souris_x = event.motion.x;
                app.souris_y = event.motion.y;
                if (app.deplacement_actif && app.ecran == ECRAN_PLAN) {
                    int dx = app.souris_x - app.souris_prec_x;
                    int dy = app.souris_y - app.souris_prec_y;
                    app.decalage_x += dx;
                    app.decalage_y += dy;
                    app.souris_prec_x = app.souris_x;
                    app.souris_prec_y = app.souris_y;
                }
            } else if (event.type == SDL_MOUSEWHEEL && app.ecran == ECRAN_PLAN) {
                if (event.wheel.y > 0) {
                    app.zoom *= ZOOM_FACTEUR_AVANT;
                } else if (event.wheel.y < 0) {
                    app.zoom *= ZOOM_FACTEUR_ARRIERE;
                }
            } else if (event.type == SDL_TEXTINPUT && app.ecran == ECRAN_PLAN && app.recherche_active) {
                // Ignorer le premier événement (la touche F qui a activé la recherche)
                if (app.recherche_ignorer_prochain) {
                    app.recherche_ignorer_prochain = 0;
                } else {
                    // Ajouter le texte saisi à la recherche
                    int len = strlen(app.recherche_texte);
                    int input_len = strlen(event.text.text);
                    if (len + input_len < PLAN_RECHERCHE_MAX_TEXTE - 1) {
                        strcat(app.recherche_texte, event.text.text);
                        // Rechercher la station au fur et à mesure de la saisie (recherche par préfixe)
                        gui_plan_actualiser_recherche(&app);
                    }
                }
            }
        }

        // Dessin de l'écran selon l'état actif
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(app.fenetre, &width, &height);
        if (app.ecran == ECRAN_MENU) {
            gui_menu_dessiner(&app, width, height);
        } else {
            gui_dessiner_plan(&app, width, height);
        }
        // On affiche le rendu à l'écran
        SDL_RenderPresent(app.moteurRendu);
    }

    // Libération de la mémoire des données du métro
    // IMPORTANT : L'ordre de libération est très important.
    // On libère d'abord les connexions, puis les structures qui y font référence.
    if (app.chemin_courant != NULL) {
        // La liste du chemin contient uniquement des nœuds alloués, pas les trucs associés.
        // Une recherche de chemin le plus court a été lancée, il faut libérer la mémoire associée.
        detruire_liste(app.chemin_courant);
    }
    if (app.liste_connexions != NULL) {
        detruire_liste_et_truc(app.liste_connexions);
    }
    if (app.aqr_topo != NULL) {
        detruire_aqr(app.aqr_topo);
    }
    if (app.abr_stations != NULL) {
        detruire_abr(app.abr_stations);
    }
    if (app.liste_stations != NULL) {
        detruire_liste_et_truc(app.liste_stations);
    }
    if (app.lignes != NULL) {
        detruire_lignes(app.lignes);
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
