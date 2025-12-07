#!/usr/bin/env python3
"""
Script pour générer le fichier connexions_paris.csv
Basé sur les données réelles du métro parisien depuis data/metro-paris.csv
"""

import csv
import sys


def lire_metro_paris(fichier_metro="data/metro-paris.csv"):
    """
    Lit le fichier metro-paris.csv et organise les stations par ligne dans l'ordre.

    Returns:
        dict: {code_ligne: [(nom_station, lon, lat), ...]}
    """
    lignes = {}

    try:
        with open(fichier_metro, 'r', encoding='utf-8') as f:
            reader = csv.DictReader(f)

            for row in reader:
                code_ligne = row['Libelle Line'].strip()
                nom_station = row['Libelle station'].strip()

                # Normaliser les noms de lignes (3bis -> 3B, 7bis -> 7B)
                # pour être cohérent avec lignes_paris.csv
                if code_ligne.lower() == '3bis':
                    code_ligne = '3B'
                elif code_ligne.lower() == '7bis':
                    code_ligne = '7B'

                # Initialiser la ligne si nécessaire
                if code_ligne not in lignes:
                    lignes[code_ligne] = []

                # Ajouter la station (on garde l'ordre du fichier CSV)
                lignes[code_ligne].append(nom_station)

        return lignes

    except FileNotFoundError:
        print(f"ERREUR: Fichier {fichier_metro} non trouvé!", file=sys.stderr)
        return {}
    except Exception as e:
        print(f"ERREUR lors de la lecture: {e}", file=sys.stderr)
        return {}


def generer_connexions(fichier_metro="data/metro-paris.csv",
                       fichier_sortie="connexions_paris.csv"):
    """
    Génère le fichier CSV des connexions pour le métro parisien.
    """
    print(f"Lecture des données depuis {fichier_metro}...")
    lignes = lire_metro_paris(fichier_metro)

    if not lignes:
        print("ERREUR: Aucune donnée chargée!")
        return 0

    print(f"  → {len(lignes)} lignes trouvées")

    print(f"\nGénération du fichier {fichier_sortie}...")

    connexions_total = 0

    with open(fichier_sortie, 'w', encoding='utf-8') as f:
        # Entête
        f.write("# Fichier de connexions du métro parisien\n")
        f.write("# Généré automatiquement depuis data/metro-paris.csv\n")
        f.write("# Format : Code_Ligne ; Station_Départ ; Station_Arrivée ; Temps_Parcours\n")
        f.write("# Si Temps_Parcours = 0.0, il sera calculé automatiquement\n")
        f.write("\n")

        # Trier les lignes par code (1, 2, 3, ...)
        def tri_ligne(code):
            """Fonction de tri personnalisée pour les codes de ligne."""
            # Séparer le numéro de la lettre éventuelle (ex: "3B" -> 3, "B")
            if code[-1].isalpha() and len(code) > 1:
                return (int(code[:-1]), code[-1])
            else:
                return (int(code), "")

        lignes_triees = sorted(lignes.keys(), key=tri_ligne)

        # Pour chaque ligne de métro
        for code_ligne in lignes_triees:
            stations = lignes[code_ligne]

            if len(stations) < 2:
                print(f"  AVERTISSEMENT: Ligne {code_ligne} a moins de 2 stations, ignorée.")
                continue

            f.write(f"# Ligne {code_ligne} ({len(stations)} stations)\n")

            # Créer les connexions entre stations consécutives (bidirectionnelles)
            for i in range(len(stations) - 1):
                station_dep = stations[i]
                station_arr = stations[i + 1]

                # Connexion aller
                f.write(f"{code_ligne} ; {station_dep} ; {station_arr} ; 0.0\n")
                connexions_total += 1

                # Connexion retour
                f.write(f"{code_ligne} ; {station_arr} ; {station_dep} ; 0.0\n")
                connexions_total += 1

            f.write("\n")

    print(f"\n✓ Fichier généré avec succès!")
    print(f"  → {connexions_total} connexions créées")
    print(f"  → {len(lignes)} lignes de métro")

    # Statistiques par ligne
    print(f"\nDétails par ligne:")
    for code_ligne in lignes_triees:
        nb_stations = len(lignes[code_ligne])
        nb_connexions = (nb_stations - 1) * 2
        print(f"  Ligne {code_ligne:3s}: {nb_stations:3d} stations → {nb_connexions:3d} connexions")

    return connexions_total


if __name__ == "__main__":
    fichier_metro = "data/metro-paris.csv"
    fichier_sortie = "connexions_paris.csv"

    # Arguments en ligne de commande
    if len(sys.argv) > 1:
        fichier_metro = sys.argv[1]
    if len(sys.argv) > 2:
        fichier_sortie = sys.argv[2]

    nb_connexions = generer_connexions(fichier_metro, fichier_sortie)

    if nb_connexions > 0:
        print(f"\n✓ Succès! {fichier_sortie} contient {nb_connexions} connexions.")
        print(f"\nUtilisation:")
        print(f"  python3 {sys.argv[0]} [fichier_metro] [fichier_sortie]")
        print(f"  Par défaut: data/metro-paris.csv → connexions_paris.csv")
    else:
        print(f"\n✗ Échec de la génération!")
        sys.exit(1)
