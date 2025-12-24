/*
 * PROJET MAIN3 / METRO
 * Gestion des arbres quaternaires pour la représentation topologique.
 * Un arbre quaternaire divise l'espace en quatre quadrants (NO, SO, NE, SE) de manière récursive.
 */

#include <stdlib.h>
#include <math.h>

#include "aqrtopo.h"
#include "liste.h"

/**
 * Fonction qui calcule la distance au carré entre deux coordonnées.
 * On utilise le carré pour éviter le calcul trop lent de la racine carrée,
 * car on ne compare que des distances relatives.
 *
 * @param coord1 : Première coordonnée.
 * @param coord2 : Deuxième coordonnée.
 * @return : La distance au carré entre les deux points.
 */
float distance_carree(Une_coord coord1, Une_coord coord2) {
    float delta_lon = coord2.lon - coord1.lon;
    float delta_lat = coord2.lat - coord1.lat;
    return delta_lon * delta_lon + delta_lat * delta_lat;
}

/**
 * Fonction qui calcule un seuil de distance minimal de l'arbre quaternaire.
 * Le seuil est calculé automatiquement en fonction de la taille de la zone :
 * il correspond à environ 1/10000ème de la diagonale de la zone.
 *
 * Ce seuil permet d'éviter d'insérer deux trucs aux coordonnées trop proches,
 * ce qui causerait une subdivision infinie de l'arbre.
 *
 * @param limite_no : Coin Nord-Ouest de la zone.
 * @param limite_se : Coin Sud-Est de la zone.
 * @return : Le seuil de distance calculé (en degrés).
 */
float calculer_seuil_aqr(Une_coord limite_no, Une_coord limite_se) {
    // Seuil minimal : 0.00001° ≈ 1.1 mètre à Paris (évite division par zéro si la zone est nulle)
    const float SEUIL_MINIMAL = 0.00001f;

    // Calculer la diagonale de la zone
    float delta_lon = limite_se.lon - limite_no.lon;
    float delta_lat = limite_no.lat - limite_se.lat;
    float diagonale = sqrtf(delta_lon * delta_lon + delta_lat * delta_lat);

    // Le seuil est 1/10000ème de la diagonale
    // Pour Paris (~10km × 10km), cela donne environ 0.000014° ≈ 1.5 mètre.
    float seuil = diagonale / 10000.0f;

    // On s'assure qu'on a au moins le seuil minimal (cas où tous les points sont identiques).
    if (seuil < SEUIL_MINIMAL) {
        seuil = SEUIL_MINIMAL;
    }

    return seuil;
}

/**
 * Fonction qui insère un truc dans un arbre quaternaire de manière récursive.
 * L'arbre divise l'espace en quatre quadrants : Nord-Ouest, Sud-Ouest, Nord-Est, Sud-Est.
 *
 * @param aqr : L'arbre quaternaire dans lequel on veut insérer (peut être NULL).
 * @param limite_no : Coordonnée du coin Nord-Ouest de la zone couverte par ce nœud.
 * @param limite_se : Coordonnée du coin Sud-Est de la zone couverte par ce nœud.
 * @param truc : Le truc, à insérer (station ou connexion).
 * @return : L'arbre quaternaire mis à jour avec le nouveau truc.
 */
