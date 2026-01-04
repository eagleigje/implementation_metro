========================================
        PROJET METRO 2025

    Développé par :
        Damien LE STRAT
        Clovis GAUTIER
========================================

Plan interactif d'un réseau de transport en commun avec interface SDL2.

INSTALLATION RAPIDE
-------------------
Outils de compilation :
    sudo apt update && sudo apt install make gcc    # Ubuntu/Debian
    sudo pacman -Syu base-devel                     # Arch Linux

Bibliothèques SDL2 :
    sudo apt install libsdl2-dev libsdl2-ttf-dev    # Ubuntu/Debian
    sudo pacman -S sdl2-compat sdl2_ttf             # Arch Linux

Compilation :
    make

Exécution :
    ./metro                 # Interface graphique
    ./metro config_paris.ini # Métro parisien complet
    ./tests                 # Programme de tests

COMPATIBILITÉ TESTÉE
--------------------
Ce projet a été testé avec succès sur :
- Arch Linux / KDE / X11 et Wayland
- Ubuntu 25.10 LTS / GNOME / Wayland

DOCUMENTATION COMPLÈTE
----------------------
Toute la documentation se trouve dans le répertoire doc/ :

doc/DOCUMENTATION_COMPLETE.txt
    Documentation complète du projet :
    - Structure des fichiers et modules
    - Instructions de compilation détaillées
    - Configuration et fichiers de données
    - État d'avancement des exercices
    - Workflow de développement

doc/ALGORITHMES.txt
    Explications détaillées des algorithmes :
    - Dijkstra (plus court chemin)
    - AQR (arbre quaternaire pour la recherche spatiale)
    - ABR (arbre binaire de recherche par nom)
    - Haversine (distances géographiques)
    Avec exemples, complexités et justifications.

doc/MODE_EMPLOI_GUI.txt
    Guide utilisateur de l'interface graphique :
    - Raccourcis clavier et souris
    - Mode sélection et mode chemin
    - Recherche par nom de station
    - Zoom et déplacement

doc/README_DONNEES.txt
    Extraction des données sources :
    - Commandes bash pour générer stations_paris.csv
    - Commandes bash pour générer lignes_paris.csv
    - Sources officielles (data.gouv.fr, IDFM)

doc/README_GENERER_CONNEXIONS.txt
    Script Python pour générer connexions_paris.csv
    - Utilisation du script
    - Format de sortie

doc/README_FONTS.txt
    Gestion de la police TrueType utilisée par le GUI.

doc/projet-metro2025.pdf
    Sujet du projet (PDF).

doc/projet/html/index.html
    Documentation technique Doxygen (générée avec "make docs").

AIDE RAPIDE
-----------
Compilation et tests :
    make                    # Compile metro et tests
    make run-tests          # Compile et exécute les tests
    make test-memory        # Vérifie les fuites mémoire (valgrind)

Mode debug (pour gdb) :
    make debug-metro        # Compile avec -O0
    make debug-memory-metro # Lance valgrind+gdb (vgdb)

Documentation :
    make docs               # Génère la doc Doxygen
    make clean-docs         # Supprime la doc générée

Nettoyage :
    make clean              # Supprime .o et exécutables

FICHIERS IMPORTANTS
-------------------
config.ini              # Configuration par défaut (tests)
config_paris.ini        # Configuration métro parisien complet

stations.csv            # Stations de test
stations_paris.csv      # Toutes les stations parisiennes

lignes.csv              # Lignes de test
lignes_paris.csv        # Toutes les lignes parisiennes

connexions.csv          # Connexions de test
connexions_paris.csv    # Connexions parisiennes (généré)

generer_connexions_paris.py  # Script de génération des connexions

RESSOURCES
----------
Documentation SDL2  : https://wiki.libsdl.org/SDL2/FrontPage
Tutoriels SDL2      : https://lazyfoo.net/tutorials/SDL/index.php
