#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "board_puissance4.h"


/* ça retourne "0", si ça trouve dans les 3 positions devant un jeton de l'adversaire ( IA dans notre cas ) , '1' sinon  */
int Section1(Item *node, int position, int joueur, int direction)
{
    int adversaire = joueur * -1;

    for (int i = 1; i < 4; i++) 
    {
        if (node->board[position + i * direction] == adversaire) // regarde ces positions, si ce sont occopées par l'adversaire
        {
            return 0;
        }
    }
    return 1;
}

/* La fonction retourne "0" si elle trouve un jeton de l'adversaire (IA dans ce cas) dans les deux positions devant ou dans la position juste derrière, sinon elle retourne "1".  */
int Section2(Item *node, int position, int joueur, int direction)
{
    int adversaire = joueur * -1;

    for (int i = -1; i < 3; i++) 
    {   
        if (i != 0) 
        {
            if (node->board[position + i * direction] == adversaire) // regarde ces positions, si ce sont occopées par l'adversaire
            {
                return 0;
            }
        }   
    }
    return 1;
}

/* La fonction retourne "0" si elle trouve un jeton de l'adversaire (IA dans ce cas) dans une position juste devant ou dans les deux position derrière, sinon elle retourne "1". */
int Section3(Item *node, int position, int joueur, int direction)
{
    int adversaire = joueur * -1;

    for (int i = -2; i < 2; i++) 
    {
        if (i != 0) 
        {
            if (node->board[position + i * direction] == adversaire) // regarde ces positions, si ce sont occopées par l'adversaire
            {
                return 0;
            }
        }
    }
    return 1;
}

/* La fonction retourne "0" si elle trouve un jeton de l'adversaire (IA dans ce cas) dans les trois positions précédentes, sinon elle retourne "1".  */
int Section4(Item *node, int position, int joueur, int direction)
{
    int adversaire = joueur * -1;
    for (int i = -3; i < 0; i++) 
    {
        if (node->board[position + i * direction] == adversaire) // regarde ces positions, si ce sont occopées par l'adversaire
        {
            return 0;
        }
    }
    return 1;
}


