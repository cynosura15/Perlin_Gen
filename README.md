# Terrain Perlin
## Description
Génère un fichier 3D OBJ et une image PGM avec l'algo du bruit de perlin.
Peut également générer des fichiers OBJ selon une map greyscale existante.

## Compilation
Pas de bibliothèques externes nécessaires. Visual Studio conseillé pour le debugger.

## Utilisation
Paramètres ajustables depuis la console (taille du terrain, fréquence, amplitude, octaves (fbm)).
Exemple pour avoir un beau terrain (ou une belle map greyscale) :
\* taille : 300
\* fréquence : 0.01
\* amplitude max : 110 (255 si on fait une map greyscale)
\* octaves (fbm) : 6

## À faire
1. Passer sous Qt pour avoir une interface graphique potable (actuellement, on utilise la console)

## Auteurs
Robin LEMAÎTRE et Rayan KHEMMAR - groupe EG4