========================================
    LOCALISATION DES DONNÉES
        PROJET METRO 2025
========================================

Ce document explique comment les fichiers de données du projet (stations_paris.csv,
lignes_paris.csv, connexions_paris.csv) ont été générés à partir des sources officielles
disponibles en ligne.

========================================
STATIONS PARISIENNES
========================================

SOURCE OFFICIELLE
-----------------
https://explore.data.gouv.fr/fr/datasets/6405b01aa7f6f0f5523c8653/?Commune%20nom__contains=Paris#/resources/5b645234-e6fd-4de8-889c-574a80531aca

FICHIERS INTERMÉDIAIRES
-----------------------
Le répertoire "data" contient :
- metro-france.csv : Extraction de tous les métros français
- metro-paris.csv : Extraction des métros parisiens uniquement

ÉTAPE 1 : EXTRAIRE LES STATIONS PARISIENNES
--------------------------------------------
L'extraction "parisienne" a été réalisée en bash.

Pour cela, on récupère les IDs des lignes (première colonne) parisiennes, puis on
cherche à nouveau les lignes en entier grâce à ces IDs (en effet, les lignes ne
commencent ou ne finissent pas toutes à Paris). On conserve l'en-tête du fichier d'origine.

Commande :
    (head -1 data/metro-france.csv && \
    grep -E -f <(grep '"Paris' data/metro-france.csv | cut -d',' -f1 | sort -u | sed 's/^/^/; s/$/,/') \
    data/metro-france.csv) > data/metro-paris.csv

ÉTAPE 2 : CONVERTIR AU FORMAT DU PROJET
----------------------------------------
On extrait les données des métros parisiens dans le format demandé dans le sujet :
(séparateur "," puis tous les champs à partir du troisième, on dédoublonne, on supprime
les " et enfin, on récupère les champs 2, 3 et 1)

Commande :
    cut -d "," -f3- < data/metro-paris.csv | sort | uniq | sed "s/\"//g" | awk -F "," '{print($2";"$3";"$1)}' > stations_paris.csv

Résultat : stations_paris.csv au format "longitude;latitude;nom_station"


========================================
LIGNES PARISIENNES
========================================

SOURCE OFFICIELLE
-----------------
https://data.iledefrance-mobilites.fr/explore/dataset/referentiel-des-lignes/table/?disjunctive.transportsubmode&disjunctive.operatorname&disjunctive.networkname&disjunctive.transportmode&refine.transportmode=metro&sort=name_line&refine.status=active

FICHIER INTERMÉDIAIRE
---------------------
Le répertoire "data" contient :
- referentiel-des-lignes-paris.csv : Extraction des lignes du métro parisien

Ce fichier contient les noms et couleurs officiels des lignes.

ÉTAPE 1 : EXTRAIRE NOM ET COULEUR
----------------------------------
Pour extraire le nom de la ligne et sa couleur :
- On récupère toutes les lignes en sautant la première (noms des champs)
- On extrait les champs 2 et 11
- On trie sur la colonne nom (format humain "sort -h")
- Et enfin, on extrait le champ couleur en lui ajoutant un # devant

Commande :
    tail -n+2 referentiel-des-lignes-paris.csv | cut -d ";" -f 2,11 | sort -h | sed "s/\(.*\);\(.*\)/\#\2/g"

ÉTAPE 2 : COMPLÉTER AVEC VITESSE ET INTERVALLE
-----------------------------------------------
On trouve des informations complémentaires sur :

1. Site itineraire-metro.fr (attente moyenne entre deux rames)
   Exemple ligne 1 : https://www.itineraire-metro.fr/ligne-1.html

2. Wikipédia (temps de parcours, distance, nombre de stations → vitesse moyenne)
   Exemple ligne 2 : https://fr.wikipedia.org/wiki/Ligne_2_du_m%C3%A9tro_de_Paris

3. Validation des vitesses moyennes :
   https://www.liberation.fr/checknews/2018/02/14/a-quelle-vitesse-roule-le-metro-parisien_1653099/

DONNÉES COLLECTÉES
------------------
Temps moyen entre rames :
- En journée : 2 à 4 minutes en semaine
- Dimanche : 3 à 5 minutes
- Après 20h : 5 à 8 minutes
- Temps moyen retenu : 5 minutes

ÉTAPE 3 : FICHIER EXCEL ET CSV FINAL
-------------------------------------
Grâce à ces informations, nous avons rempli un fichier Excel :
    data/informations_lignes_paris_collectées_sur_internet.xlsx

Puis exporté en CSV :
    data/informations_lignes_paris_collectées_sur_internet.csv

Extraction finale (champs 1 (nom), 5 et suivants) :
    tail -n+2 data/informations_lignes_paris_collectées_sur_internet.csv | cut -d ";" -f1,5- > lignes_paris.csv

Résultat : lignes_paris.csv au format "code;vitesse;intervalle;couleur"


========================================
CONNEXIONS PARISIENNES
========================================

Les connexions entre stations sont générées automatiquement à partir de
data/metro-paris.csv en utilisant le script Python :

    python3 generer_connexions_paris.py

Pour plus de détails sur la génération des connexions, voir :
    README_GENERER_CONNEXIONS.txt


========================================
FICHIERS FINAUX DU PROJET
========================================

stations_paris.csv   → Format : longitude;latitude;nom_station
lignes_paris.csv     → Format : code;vitesse;intervalle;couleur
connexions_paris.csv → Format : code_ligne;station_depart;station_arrivee;temps_parcours

Ces fichiers sont référencés dans config_paris.ini et utilisés par :
- Le programme de tests : ./tests config_paris.ini
- L'application principale : ./metro config_paris.ini