/* Cette fonction calcule la valeur heuristique d'une position.
Pour ce faire, elle examine les différents emplacements sur une section de
4 positions. Elle retourne ensuite la valeur heuristique. */
int calculValeurPosition(Item *node, int position, int joueur)
{
    int val_heuristique = 0;
    int ligne = position / COLUMNS; //  ligne de la position
    int colonne = position % COLUMNS; // colonne de la position

    /* on regarde dans chaque direction, des sections de 4 positions si ce sont prises par l'adversaire */

    // on commence par l'horizontale
    switch(colonne)
    {
    case 0:
        /* Dans la colonne 0, seules les 3 positions devant sont évaluées, donc Section1(). 
        Par exemple : | - |  |  |  |  dans cette section, ça regarde les trois positions devant, 
        si l'adversaire n'a pas de pion dans ces positions ça incrémente la valeur heuristique */
        val_heuristique += Section1(node, position, joueur, HORIZON);
        break;
    case 1:
        // Dans la colonne 1, il faut évaluer 3 positions devant et 1 position derrière, donc on appelle Section1() et Section2().
        val_heuristique += Section1(node, position, joueur, HORIZON); 
        val_heuristique += Section2(node, position, joueur, HORIZON);
        break;
    case 2:
        // Dans la colonne 2, il faut évaluer 3 positions devant et 2 positions dérriére, donc on appelle Section1(), Section2() et Section3().
        val_heuristique += Section1(node, position, joueur, HORIZON);
        val_heuristique += Section2(node, position, joueur, HORIZON);
        val_heuristique += Section3(node, position, joueur, HORIZON);
        break;
    case 3:
        // Dans la colonne 3, il faut évaluer 3 positions devant et 3 positions dérriére, donc on appelle Section1(), Section2(), Section3() et Section4().
        val_heuristique += Section1(node, position, joueur, HORIZON);
        val_heuristique += Section2(node, position, joueur, HORIZON);
        val_heuristique += Section3(node, position, joueur, HORIZON);
        val_heuristique += Section4(node, position, joueur, HORIZON);
        break;
    case 4:
        // Dans la colonne 4, il faut évaluer 2 positions devant et 3 positions dérriére, donc on appelle Section2(), Section3() et Section4().
        val_heuristique += Section2(node, position, joueur, HORIZON);
        val_heuristique += Section3(node, position, joueur, HORIZON);
        val_heuristique += Section4(node, position, joueur, HORIZON);
        break;
    case 5:
        // Dans la colonne 5, il faut évaluer 1 positions devant et 3 positions dérriére, donc on appelle Section3() et Section4().
        val_heuristique += Section3(node, position, joueur, HORIZON);
        val_heuristique += Section4(node, position, joueur, HORIZON);
        break;
    case 6:
        // Dans la colonne 2, il faut évaluer 3 positions dérriére, donc on appelle Section4().
        val_heuristique += Section4(node, position, joueur, HORIZON);
        break;
    }
    /* Verticale */
    switch (ligne)
{
    case 1:
    case 2:
    case 3:
        val_heuristique += Section1(node, position, joueur, VERTICAL);
        val_heuristique += Section2(node, position, joueur, VERTICAL);
        val_heuristique += Section3(node, position, joueur, VERTICAL);
        break;
    case 4:
        val_heuristique += Section1(node, position, joueur, VERTICAL);
        val_heuristique += Section2(node, position, joueur, VERTICAL);
        break;
    case 5:
        val_heuristique += Section1(node, position, joueur, VERTICAL);
        break;
}


    /* Diagonale gauche */
    if (ligne > 2 && colonne > 2)
    {
        val_heuristique += Section1(node, position, joueur, DIAGLEFT);
    }

    if (ligne > 1 && ligne < 5 && colonne > 1 && colonne < 6)
    {
        val_heuristique += Section2(node, position, joueur, DIAGLEFT);
    }

    if (ligne > 0 && ligne < 4 && colonne > 0 && colonne < 5)
    {
        val_heuristique += Section3(node, position, joueur, DIAGLEFT);
    }

    /* Diagonale droite */
    if (ligne > 2 && colonne < 4)
    {
        val_heuristique += Section1(node, position, joueur, DIAGRIGHT);
    }

    if (ligne > 1 && ligne < 5 && colonne < 5 && colonne > 0)
    {
        val_heuristique += Section2(node, position, joueur, DIAGRIGHT);
    }

    if (ligne > 0 && ligne < 4 && colonne < 6 && colonne > 1)
    {
        val_heuristique += Section3(node, position, joueur, DIAGRIGHT);
    }

    return val_heuristique;
}

/* évaluer la position actuelle sur le plateau pour un joueur donné
 en analysant les mouvements effectués et en calculant la valeur heuristique 
 de chaque position. */
int evaluateBoard(Item *node, int joueur)
{
    int val_heuristique = 0;
    int position;

    if (gameOver(node))
    {
        return val_heuristique = -1 * joueur * 1000000;
    }
    switch (joueur)
    {
    case 1:
        for (int i = 0; i < node->movesjoueur1; i++)
        {
            position = node->joueur1[i];
            val_heuristique += calculValeurPosition(node, position, 1);
        }
        for (int i = 0; i < node->movesjoueur2; i++)
        {
            position = node->joueur2[i];
            val_heuristique -= calculValeurPosition(node, position, -1);
        }
        break;

    case -1:
        for (int i = 0; i < node->movesjoueur2; i++)
        {
            position = node->joueur2[i];
            val_heuristique += calculValeurPosition(node, position, -1);
        }
        for (int i = 0; i < node->movesjoueur1; i++)
        {
            position = node->joueur1[i];
            val_heuristique -= calculValeurPosition(node, position, 1);
        }
        break;
    }

    return val_heuristique;
}


