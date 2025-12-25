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

INFORMATIONS COURS ABR
----------------------
Un arbre binaire de recherche est un arbre binaire dans lequel tout nœud a une clé qui est supérieure
à toutes les clés de son sous-arbre gauche et inférieure à toutes les clés de son sous-arbre droit.

STRUCTURE DU PROJET
-------------------
Le projet est organisé en plusieurs modules :
** La documentation technique au format Doxygen se trouve dans doc/projet/html/index.html **

1. Structures de données de base :
   - coord.h       : Coordonnées géographiques (latitude/longitude)
   - station.h     : Définition des stations
   - connexion.h   : Définition des connexions entre stations
   - truc.h/truc.c : Structure unifiée pour stations et connexions

2. Structures de données avancées :
   - liste.h/liste.c         : Listes chaînées de trucs
   - abr.h/abr.c/abr_type.h  : Arbres binaires de recherche (pour recherche rapide de stations)
   - aqrtopo.h/aqrtopo.c     : Arbres quaternaires (pour représentation topologique)
   - ligne.h/ligne.c         : Gestion des lignes de métro
   - dijkstra.h/dijkstra.c   : Algorithme de Dijkstra pour le calcul du plus court chemin

3. Utilitaires :
   - outils.h/outils.c       : Fonctions utilitaires génériques (trim, strdup_custom)
   - csv_utils.h/csv_utils.c : Lecteur CSV centralisé avec validation
   - constantes.h            : Constantes globales du projet

4. Programmes :
   - main.c                  : Point d'entrée de l'application principale
   - tests.c/tests.h         : Programme de tests et validation

5. Interface graphique (exercice 7) :
   Le répertoire gui/ contient l'implémentation de l'interface SDL2 :
   - gui/gui.h               : Types, constantes, structure Etat_affichage, fonction publique
   - gui/gui.c               : Boucle principale, initialisation/destruction SDL, dispatch des événements
   - gui/gui_commun.h/c      : Fonctions communes (polices, rendu texte, aide)
   - gui/gui_menu.h/c        : Écran menu (dessin, gestion clavier/souris)
   - gui/gui_plan.h/c        : Écran plan (dessin, gestion clavier/souris, grille, panneau info)

   Organisation modulaire pour faciliter la maintenance et l'évolution du code :
   - Séparation par écran
   - Fonctions communes centralisées dans gui_commun
   - Chaque module est indépendant

   Fonctionnalités GUI actuelles :
   - Affichage des stations et des connexions (couleur de ligne respectée)
   - Zoom (clavier +/-, molette) et déplacement (flèches, clic droit + glisser)
   - Réinitialisation de la vue (touche R)
   - Sélection de station au clic (mise en évidence + console)
   - Mode chemin (départ/arrivée) + calcul Dijkstra et affichage du chemin
   - Infobulles de station et de ligne au survol
   - Encart récapitulatif du chemin (avec temps total estimé)

6. Configuration :
   - config.h/config.c       : Système de configuration .ini
   - config.ini              : Fichier de configuration principal
   - config_paris.ini        : Fichier de configuration pour le métro parisien

7. Fichiers de données :
   - stations.csv : Fichier contenant les stations (longitude;latitude;nom), allégé pour les tests.
   - stations_paris.csv : Fichier contenant toutes les stations parisiennes (cf. LOCALISATION DES DONNEES).
   - lignes.csv : Fichier contenant les lignes (code;vitesse;intervalle;couleur).
   - lignes_paris.csv : Fichier contenant toutes les lignes parisiennes.
   - connexions.csv : Fichier contenant les connexions (test).
   - connexions_paris.csv : Fichier contenant toutes les connexions parisiennes (généré automatiquement).

