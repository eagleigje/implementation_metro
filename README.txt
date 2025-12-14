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
   - main.c                  : Application principale (interface graphique à implémenter)
   - tests.c/tests.h         : Programme de tests et validation

5. Configuration :
   - config.h/config.c       : Système de configuration .ini
   - config.ini              : Fichier de configuration principal
   - config_alt.ini          : Exemple de configuration alternative

6. Fichiers de données :
   - stations.csv : Fichier contenant les stations (longitude;latitude;nom), allégé pour les tests.
   - stations_paris.csv : Fichier contenant toutes les stations parisiennes (cf. LOCALISATION DES DONNEES).
   - lignes.csv : Fichier contenant les lignes (code;vitesse;intervalle;couleur).
   - lignes_paris.csv : Fichier contenant toutes les lignes parisiennes.
   - connexions.csv : Fichier contenant les connexions (test).
   - connexions_paris.csv : Fichier contenant toutes les connexions parisiennes (généré automatiquement).

7. Programmes optionnels à installer :
    - doxygen : Afin de générer la documentation technique du projet.
    - valgrind : Pour vérifier que le projet libère bien toute la mémoire à la sortie.
    - glibc-debug : Pour valgrind.

8. Documentation :
    - doc/projet-metro2025.pdf         : Le PDF du projet
    - doc/projet/html/index.html       : La documentation Doxygen du projet
    - README_GENERER_CONNEXIONS.txt    : Guide d'utilisation du script de génération des connexions

9. Scripts utilitaires :
    - generer_connexions_paris.py      : Script Python pour générer connexions_paris.csv automatiquement

COMPILATION
-----------
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

Pour vérifier les fuites mémoire avec valgrind :
    make test-memory

Pour nettoyer les fichiers objets et les exécutables :
    make clean

Pour tout nettoyer et tout recompiler :
    make all

Pour générer la documentation (doxygen doit être installé sur le PC) :
    make docs

Pour nettoyer la documentation :
    make clean-docs

EXECUTION
---------
Le projet produit deux exécutables :

1. Programme de TESTS (pour validation et développement) :
    ./tests

   Ce programme permet de :
   - Charger et valider les stations depuis le fichier CSV
   - Tester l'arbre binaire de recherche (recherche case-insensitive)
   - Tester les fonctions d'extraction de listes
   - Afficher des statistiques détaillées
   - Calculer les limites de la zone géographique

   Pour utiliser un fichier de configuration différent :
    ./tests config_alt.ini
    ./tests /chemin/vers/ma_config.ini

2. Application PRINCIPALE (interface graphique à implémenter) :
    ./metro

   Cette application est en cours de développement.
   Elle sera l'interface graphique finale pour l'utilisateur.

   Pour utiliser un fichier de configuration différent :
    ./metro config_alt.ini
    ./metro /chemin/vers/ma_config.ini

TESTS ET VALIDATION
-------------------
Le programme de tests (./tests) valide les fonctionnalités, au fur et à mesure.
Pour ajouter de nouveaux tests :

1. Créer une nouvelle fonction de test dans `tests.c`
2. Ajouter le prototype dans `tests.h`
3. Appeler la fonction dans le `main()` de `tests.c`
4. Suivre le même style de rapport (compteur tests_ok/tests_total)

Dans le répertoire test_data, on trouve des fichiers mal formés pour les tests.
    * test_data/config_invalide.ini
    * test_data/stations_malformees.csv
    * test_data/stations_vide.csv

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
        (head -1 metro-france.csv && grep \"Paris metro-france.csv) > metro-paris.csv

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
        * Nous partons du principe que le temps moyens sera de 5 minutes.
    * Grâce à ces informations, nous pouvons remplir un fichier Excel que l'on trouve dans "data/informations_lignes_paris_collectées_sur_internet.xlsx"
    * Enfin, on peut extraire ces informations dans un fichier CSV "data/informations_lignes_paris_collectées_sur_internet.csv"
    * Ce fichier va nous servir de base afin de créer les données manquantes :
        * On extrait les champs 1 (nom), 5 et suivants afin de créer le fichier des lignes de paris :
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
   - inserer_deb_liste (implémentée dans liste.c)
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
   - Pensez à appeler configurer_dijkstra(config->temps_correspondance, config->temps_arret)
     après la lecture du fichier .ini et avant de lancer dijkstra() dans l'application.

7. Exercice 7 : Interface graphique ........................................... A FAIRE
   - Design de l'interface
   - Affichage du plan
   - Zoom et déplacement
   - Sélection de stations
   - Calcul et affichage du chemin

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
  Ces paramètres peuvent être configurés via le fichier config.ini ou config_alt.ini.

WORKFLOW DE DEVELOPPEMENT RECOMMANDE
-------------------------------------
1. Développer une nouvelle fonctionnalité
2. Ajouter des tests dans le fichier tests.c pour valider la fonctionnalité
3. Compiler avec : make tests
4. Exécuter les tests : ./tests
5. Vérifier les fuites mémoire : "valgrind --leak-check=full ./tests"
6. Intégrer la fonctionnalité dans le fichier main.c si nécessaire
7. Mettre à jour ce README.txt