Item *initGame()
{
    Item *node;

    node = nodeAlloc();
    // Calloc initializes with zeroes everywhere
    node->board = calloc(SIZE, sizeof(char));
    node->joueur1 = calloc(SIZE / 2, sizeof(int));
    node->movesjoueur1 = 0;
    node->joueur2 = calloc(SIZE / 2, sizeof(int));
    node->movesjoueur2 = 0;
    node->f = 0;

    return node;
}

/*Nous avons conçu une fonction destinée à déterminer si une condition de fin de jeu est remplie 
(soit lorsqu'un joueur a gagné, soit lorsque le plateau est plein). Nous avons implémenté une fonction nommée « gameOver », 
qui prend en paramètre le nœud du jeu et retourne vrai si le jeu est terminé, faux sinon.*/

void printBoard(Item *node)
{
    assert(node);
    printf("\n************************************\n");
    printf("\n  0   1   2   3   4   5   6 \n");
    for (int i = 0; i < COLUMNS; i++)
        if (i == 0)
            printf(" ___");
        else
            printf("____");
    printf("\n");
    for (int j = 0; j < SIZE; j++)
    {
        if (j % COLUMNS == 0)
            printf("|");
        if (node->board[j] == 1)
            printf(" O |");
        else if (node->board[j] == -1)
            printf(" X |");
        else
            printf("   |");
        if ((j + 1) % COLUMNS == 0)
        {
            printf("\n");
            for (int i = 0; i < COLUMNS; i++)
                if (i == 0)
                    printf(" ___");
                else
                    printf("____");
            printf("\n");
        }
    }
    printf("\n************************************\n");
}

// Vérifie s'il existe une condition pour terminer le jeu. Si oui, retourne 1, sinon retourne 0.
int gameOver(Item *node){
    if (node->movesjoueur1 + node->movesjoueur2 == SIZE)
        // Plateau est plein

        return 1;
    else if (node->movesjoueur1 + node->movesjoueur2 == 0)
        // Plateau est vide
        return 0;

    int DernMove;                                   // Dérnier mouvement dans le plateau
    int verticalIndex, horizontalIndex, diagDownIndex, diagUpIndex;       // Suivre le parcours du tableau
    int vertAlign = 0, horizAlign = 0, diagDownAlign = 0, diagUpAlign = 0; // Garde le compte du nombre de pions alignés consécutivement

    if (node->movesjoueur1 > node->movesjoueur2)
        // Player 1, humain, toujours commence
        DernMove = node->joueur1[node->movesjoueur1 - 1];
    else
        DernMove = node->joueur2[node->movesjoueur2 - 1];

    int joueur = node->board[DernMove]; // Le joueur qui a fait le dérnier mouvement
    int i = -3;                     //Trois positions avant, boucle jusqu'à trois positions après

    while (i <= 3 &&
           vertAlign < 4 &&
           horizAlign < 4 &&
           diagDownAlign < 4 &&
           diagUpAlign < 4)
    {
        if (i == 0)
        // Ceci est le dernier pion, il contribue toujours à une chaîne
        {
            vertAlign++;
            horizAlign++;
            diagDownAlign++;
            diagUpAlign++;
            i++;
            continue;
        }

        // Calcul d'index
        horizontalIndex = DernMove + i;
        verticalIndex = DernMove + i * COLUMNS;
        diagDownIndex = DernMove + i * (COLUMNS + 1);
        diagUpIndex = DernMove + i * (COLUMNS - 1);

        // vérification de la verticale
        if ((verticalIndex >= 0) && (verticalIndex < SIZE) &&
            ((verticalIndex / COLUMNS - DernMove / COLUMNS) == i) &&
            (node->board[verticalIndex] == joueur))
            vertAlign++;
        else
            vertAlign = 0;

        // Vérification de la horizontale
        if ((horizontalIndex >= 0) && (horizontalIndex < SIZE) &&
            ((horizontalIndex % COLUMNS - DernMove % COLUMNS) == i) &&
            (node->board[horizontalIndex] == joueur))
            horizAlign++;
        else
            horizAlign = 0;

        // Vérification de la diagonale descendante
        if ((diagDownIndex >= 0) && (diagDownIndex < SIZE) &&
            ((diagDownIndex % COLUMNS - DernMove % COLUMNS) == i) &&
            (node->board[diagDownIndex] == joueur))
            diagDownAlign++;
        else
            diagDownAlign = 0;

        // Vérification de la diagonale ascendante
        if ((diagUpIndex >= 0) && (diagUpIndex < SIZE) &&
            ((DernMove % COLUMNS - diagUpIndex % COLUMNS) == i) &&
            (node->board[diagUpIndex] == joueur))
            diagUpAlign++;
        else
            diagUpAlign = 0;

        i++;
    }

    if (vertAlign != 4 && horizAlign != 4 && diagDownAlign != 4 && diagUpAlign != 4)
        return 0;
    return 1;
}



