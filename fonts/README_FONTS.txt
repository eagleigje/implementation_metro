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
2. Copiez le fichier ici et renommez-le en "DejaVuSans.ttf".
3. Ou bien, ajoutez cette nouvelle police dans la liste de la fonction gui_charger_police dans gui/gui_commun.c

REMARQUE
--------
- Ce mécanisme évite un écran sans texte si les polices système sont absentes.
- Nous fournissons par défaut la police DejaVuSans.ttf car elle est libre et bien adaptée au projet :
    https://en.wikipedia.org/wiki/DejaVu_fonts
    https://dejavu-fonts.github.io/