Un_noeud *inserer_aqr(Un_noeud *aqr, Une_coord limite_no, Une_coord limite_se, Un_truc *truc) {
    // Si l'arbre est vide, on crée un nouveau nœud
    if (aqr == NULL) {
        Un_noeud *nouveau = (Un_noeud *)malloc(sizeof(Un_noeud));
        if (nouveau == NULL) return NULL;

        nouveau->truc = truc;
        nouveau->limite_no = limite_no;
        nouveau->limite_se = limite_se;
        nouveau->no = NULL;
        nouveau->so = NULL;
        nouveau->ne = NULL;
        nouveau->se = NULL;

        return nouveau;
    }

    // Sinon, on calcule le centre de la zone actuelle
    Une_coord centre;
    centre.lon = (limite_no.lon + limite_se.lon) / 2.0; // Milieu en longitude (coupe la zone en deux).
    centre.lat = (limite_no.lat + limite_se.lat) / 2.0; // Milieu en latitude (coupe la zone en deux).

    // On détermine dans quel quadrant insérer le truc
    // On compare les coordonnées du truc avec le centre calculé
    if (truc->coord.lon < centre.lon) {
        // Partie ouest (gauche) : longitude plus petite que le centre
        if (truc->coord.lat >= centre.lat) {
            // Quadrant Nord-Ouest
            Une_coord nouvelle_limite_no = limite_no;
            Une_coord nouvelle_limite_se = centre;
            aqr->no = inserer_aqr(aqr->no, nouvelle_limite_no, nouvelle_limite_se, truc);
        } else {
            // Quadrant Sud-Ouest
            Une_coord nouvelle_limite_no;
            nouvelle_limite_no.lon = limite_no.lon;
            nouvelle_limite_no.lat = centre.lat;
            Une_coord nouvelle_limite_se;
            nouvelle_limite_se.lon = centre.lon;
            nouvelle_limite_se.lat = limite_se.lat;
            aqr->so = inserer_aqr(aqr->so, nouvelle_limite_no, nouvelle_limite_se, truc);
        }
    } else {
        // Partie est (droite) : longitude supérieure ou égale au centre
        if (truc->coord.lat >= centre.lat) {
            // Quadrant Nord-Est
            Une_coord nouvelle_limite_no;
            nouvelle_limite_no.lon = centre.lon;
            nouvelle_limite_no.lat = limite_no.lat;
            Une_coord nouvelle_limite_se;
            nouvelle_limite_se.lon = limite_se.lon;
            nouvelle_limite_se.lat = centre.lat;
            aqr->ne = inserer_aqr(aqr->ne, nouvelle_limite_no, nouvelle_limite_se, truc);
        } else {
            // Quadrant Sud-Est
            Une_coord nouvelle_limite_no = centre;
            Une_coord nouvelle_limite_se = limite_se;
            aqr->se = inserer_aqr(aqr->se, nouvelle_limite_no, nouvelle_limite_se, truc);
        }
    }

    return aqr;
}

/**
 * Fonction qui construit un arbre quaternaire à partir d'une liste de trucs.
 *
 * Cette fonction peut modifier les coordonnées des trucs si deux trucs se trouvent à
 * une distance inférieure au seuil calculé. Cela permet d'éviter une subdivision infinie de
 * l'arbre (cas où deux trucs ont les mêmes coordonnées).
 *
 * @param liste : La liste chaînée de trucs à insérer dans l'arbre.
 * @return : L'arbre quaternaire construit, ou NULL si la liste est vide.
 */
Un_noeud *construire_aqr(Un_elem *liste) {
    if (liste == NULL) {
        return NULL;
    }

    // On calcule les limites de la zone globale
    Une_coord limite_no, limite_se;
    limites_zone(liste, &limite_no, &limite_se);

    // On calcule le seuil de distance minimal automatiquement
    float seuil = calculer_seuil_aqr(limite_no, limite_se);

    // Et on construit l'arbre en insérant chaque élément
    Un_noeud *aqr = NULL;
    Un_elem *courant = liste;

    while (courant != NULL) {
        // On vérifie si un truc trop proche existe déjà dans l'arbre
        if (aqr != NULL) {
            Un_truc *plus_proche = chercher_aqr(aqr, courant->truc->coord);
            if (plus_proche != NULL) {
                float distance = distance_carree(courant->truc->coord, plus_proche->coord);

                // Si la distance est inférieure au seuil au carré, on décale légèrement
                if (distance < seuil * seuil) {
                    // Décalage en diagonale (lon et lat) pour éviter la collision
                    courant->truc->coord.lon += seuil;
                    courant->truc->coord.lat += seuil;
                }
            }
        }

        // Et on finit par insérer le truc dans l'arbre
        aqr = inserer_aqr(aqr, limite_no, limite_se, courant->truc);
        courant = courant->suiv;
    }

    return aqr;
}