/* la on crée un nouveau enfant à partir du noeud parent et on copie l'etat de la partie cette fonction va etre appeller dans 
la fonction getChildBoard()
*/
void createChild(Item *child, Item *parent)
{
    assert(parent);
    child->board = calloc(SIZE, sizeof(char));
    child->joueur1 = calloc(SIZE / 2, sizeof(int));
    child->joueur2 = calloc(SIZE / 2, sizeof(int));
    child->movesjoueur1 = parent->movesjoueur1;
    child->movesjoueur2 = parent->movesjoueur2;

    assert(child->board);
    /* On copie le board dans l */
    for (int i = 0; i < SIZE; i++)
        child->board[i] = parent->board[i];

    assert(child->joueur1);
    /* Copy joueur1 */
    for (int i = 0; i < SIZE / 2; i++)
        child->joueur1[i] = parent->joueur1[i];

    assert(child->joueur2);
    /* Copy joueur2 */
    for (int i = 0; i < SIZE / 2; i++)
        child->joueur2[i] = parent->joueur2[i];
}

/*La fonction isFreeRow vérifie la disponibilité d'une place libre dans une colonne donnée d'un plateau de jeu. 
Elle parcourt les cases de cette colonne de haut en bas jusqu'à trouver une case vide, indiquant la dernière rangée libre. 
Si aucune case vide n'est trouvée dès la première rangée, elle retourne -1, signifiant que la colonne est complètement pleine.*/
int isFreeRow(Item *node, int column)
{
    int row = 0;

    for (int i = 0; i < 6; i++)
    {
        if (node->board[column] == 0)
        {
            row++;
            column = column + COLUMNS;
        }
    }
    
    row = row - 1;
    return row;
}


/*La fonction getChildNode permet de simuler le placement d'une pièce dans une colonne spécifiée sur un plateau de jeu. 
Elle crée un nouveau nœud pour le jeu, vérifie si le placement est possible dans la colonne donnée, et si c'est le cas, 
enregistre le mouvement du joueur dans ce nœud. Si le coup est valide, le nouveau nœud est retourné; sinon, la fonction retourne NULL,
 indiquant que le placement n'est pas possible.
 */
Item *getChildNode(Item *node, int column, int joueur)
{

// creation du child, et le lier au parent ici, il le passent en paramètre 
    Item *child_p = nodeAlloc();
    createChild(child_p, node);
    int row = isFreeRow(node, column);

    if (row != -1)
    {
        int pos = row * COLUMNS + column;
        child_p->board[pos] = joueur; 
        if (joueur == 1)
        {
            child_p->joueur1[child_p->movesjoueur1] = pos;
            child_p->movesjoueur1++;
        }
        else
        {
            child_p->joueur2[child_p->movesjoueur2] = pos;
            child_p->movesjoueur2++;
        }
        return child_p;
    }
    else
        return NULL;
}