8. Programmes à installer :
    - SDL2 (lib SDL2 + en-têtes) : Nécessaire pour compiler l'interface graphique.
    - SDL2_ttf : Nécessaire pour afficher du texte (noms de stations, lignes,
      panneaux d'information), car SDL2 ne fournit pas de rendu de texte nativement.
    - Police locale (optionnel) : si aucune police système n'est trouvée, le programme
      charge en dernier recours fonts/DejaVuSans.ttf (voir fonts/README_FONTS.txt).

    INSTALLATION SDL2 / SDL2_TTF (LINUX)
    ------------------------------------
    Ubuntu / Debian :
        sudo apt update
        sudo apt install libsdl2-dev libsdl2-ttf-dev

    Arch Linux :
        sudo pacman -S sdl2 sdl2_ttf

9. Programmes optionnels à installer :
    - Doxygen : Afin de générer la documentation technique du projet.
    - Valgrind : Pour vérifier que le projet libère bien toute la mémoire à la sortie.
    - glibc-debug : Pour valgrind.


10. Documentation :
    - ALGORITHMES.txt                  : Documentation détaillée des algorithmes principaux (Dijkstra, AQR, ABR, Haversine)
    - doc/projet-metro2025.pdf         : Le PDF du projet
    - doc/projet/html/index.html       : La documentation Doxygen du projet
    - README_GENERER_CONNEXIONS.txt    : Guide d'utilisation du script de génération des connexions
    - MODE_EMPLOI_GUI.txt              : Mode d'emploi de l'interface graphique (menus, clavier, souris)
    - fonts/README_FONTS.txt           : Gestion de la police utilisée par le GUI.

11. Scripts utilitaires :
    - generer_connexions_paris.py      : Script Python pour générer connexions_paris.csv automatiquement

COMPILATION
-----------
Prérequis (GUI) :
    - SDL2 et SDL2_ttf doivent être installés pour compiler l'exécutable `metro`.
    - Le programme `tests` ne dépend pas de SDL2.

Pour compiler le projet (les deux exécutables : metro et tests), utilisez :
    make
    ou
    make build

Pour compiler uniquement l'application principale :
    make metro

Pour compiler uniquement le programme de tests :
    make tests

Pour compiler et exécuter les tests immédiatement :
    make run-tests

Pour vérifier les fuites mémoire du programme de tests avec valgrind :
    make test-memory

Pour vérifier les fuites mémoire du programme principal avec valgrind :
    make test-memory-metro

Pour vérifier les fuites mémoire du programme principal avec valgrind et la configuration du métro parisien :
    make test-memory-metro-paris

Pour compiler les tests en mode debug (optimisation O0, pour gdb) :
    make debug-tests

Pour compiler metro en mode debug (optimisation O0, pour gdb) :
    make debug-metro

Pour exécuter les tests avec valgrind+gdb (vgdb) :
    make debug-memory-tests
    (Pour attacher gdb : dans un autre terminal, tapez 'gdb tests' puis 'target remote | vgdb')

Pour exécuter metro avec valgrind+gdb (vgdb) :
    make debug-memory-metro
    (Pour attacher gdb : dans un autre terminal, tapez 'gdb metro' puis 'target remote | vgdb')

Pour nettoyer les fichiers objets et les exécutables :
    make clean

Pour tout nettoyer et tout recompiler :
    make all

Pour générer la documentation (Doxygen doit être installé sur le PC) :
    make docs

Pour nettoyer la documentation :
    make clean-docs

MODES DE COMPILATION
--------------------
Le Makefile propose deux modes de compilation :

1. Mode NORMAL (par défaut) :
   - Optimisation O3 (performance maximale)
   - Symboles de débogage (-g) pour valgrind
   - Utilisé par : make, make metro, make tests, make test-memory*

2. Mode DEBUG :
   - Optimisation O0 (désactivée, pour faciliter le débogage avec gdb)
   - Symboles de débogage (-g)
   - Utilisé par : make debug-tests, make debug-metro, make debug-memory*
   - De mode est recommandé pour :
        - Déboguer avec gdb (step-by-step, inspection de variables)
        - Utiliser valgrind avec gdb (vgdb)

EXECUTION
---------
Le projet produit deux exécutables :

1. Programme de TESTS (pour validation et développement) :
    ./tests

   * Ces tests ont été réalisés par une IA afin de couvrir au maximum les différents cas possibles.

   Ce programme permet de :
   - Charger et valider les stations depuis le fichier CSV
   - Tester l'arbre binaire de recherche (recherche case-insensitive)
   - Tester les fonctions d'extraction de listes
   - Afficher des statistiques détaillées
   - Calculer les limites de la zone géographique.

   Pour utiliser un fichier de configuration différent :
    ./tests config_alt.ini
    ./tests /chemin/vers/ma_config.ini

   Pour tester le fichier de configuration du métro parisien :
    ./tests config_paris.ini

2. Application PRINCIPALE (interface graphique SDL2) :
    ./metro

   Cette application est l'interface graphique de l'utilisateur.
   Elle nécessite SDL2 et SDL2_ttf installés (voir section Installation).

   Pour un guide détaillé de l'interface graphique (menus, raccourcis, souris),
   voir MODE_EMPLOI_GUI.txt.

   Contrôles principaux :
   - MENU : 1=Afficher plan, 2=Plus court chemin, 3=Quitter
   - PLAN : S=Sélection, P=Chemin, R=Réinitialiser, ESC=Menu, Q=Quitter
   - Zoom : +/- ou molette souris
   - Déplacement : flèches ou clic droit + glisser
   - Sélection : clic gauche sur une station

   Pour utiliser un fichier de configuration différent :
    ./metro config_alt.ini
    ./metro /chemin/vers/ma_config.ini

   Pour utiliser le fichier de configuration du métro parisien :
    ./metro config_paris.ini

TESTS ET VALIDATION
-------------------
Le programme de tests (./tests) valide les fonctionnalités, au fur et à mesure.
Pour ajouter de nouveaux tests :

1. Créer une nouvelle fonction de test dans `tests.c`
2. Ajouter le prototype dans `tests.h`
3. Appeler la fonction dans le `main()` de `tests.c`
4. Suivre le même style de rapport (compteur tests_ok/tests_total)

Dans le répertoire tests_data, on trouve des fichiers mal formés pour les tests.
    * tests_data/config_invalide.ini
    * tests_data/stations_malformees.csv
    * tests_data/stations_vide.csv
    * tests_data/lignes_invalides.csv
    * tests_data/connexions_invalides.csv

FICHIERS CSV
------------
** Stations **
    Format du fichier stations.csv :
        longitude;latitude;nom_station

    Exemple :
        2.354660;48.845990;Jussieu

** Informations lignes **
    Format du fichier lignes.csv :
        code;vitesse;intervalle;couleur

    Exemple :
        M1;35.0;3.0;#FFCD00

** Connexions **
    Format du fichier connexions.csv :
        code_ligne;station_départ;station_arrivée;temps_parcours

    Exemple :
        M1;Châtelet;Gare de Lyon;2.2

    Le temps_parcours est le temps en minutes pour aller de la station de départ à la station d'arrivée.
    Ce temps est utilisé par l'algorithme de Dijkstra pour calculer le plus court chemin.

    Pour générer automatiquement le fichier connexions_paris.csv depuis les données réelles :
        python3 generer_connexions_paris.py

    Voir README_GENERER_CONNEXIONS.txt pour plus de détails sur la génération automatique.

LOCALISATION DES DONNEES
------------------------
** Stations **
    * Stations parisiennes : https://explore.data.gouv.fr/fr/datasets/6405b01aa7f6f0f5523c8653/?Commune%20nom__contains=Paris#/resources/5b645234-e6fd-4de8-889c-574a80531aca
    * Le répertoire "data" inclus une extraction de tous les métros Français (metro-france.csv), et une extraction des métros parisiens à partir de ce fichier (metro-paris.csv).
    * L'extraction "parisienne" a été réalisée en bash :
        Pour cela, on récupère les IDs des lignes (première colonne) parisiennes, puis on cherche à nouveaux
        les lignes en entier grâce à ces IDs (en effet, les lignes ne commencent ou ne finissent pas toutes à Paris).
        On conserve l'en-tête du fichier d'origine.
        (head -1 data/metro-france.csv && \
        grep -E -f <(grep '"Paris' data/metro-france.csv | cut -d',' -f1 | sort -u | sed 's/^/^/; s/$/,/') \
        data/metro-france.csv) > data/metro-paris.csv

    * On peut maintenant, extraire les données des métros parisiens dans le format demandé dans le sujet :
    (séparateur "," puis tous les champs à partir du troisième, on dédoublonne, on supprime les " et enfin, on récupère les champs 2, 3 et 1)
        cut -d "," -f3- < data/metro-paris.csv | sort | uniq | sed "s/\"//g" | awk -F "," '{print($2";"$3";"$1)}' > stations_paris.csv

** Informations lignes **
    * Informations concernant les lignes parisiennes : https://data.iledefrance-mobilites.fr/explore/dataset/referentiel-des-lignes/table/?disjunctive.transportsubmode&disjunctive.operatorname&disjunctive.networkname&disjunctive.transportmode&refine.transportmode=metro&sort=name_line&refine.status=active
    * Le répertoire "data" inclus une extraction des lignes du métro parisien dans le fichier "referentiel-des-lignes-paris.csv".
    * On trouve dans ce fichier, les noms et couleurs officiels des lignes. On peut donc s'en servir pour générer le fichier "lignes.csv" du sujet.
        * Pour extraire le nom de la ligne et sa couleur :
            * On récupère toutes les lignes en sautant la première (noms des champs).
            * On extrait les champs 2 et 11
            * On trie sur la colonne nom (format humain "sort -h")
            * Et enfin, on extrait le champ couleur en lui ajoutant un # devant afin d'être dans le bon format.
                tail -n+2 referentiel-des-lignes-paris.csv | cut -d ";" -f 2,11 | sort -h | sed "s/\(.*\);\(.*\)/\#\2/g"
    * On peut aussi trouver des informations officielles sur le site https://www.itineraire-metro.fr, cela va nous aider à remplir les données, notamment l'attente moyenne entre deux rames, mais à la main.
        * Exemple ligne 1 : https://www.itineraire-metro.fr/ligne-1.html
    * On peut encore trouver des informations sur les lignes sur Wikipédia, avec le temps de parcours, la distance, et le nombre de stations. On peut donc en déterminer les vitesses moyennes.
        * Exemple ligne 2 : https://fr.wikipedia.org/wiki/Ligne_2_du_m%C3%A9tro_de_Paris
    * L'article : https://www.liberation.fr/checknews/2018/02/14/a-quelle-vitesse-roule-le-metro-parisien_1653099/ valide les vitesses moyennes calculées.
    * Globalement, on trouve que le temps moyen entre rame est :
        * En journée, l'intervalle de temps entre chaque rame est de deux à quatre minutes en semaine
        * et de trois à cinq minutes le dimanche.
        * Après 20 heures, compter cinq à huit minutes d'attente.
        * Nous partons du principe que le temps moyen sera de cinq minutes.
    * Grâce à ces informations, nous pouvons remplir un fichier Excel que l'on trouve dans "data/informations_lignes_paris_collectées_sur_internet.xlsx"
    * Enfin, on peut extraire ces informations dans un fichier CSV "data/informations_lignes_paris_collectées_sur_internet.csv"
    * Ce fichier va nous servir de base afin de créer les données manquantes :
        * On extrait les champs 1 (nom), 5 et suivants pour créer le fichier des lignes de paris :
            tail -n+2 data/informations_lignes_paris_collectées_sur_internet.csv | cut -d ";" -f1,5- > lignes_paris.csv

EXERCICES A COMPLETER
----------------------
Le projet contient une base de travail avec les structures et quelques fonctions
implémentées. Voici l'état d'avancement :

1. Exercice 1 : Fonctions de gestion des listes de stations ...................  FAIT
   - creer_truc / detruire_truc
   - inserer_liste_trie / ecrire_liste / detruire_liste / detruire_liste_et_truc
   - lire_stations (parsing CSV avec gestion des espaces dans les noms)
   - limites_zone (calcul du rectangle englobant)

2. Exercice 2 : Arbres binaires de recherche pour les stations ...............   FAIT
   - creer_nabr
   - inserer_abr (avec comparaison case-insensitive)
   - construire_abr
   - detruire_abr
   - chercher_station (recherche insensible à la casse)

3. Exercice 3 : Listes de lignes de métro .....................................  FAIT
   - lire_lignes
   - afficher_lignes
   - detruire_lignes
   - chercher_ligne

4. Exercice 4 : Gestion des connexions ........................................  FAIT
   - inserer_deb_liste (implémentée dans le fichier liste.c)
   - lire_connexions (calcul automatique des temps via formule de Haversine)
   - ecrire_connexions
   - Script Python pour générer connexions_paris.csv (voir README_GENERER_CONNEXIONS.txt)

5. Exercice 5 : Arbres quaternaires pour la topologie .........................  FAIT
   - inserer_aqr (insertion récursive dans les 4 quadrants)
   - construire_aqr (avec calcul automatique du seuil et détection de collisions)
   - detruire_aqr (destruction récursive)
   - chercher_aqr (recherche optimisée du truc le plus proche)
   - chercher_zone (recherche de tous les trucs dans un rectangle)

6. Exercice 6 : Algorithme de Dijkstra pour le plus court chemin ..............  FAIT
   - extraire_deb_liste (implémentée et testée)
   - extraire_liste (implémentée et testée)
   - dijkstra (avec gestion du temps de parcours, temps d'arrêt et pénalité de correspondance)
   - cherche_chemin (reconstitution du chemin optimal)
   - configurer_dijkstra (configuration des temps de correspondance et d'arrêt)
   - Pensez à appeler la fonction configurer_dijkstra(config->temps_correspondance, config->temps_arret)
     après la lecture du fichier .ini et avant de lancer dijkstra() dans l'application.

7. Exercice 7 : Interface graphique ........................................... FAIT
   FAIT :
   - Architecture de base (structure Etat_affichage, enum Ecran/Mode)
   - Organisation modulaire du code GUI dans le répertoire gui/
     * gui/gui.c : Boucle principale et dispatch des événements
     * gui/gui_commun.c : Fonctions communes (polices, texte, aide)
     * gui/gui_menu.c : Écran menu (dessin, gestion clavier/souris)
     * gui/gui_plan.c : Écran plan (dessin, gestion clavier/souris)
   - Menu principal avec 3 options (affichage plan, calcul chemin, quitter)
     * Support clavier (touches 1, 2, 3, Q)
     * Support souris (clic sur les cartes)
   - Écran plan avec grille de repérage, panneau d'information et encart de chemin
   - Chargement des données du métro (stations, ABR, AQR, lignes, connexions)
   - Gestion du zoom (+/-, molette) et déplacement (flèches, clic droit + glisser)
   - Réinitialisation de la vue (touche R)
   - Détection des clics souris (curseur rouge)
   - Affichage des stations et des connexions (couleur de ligne avec validation)
   - Sélection de station au clic (mise en évidence + console)
   - Optimisation de la sélection avec AQR (recherche O(log n) avec fallback linéaire)
   - Mode chemin (départ/arrivée), Dijkstra et affichage du chemin
   - Infobulles station/ligne au survol (détection de proximité point-segment)
   - Encart récapitulatif du chemin (regroupement par ligne + temps total)
   - Extraction des constantes (couleurs, dimensions) dans gui/gui.h
   - Validation des couleurs hexadécimales au chargement des lignes
   - Documentation complète des algorithmes dans ALGORITHMES.txt
   - Recherche textuelle de station (touche F, recherche incrémentale avec ABR, centrage automatique)

   AMÉLIORATIONS POSSIBLES :
   - Affichage permanent des noms de stations importantes
   - Légende des lignes avec correspondance couleur ↔ nom
   - Centrage automatique de la vue sur le réseau au démarrage
   - Cache des limites géographiques pour optimiser geo_vers_ecran()

CHOIX DE LA BIBLIOTHÈQUE GRAPHIQUE
-----------------------------------
Pour l'exercice 7 (interface graphique), nous avons comparé trois bibliothèques majeures :

1. GTK+ (GIMP Toolkit)
   - Toolkit complet pour interfaces utilisateur (boutons, menus, fenêtres)
   - Orienté vers des applications de bureau traditionnelles
   - Nombreux widgets prédéfinis
   - Courbe d'apprentissage importante
   - Excellente intégration avec GNOME
   - Liens :
     * https://www.gtk.org/
     * https://docs.gtk.org/gtk4/

2. Qt
   - Framework complet et multiplateforme (C++)
   - Très riche en fonctionnalités (widgets, réseau, base de données, etc.)
   - Designer graphique pour créer des interfaces (Qt Designer)
   - Excellent pour applications professionnelles complexes
   - Licence complexe (GPL/LGPL ou commerciale)
   - Nécessite une installation volumineuse
   - Liens :
     * https://www.qt.io/
     * https://doc.qt.io/

3. SDL2 (Simple DirectMedia Layer 2)
   - Bibliothèque multimédia bas niveau
   - Contrôle direct du rendu graphique (pixel par pixel)
   - Légère et rapide
   - Pas de widgets prédéfinis (on dessine tout manuellement)
   - Excellente pour jeux vidéo et visualisations personnalisées
   - Licence permissive (zlib)
   - Liens :
     * https://www.libsdl.org/
     * https://wiki.libsdl.org/SDL2/FrontPage

CHOIX RETENU : SDL2

Raisons du choix :
- Notre projet nécessite un rendu graphique personnalisé (plan de métro, lignes, stations)
- Pas besoin de widgets standards (boutons, menus déroulants, etc.)
- Besoin de contrôle total sur l'affichage (zoom, déplacement, sélection graphique)
- Simplicité d'installation et de compilation
- Bibliothèque légère et performante pour le rendu 2D
- Licence permissive adaptée à un projet pédagogique
- Bonne documentation et nombreux tutoriels disponibles

GTK+ et Qt seraient surdimensionnés pour nos besoins : nous n'avons pas besoin de
widgets complexes, juste de dessiner des formes géométriques (stations, lignes) et
gérer des événements souris/clavier. SDL2 offre exactement ce qu'il faut sans la
complexité additionnelle d'un toolkit complet.

INFORMATIONS SDL2
-----------------
- Tutos et documentation :
    - https://www.meruvia.fr/index.php/fr/big-tuto-sdl-2
    - https://devopssec.fr/article/cours-introduction-sdl-2
    - https://wiki.libsdl.org/SDL2/Tutorials
    - https://lazyfoo.net/tutorials/SDL/index.php
    - https://wiki.libsdl.org/SDL2/FrontPage
- Afin d'accélérer l'apprentissage et la mise en application du code, nous nous sommes aidé de Claude AI et ChatGPT.

NOTES IMPORTANTES
-----------------
- Les coordonnées sont en degrés décimaux.
- Les vitesses sont en km/h.
- Les intervalles sont en minutes.
- Les couleurs sont au format hexadécimal #RRGGBB.
- Le champ user_val des trucs est utilisé pour :
  * Le tri par latitude dans les listes.
  * Le calcul des distances dans l'algorithme de Dijkstra.
- La recherche dans l'ABR est insensible à la casse (case insensitive).
- Les fonctions d'extraction de liste modifient la liste d'origine.
- L'algorithme de Dijkstra utilise les paramètres de configuration :
  * temps_correspondance : Pénalité (en minutes) ajoutée lors d'un changement de ligne
  * temps_arret : Temps (en minutes) ajouté à chaque station visitée
  Ces paramètres peuvent être configurés via le fichier config.ini ou config_paris.ini.

WORKFLOW DE DÉVELOPPEMENT RECOMMANDE
-------------------------------------
1. Développer une nouvelle fonctionnalité
2. Ajouter des tests dans le fichier tests.c pour valider la fonctionnalité
3. Compiler avec : make tests
4. Exécuter les tests : ./tests
5. Vérifier les fuites mémoire : "valgrind --leak-check=full ./tests"
6. Intégrer la fonctionnalité dans le fichier main.c si nécessaire
7. Mettre à jour ce README.txt