/**
 * Fonction qui détruit un arbre quaternaire et libère toute la mémoire associée.
 * La destruction est récursive : on détruit d'abord les quatre sous-arbres, puis le nœud courant.
 *
 * Cette fonction ne libère pas les trucs contenus dans l'arbre, seulement les nœuds de l'arbre eux-mêmes.
 *
 * @param aqr : L'arbre quaternaire à détruire.
 */
void detruire_aqr(Un_noeud *aqr) {
    if (aqr == NULL) {
        return;
    }

    // On détruit récursivement les quatre sous-arbres
    detruire_aqr(aqr->no);
    detruire_aqr(aqr->so);
    detruire_aqr(aqr->ne);
    detruire_aqr(aqr->se);

    // Et on libère le nœud courant
    free(aqr);
}

/**
 * Fonction qui cherche le truc le plus proche d'une coordonnée donnée dans l'arbre quaternaire.
 * La recherche est optimisée en explorant d'abord le quadrant le plus proche,
 * puis en explorant les autres quadrants uniquement si c'est nécessaire.
 *
 * @param aqr : L'arbre quaternaire dans lequel chercher.
 * @param coord : La coordonnée de référence.
 * @return : Le truc le plus proche, ou NULL si l'arbre est vide.
 */
Un_truc *chercher_aqr(Un_noeud *aqr, Une_coord coord) {
    if (aqr == NULL) {
        return NULL;
    }

    // Le truc du nœud courant est un candidat potentiel.
    Un_truc *meilleur = aqr->truc;
    float meilleure_distance = distance_carree(coord, aqr->truc->coord);

    // On calcule le centre pour déterminer quel quadrant nous devons explorer en priorité
    Une_coord centre;
    centre.lon = (aqr->limite_no.lon + aqr->limite_se.lon) / 2.0;
    centre.lat = (aqr->limite_no.lat + aqr->limite_se.lat) / 2.0;

    // On détermine le quadrant prioritaire (celui qui contient le point recherché).
    Un_noeud *quadrant_proche = NULL;
    Un_noeud *quadrants_autres[3] = {NULL, NULL, NULL};
    int nb_autres = 0;

    if (coord.lon < centre.lon) {
        if (coord.lat >= centre.lat) {
            // Priorité au NO
            quadrant_proche = aqr->no;
            quadrants_autres[0] = aqr->so;
            quadrants_autres[1] = aqr->ne;
            quadrants_autres[2] = aqr->se;
            nb_autres = 3;
        } else {
            // Priorité au SO
            quadrant_proche = aqr->so;
            quadrants_autres[0] = aqr->no;
            quadrants_autres[1] = aqr->ne;
            quadrants_autres[2] = aqr->se;
            nb_autres = 3;
        }
    } else {
        if (coord.lat >= centre.lat) {
            // Priorité au NE
            quadrant_proche = aqr->ne;
            quadrants_autres[0] = aqr->no;
            quadrants_autres[1] = aqr->so;
            quadrants_autres[2] = aqr->se;
            nb_autres = 3;
        } else {
            // Priorité au SE
            quadrant_proche = aqr->se;
            quadrants_autres[0] = aqr->no;
            quadrants_autres[1] = aqr->so;
            quadrants_autres[2] = aqr->ne;
            nb_autres = 3;
        }
    }

    // On cherche maintenant dans le quadrant prioritaire déterminé.
    // L'idée est que le plus proche a de fortes chances d'être dans ce quadrant.
    if (quadrant_proche != NULL) {
        // Appel récursif pour descendre dans le sous-arbre le plus probable.
        Un_truc *candidat = chercher_aqr(quadrant_proche, coord);
        if (candidat != NULL) {
            float dist = distance_carree(coord, candidat->coord);
            if (dist < meilleure_distance) {
                meilleur = candidat;
                meilleure_distance = dist;
            }
        }
    }

    // On cherche dans les autres quadrants pour vérifier qu'il n'y a pas mieux.
    // Même si le point est dans un quadrant, un voisin peut être plus proche.
    for (int i = 0; i < nb_autres; i++) {
        if (quadrants_autres[i] != NULL) {
            // Appel récursif pour vérifier les autres sous-arbres.
            Un_truc *candidat = chercher_aqr(quadrants_autres[i], coord);
            if (candidat != NULL) {
                float dist = distance_carree(coord, candidat->coord);
                if (dist < meilleure_distance) {
                    meilleur = candidat;
                    meilleure_distance = dist;
                }
            }
        }
    }

    return meilleur;
}

