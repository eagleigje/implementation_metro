/*
 * PROJET MAIN3 / METRO
 * Implémentation de l'interface graphique SDL2.
 */

#include <stdio.h>
#include <SDL2/SDL_ttf.h>

#include "gui.h"
#include "liste.h"
#include "abr.h"
#include "aqrtopo.h"
#include "ligne.h"
#include "connexion.h"
#include "dijkstra.h"

/**
 * Type énuméré représentant les différents écrans de l'application.
 * ECRAN_MENU : Écran d'accueil avec les options de navigation.
 * ECRAN_PLAN : Écran principal affichant le plan du métro.
 */
typedef enum {
    ECRAN_MENU = 0,
    ECRAN_PLAN = 1
} Ecran;

/**
 * Type énuméré représentant les modes d'interaction disponibles sur le plan.
 * MODE_SELECTION : Permet de sélectionner des stations individuellement.
 * MODE_CHEMIN : Permet de calculer le plus court chemin entre deux stations.
 */
typedef enum {
    MODE_SELECTION = 0,
    MODE_CHEMIN = 1
} Mode;

/**
 * Structure contenant l'état global de l'interface graphique.
 * Regroupe les ressources SDL, les paramètres d'affichage et les données du métro.
 */
typedef struct {
    SDL_Window *fenetre;            // Fenêtre principale de l'application
    SDL_Renderer *moteurRendu;      // Moteur de rendu SDL pour dessiner à l'écran
    TTF_Font *petite_police;        // Police de petite taille pour les textes secondaires
    TTF_Font *grande_police;        // Police de grande taille pour les titres
    Ecran ecran;                    // Écran actuellement affiché (menu ou plan)
    Mode mode;                      // Mode d'interaction actif (sélection ou chemin)
    float zoom;                     // Facteur de zoom appliqué au plan (1.0 = échelle normale)
    float decalage_x;               // Décalage horizontal de la caméra en pixels
    float decalage_y;               // Décalage vertical de la caméra en pixels
    int dernier_clic_x;             // Position X du dernier clic souris (-1 si aucun)
    int dernier_clic_y;             // Position Y du dernier clic souris (-1 si aucun)
    // Données du métro
    Un_elem *liste_stations;        // Liste chaînée de toutes les stations
    Un_nabr *abr_stations;          // Arbre binaire de recherche pour recherche rapide par nom
    Une_ligne *lignes;              // Liste des lignes de métro
    Un_noeud *aqr_topo;             // Arbre quaternaire pour recherche spatiale des stations
} Etat_affichage;

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
    printf("  PLAN: S=Sélection, P=Plus court chemin, ESC=Menu, Q=Quitter\n");
    printf("        Flèches=Déplacer, +/-=Zoom, Clic=Sélection\n\n");
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
 * Fonction qui rend une ligne de texte à l'écran.
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

/**
 * Fonction qui dessine l'écran, le menu principal.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_dessiner_menu(Etat_affichage *app, int largeur, int hauteur) {
    // On efface l'écran avec la couleur de fond (bleu-gris foncé).
    Couleur fond = COULEUR_FOND_MENU;
    SDL_SetRenderDrawColor(app->moteurRendu, fond.r, fond.g, fond.b, fond.a);
    SDL_RenderClear(app->moteurRendu);

    // On définit les positions des trois cartes du menu, centrées à l'écran
    SDL_Rect carte1 = {largeur / 2 - MENU_LARGEUR_CARTE / 2, hauteur / 2 + MENU_OFFSET_CARTE_1_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};
    SDL_Rect carte2 = {largeur / 2 - MENU_LARGEUR_CARTE / 2, hauteur / 2 + MENU_OFFSET_CARTE_2_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};
    SDL_Rect carte3 = {largeur / 2 - MENU_LARGEUR_CARTE / 2, hauteur / 2 + MENU_OFFSET_CARTE_3_Y, MENU_LARGEUR_CARTE, MENU_HAUTEUR_CARTE};

    // On dessine les cartes avec des couleurs différentes pour chaque option
    Couleur c1 = COULEUR_CARTE_1;
    SDL_SetRenderDrawColor(app->moteurRendu, c1.r, c1.g, c1.b, c1.a);
    SDL_RenderFillRect(app->moteurRendu, &carte1);
    Couleur c2 = COULEUR_CARTE_2;
    SDL_SetRenderDrawColor(app->moteurRendu, c2.r, c2.g, c2.b, c2.a);
    SDL_RenderFillRect(app->moteurRendu, &carte2);
    Couleur c3 = COULEUR_CARTE_3;
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
    gui_rendu_texte(app, app->grande_police, "PROJET METRO 2025", largeur / 2 - 150, hauteur / 2 + MENU_OFFSET_TITRE_Y, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "1 - Afficher plan (sélection)", carte1.x + MENU_MARGE_TEXTE_X, carte1.y + MENU_MARGE_TEXTE_Y, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "2 - Plus court chemin", carte2.x + MENU_MARGE_TEXTE_X, carte2.y + MENU_MARGE_TEXTE_Y, couleur_texte);
    gui_rendu_texte(app, app->petite_police, "3 - Quitter", carte3.x + MENU_MARGE_TEXTE_X, carte3.y + MENU_MARGE_TEXTE_Y, couleur_texte);
}

/**
 * Fonction qui dessine une vue de plan simple avec une grille et un panneau d'information.
 * @param app : État global de l'application graphique.
 * @param largeur : Largeur de la fenêtre.
 * @param hauteur : Hauteur de la fenêtre.
 */
