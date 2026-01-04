========================================
           POLICES LOCALES
========================================

OBJECTIF
--------
Ce dossier permet de fournir une police locale si aucune police système
n'est trouvée. L'application tente plusieurs chemins système puis
utilise le fichier suivant en dernier recours :

    fonts/DejaVuSans.ttf

INSTRUCTIONS
------------
1. Téléchargez une police TrueType (TTF), par exemple DejaVu Sans.
2. Renommez le fichier téléchargé en "DejaVuSans.ttf", et copiez-le dans fonts/.
3. Ou mieux, ajoutez cette nouvelle police dans la liste de la fonction gui_charger_police dans gui/gui_commun.c.

REMARQUE
--------
- Ce mécanisme évite un écran sans texte si les polices système sont absentes.
- Le dépôt inclut fonts/DejaVuSans.ttf (licence libre), utilisée par défaut si aucune police système n'est trouvée :
    https://en.wikipedia.org/wiki/DejaVu_fonts
    https://dejavu-fonts.github.io/