/**
 * Fonction qui teste si deux rectangles se chevauchent.
 *
 * @param r1_no : Coin Nord-Ouest du rectangle 1.
 * @param r1_se : Coin Sud-Est du rectangle 1.
 * @param r2_no : Coin Nord-Ouest du rectangle 2.
 * @param r2_se : Coin Sud-Est du rectangle 2.
 * @return : 1 si les rectangles se chevauchent, 0 sinon.
 */
int rectangles_se_chevauchent(Une_coord r1_no, Une_coord r1_se, Une_coord r2_no, Une_coord r2_se) {
    // Les rectangles ne se chevauchent PAS si l'un est complètement à gauche/droite/au-dessus/en-dessous de l'autre.
    if (r1_se.lon < r2_no.lon) return 0;  // r1 est complètement à gauche de r2
    if (r1_no.lon > r2_se.lon) return 0;  // r1 est complètement à droite de r2
    if (r1_se.lat > r2_no.lat) return 0;  // r1 est complètement au-dessus de r2
    if (r1_no.lat < r2_se.lat) return 0;  // r1 est complètement en-dessous de r2

    return 1;  // Les rectangles se chevauchent
}

/**
 * Fonction qui cherche tous les trucs situés dans une zone rectangulaire donnée.
 * La recherche exploite la structure de l'arbre quaternaire pour ne parcourir
 * que les nœuds dont la zone "intersecte" la zone recherchée.
 *
 * @param aqr : L'arbre quaternaire dans lequel chercher.
 * @param liste : La liste accumulée des trucs trouvés (passée par récursion).
 * @param limite_no : Coin Nord-Ouest de la zone recherchée.
 * @param limite_se : Coin Sud-Est de la zone recherchée.
 * @return : La liste mise à jour avec les trucs trouvés dans la zone.
 */
Un_elem *chercher_zone(Un_noeud *aqr, Un_elem *liste, Une_coord limite_no, Une_coord limite_se) {
    if (aqr == NULL) {
        return liste;
    }

    // On vérifie si la zone du nœud intersecte la zone recherchée
    if (!rectangles_se_chevauchent(aqr->limite_no, aqr->limite_se, limite_no, limite_se)) {
        // Pas d'intersection, inutile de continuer dans cette branche
        return liste;
    }

    // On vérifie si le truc du nœud courant est dans la zone recherchée
    if (aqr->truc->coord.lon >= limite_no.lon &&
        aqr->truc->coord.lon <= limite_se.lon &&
        aqr->truc->coord.lat <= limite_no.lat &&
        aqr->truc->coord.lat >= limite_se.lat) {
        // Dans ce cas, on ajoute le truc à la liste
        liste = inserer_deb_liste(liste, aqr->truc);
    }

    // Et on cherche récursivement dans les 4 sous-arbres
    liste = chercher_zone(aqr->no, liste, limite_no, limite_se);
    liste = chercher_zone(aqr->so, liste, limite_no, limite_se);
    liste = chercher_zone(aqr->ne, liste, limite_no, limite_se);
    liste = chercher_zone(aqr->se, liste, limite_no, limite_se);

    return liste;
}
