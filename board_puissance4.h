#include "list.h"

/*Les macros définies facilitent la navigation sur un plateau de jeu en grille. HORIZON permet un déplacement horizontal d'une case vers
la droite. DIAGLEFT et DIAGRIGHT gèrent les déplacements diagonaux vers la gauche et la droite, respectivement. VERTICAL représente 
un mouvement vertical d'une rangée vers le haut. Ces définitions simplifient le calcul des positions pour les vérifications et placements
de pièces selon différentes orientations sur le plateau.*/
#define HORIZON 1
#define DIAGLEFT (COLUMNS - 1)
#define DIAGRIGHT (COLUMNS + 1)
#define VERTICAL (-COLUMNS)

Item *initGame();
void printBoard(Item *node);
int gameOver(Item *node);
int evaluateBoard(Item *node, int joueur);
void createChild(Item *child, Item *parent);
int isFreeRow(Item *node, int column);
Item *getChildNode(Item *node, int column, int player);
int calculValeurPosition(Item *node, int position, int joueur);
int Section4(Item *node, int position, int joueur, int direction);
int Section3(Item *node, int position, int joueur, int direction);
int Section2(Item *node, int position, int joueur, int direction);
int Section1(Item *node, int position, int joueur, int direction);