void gui_dessiner_plan(Etat_affichage *app, int largeur, int hauteur) {
    // On efface l'écran avec une couleur beige clair (style carte).
    Couleur fond = COULEUR_FOND_PLAN;
    SDL_SetRenderDrawColor(app->moteurRendu, fond.r, fond.g, fond.b, fond.a);
    SDL_RenderClear(app->moteurRendu);

    // On dessine une grille de repérage (quadrillage tous les N pixels).
    Couleur grille = COULEUR_GRILLE;
    SDL_SetRenderDrawColor(app->moteurRendu, grille.r, grille.g, grille.b, grille.a);
    for (int x = 0; x < largeur; x += PLAN_TAILLE_GRILLE) {
        SDL_RenderDrawLine(app->moteurRendu, x, 0, x, hauteur);
    }
    for (int y = 0; y < hauteur; y += PLAN_TAILLE_GRILLE) {
        SDL_RenderDrawLine(app->moteurRendu, 0, y, largeur, y);
    }

    // On dessine le panneau d'information en haut à gauche
    SDL_Rect panneau_info = {PLAN_PANNEAU_X, PLAN_PANNEAU_Y, PLAN_PANNEAU_LARGEUR, PLAN_PANNEAU_HAUTEUR};
    Couleur fond_panneau = COULEUR_PANNEAU_FOND;
    SDL_SetRenderDrawColor(app->moteurRendu, fond_panneau.r, fond_panneau.g, fond_panneau.b, fond_panneau.a);
    SDL_RenderFillRect(app->moteurRendu, &panneau_info);
    Couleur bordure_panneau = COULEUR_PANNEAU_BORDURE;
    SDL_SetRenderDrawColor(app->moteurRendu, bordure_panneau.r, bordure_panneau.g, bordure_panneau.b, bordure_panneau.a);
    SDL_RenderDrawRect(app->moteurRendu, &panneau_info);

    // On affiche le mode actif dans le panneau
    SDL_Color couleur_info = COULEUR_SDL(COULEUR_TEXTE_INFO);
    if (app->mode == MODE_SELECTION) {
        gui_rendu_texte(app, app->petite_police, "Mode : SÉLECTION", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_1, couleur_info);
    } else {
        gui_rendu_texte(app, app->petite_police, "Mode : PLUS COURT CHEMIN", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_1, couleur_info);
    }
    // On affiche les raccourcis clavier
    gui_rendu_texte(app, app->petite_police, "S/P mode  -  ESC menu  -  Q quitter", panneau_info.x + PLAN_MARGE_TEXTE_X, panneau_info.y + PLAN_MARGE_TEXTE_Y_2, couleur_info);

    // Si l'utilisateur a cliqué, on affiche un curseur rouge à la position du clic
    if (app->dernier_clic_x >= 0 && app->dernier_clic_y >= 0) {
        SDL_Rect curseur = {app->dernier_clic_x - PLAN_CURSEUR_DEMI_TAILLE, app->dernier_clic_y - PLAN_CURSEUR_DEMI_TAILLE, PLAN_CURSEUR_TAILLE, PLAN_CURSEUR_TAILLE};
        Couleur couleur_curseur = COULEUR_CURSEUR;
        SDL_SetRenderDrawColor(app->moteurRendu, couleur_curseur.r, couleur_curseur.g, couleur_curseur.b, couleur_curseur.a);
        SDL_RenderFillRect(app->moteurRendu, &curseur);
    }
}

/**
 * Fonction qui lance la fenêtre principale et la boucle d'événements SDL2.
 * @param config : Configuration chargée (peut être NULL si non disponible).
 * @return : 0 si tout s'est bien passé, > 0 sinon.
 */
int gui_lancer(const Une_config *config) {
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
        .dernier_clic_x = -1,       // Pas de clic enregistré (-1 = invalide)
        .dernier_clic_y = -1,
        .liste_stations = NULL,     // Données du métro non chargées
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
                // Gestion des touches selon l'écran actif
                if (app.ecran == ECRAN_MENU) {
                    // On est dans le menu : on accepte les touches 1, 2, 3 ou Q.
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
                    // On est sur le plan : touches ESC, S, P, Q, +/-, flèches
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
                        app.zoom *= ZOOM_FACTEUR_AVANT;  // Zoom avant
                    } else if (key == SDLK_MINUS || key == SDLK_KP_MINUS) {
                        app.zoom *= ZOOM_FACTEUR_ARRIERE;  // Zoom arrière
                    } else if (key == SDLK_LEFT) {
                        app.decalage_x -= DEPLACEMENT_PAS;  // Déplacement vers la gauche
                    } else if (key == SDLK_RIGHT) {
                        app.decalage_x += DEPLACEMENT_PAS;  // Déplacement vers la droite
                    } else if (key == SDLK_UP) {
                        app.decalage_y -= DEPLACEMENT_PAS;  // Déplacement vers le haut
                    } else if (key == SDLK_DOWN) {
                        app.decalage_y += DEPLACEMENT_PAS;  // Déplacement vers le bas
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && app.ecran == ECRAN_PLAN) {
                // Gestion des clics souris (uniquement sur le plan)
                if (event.button.button == SDL_BUTTON_LEFT) {
                    app.dernier_clic_x = event.button.x;
                    app.dernier_clic_y = event.button.y;
                }
            }
        }

        // Dessin de l'écran selon l'état actif
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(app.fenetre, &width, &height);
        if (app.ecran == ECRAN_MENU) {
            gui_dessiner_menu(&app, width, height);
        } else {
            gui_dessiner_plan(&app, width, height);
        }
        // On affiche le rendu à l'écran
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
