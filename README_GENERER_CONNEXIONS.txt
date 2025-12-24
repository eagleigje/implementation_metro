========================================
GÉNÉRATEUR DE FICHIER CONNEXIONS
========================================

Ce script Python permet de générer automatiquement le fichier connexions_paris.csv
à partir des données réelles du métro parisien.

UTILISATION
-----------

Commande de base :
    python3 generer_connexions_paris.py

Avec paramètres personnalisés :
    python3 generer_connexions_paris.py [fichier_metro_source] [fichier_sortie]

Exemples :
    python3 generer_connexions_paris.py
    python3 generer_connexions_paris.py data/metro-paris.csv connexions_paris.csv

FICHIERS
--------

Fichier source (par défaut) :
    data/metro-paris.csv

    Format : "ID Line","Libelle Line","Libelle station","Longitude","Latitude",...
    Contient toutes les stations du métro parisien organisées par ligne.

Fichier généré (par défaut) :
    connexions_paris.csv

    Format : Code_Ligne ; Station_Départ ; Station_Arrivée ; Temps_Parcours
    Contient toutes les connexions bidirectionnelles entre stations consécutives.

RÉSULTATS
---------

Pour le métro parisien complet (16 lignes) :
    → 632 connexions bidirectionnelles
    → 16 lignes de métro (1, 2, 3, 3bis, 4, 5, 6, 7, 7bis, 8, 9, 10, 11, 12, 13, 14)
    → Temps de parcours = 0.0 (calculé automatiquement par le programme)

Le fichier généré peut être utilisé directement avec :
    - config.ini : fichier_connexions = connexions_paris.csv
    - Le programme de tests : ./tests
    - L'application principale : ./metro

NOTES TECHNIQUES
----------------

1. Le script lit data/metro-paris.csv et extrait :
   - Le code de la ligne (1, 2, 3, 3bis, etc.)
   - Le nom de chaque station
   - L'ordre des stations sur chaque ligne

2. Pour chaque paire de stations consécutives, il crée deux connexions :
   - Station A → Station B (sens 1)
   - Station B → Station A (sens 2).

3. Les temps de parcours sont mis à 0.0 pour être calculés automatiquement
   par la fonction lire_connexions() basée sur :
   - La distance géographique (formule de Haversine)
   - La vitesse de la ligne (depuis lignes_paris.csv).

4. Les noms de stations sont conservés tels quels depuis le fichier source
   - Aucune modification des noms officiels
   - Les parenthèses font partie intégrante du nom (ex : "Saint-Paul (Le Marais)")

DÉPENDANCES
-----------

Python 3 (version 3.6 minimum)
Module csv (inclus dans la bibliothèque standard)

AUTEUR
------

Généré pour le Projet Metro 2025
