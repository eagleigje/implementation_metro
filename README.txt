========================================
        PROJET METRO 2025
========================================

DESCRIPTION GENERALE
--------------------
Ce projet permet de créer un plan interactif d'un réseau de transport en commun.
Il offre les fonctionnalités suivantes :
    - Représentation d'un ensemble de stations et connexions.
    - Gestion de l'échelle d'affichage et déplacement dans le plan.
    - Sélection graphique d'objets.
    - Calcul du plus court chemin entre deux stations (algorithme de Dijkstra).

STRUCTURE DU PROJET
-------------------
Le projet est organisé en plusieurs modules :

1. Structures de données de base :
   - coord.h       : Coordonnées géographiques (latitude/longitude)
   - station.h     : Définition des stations
   - connexion.h   : Définition des connexions entre stations
   - truc.h/truc.c : Structure unifiée pour stations et connexions

2. Structures de données avancées :
   - liste.h/liste.c         : Listes chaînées de trucs
   - abr.h/abr_type.h        : Arbres binaires de recherche (pour recherche rapide de stations)
   - aqrtopo.h               : Arbres quaternaires (pour représentation topologique)
   - ligne.h/ligne.c         : Gestion des lignes de métro

3. Configuration :
   - config.h/config.c       : Système de configuration .ini
   - config.ini              : Fichier de configuration principal
   - config_alt.ini          : Exemple de configuration alternative

4. Fichiers de données :
   - stations.csv : Fichier contenant les stations (longitude;latitude;nom), allégé pour les tests.
   - stations_paris.csv : Fichier contenant toutes les stations parisiennes (cf. LOCALISATION DES DONNEES).
   - lignes.csv : Fichier contenant les lignes (code;vitesse;intervalle;couleur).
   - connexions.csv : Fichier contenant les connexions (à créer).

5. Programmes optionnels à installer :
    - doxygen : Afin de générer la documentation technique du projet.
    - valgrind : Pour vérifier que le projet libère bien toute la mémoire à la sortie.
    - glibc-debug : Pour valgrind.

6. Documentation :
    - doc/projet-metro2025.pdf : Le PDF du projet
    - doc/projet/html/index.html : La documentation Doxygen du projet.

COMPILATION
-----------
Pour compiler le projet, utilisez la commande :
    make ou make build

Pour nettoyer les fichiers objets et l'exécutable :
    make clean

Pour tout nettoyer et tout recompiler :
    make all

Pour générer la documentation (doxygen doit être installé sur le PC) :
    make docs

Pour nettoyer la documentation :
    make clean-docs

EXECUTION
---------
Pour lancer l'application :
    ./metro

Pour utiliser un fichier de configuration différent :
    ./metro config_alt.ini
    ./metro /chemin/vers/ma_config.ini

Le programme de test actuel charge les stations et lignes depuis les fichiers CSV
et affiche des informations de base.

FICHIERS CSV
------------
Format du fichier stations.csv :
    longitude;latitude;nom_station

Exemple :
    2.354660;48.845990;Jussieu

Format du fichier lignes.csv :
    code ; vitesse ; intervalle ; couleur

Exemple :
    M1 ; 35.0 ; 3.0 ; #FFCD00

LOCALISATION DES DONNEES
------------------------
* Stations parisiennes : https://explore.data.gouv.fr/fr/datasets/6405b01aa7f6f0f5523c8653/?Commune%20nom__contains=Paris#/resources/5b645234-e6fd-4de8-889c-574a80531aca
* Le répertoire "data" inclus une extraction de tous les métros Français (metro-france.csv), et une extraction des métros parisiens à partir de ce fichier (metro-paris.csv).
* L'extraction "parisienne" a été réalisée en bash :
(head -1 metro-france.csv && grep \"Paris metro-france.csv) > metro-paris.csv

* On peut maintenant, extraire les données des métros parisiens dans le format demandé dans le sujet :
(séparateur "," puis tous les champs à partir du troisième, on dédoublonne, on supprime les " et enfin, on récupère les champs 2, 3 et 1)
cut -d "," -f3- < data/metro-paris.csv | sort | uniq | sed "s/\"//g" | awk -F "," '{print($2";"$3";"$1)}' > stations_paris.csv

EXERCICES A COMPLETER
----------------------
Le projet contient une base de travail avec les structures et quelques fonctions
implémentées. Il reste à compléter :

1. Exercice 1 : Fonctions de gestion des listes de stations (FAIT)
2. Exercice 2 : Arbres binaires de recherche pour les stations (À FAIRE)
3. Exercice 3 : Listes de lignes de métro (À FAIRE)
4. Exercice 4 : Gestion des connexions (À FAIRE)
5. Exercice 5 : Arbres quaternaires pour la topologie (À FAIRE)
6. Exercice 6 : Algorithme de Dijkstra pour le plus court chemin (À FAIRE)
7. Exercice 7 : Interface graphique (À FAIRE)

NOTES IMPORTANTES
-----------------
- Les coordonnées sont en degrés décimaux.
- Les vitesses sont en km/h.
- Les intervalles sont en minutes.
- Les couleurs sont au format hexadécimal #RRGGBB.
- Le champ user_val des trucs est utilisé pour :
  * Le tri par latitude dans les listes.
  * Le calcul des distances dans l'algorithme de Dijkstra.

CONTACT
-------
Pour toute question, référez-vous au document de spécification du projet.

Dernière mise à jour : 2025
