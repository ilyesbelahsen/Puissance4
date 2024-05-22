#ifndef ITEM_H
#define ITEM_H

#define SIZE 42

#define COLUMNS 7
#define ROWS 6

typedef struct Item_s
{
    //char size; // size of board (< 255)
    char *board; // 0=board vide, 1 = utilisateur, -1 = l'IA
    //char blank;
    int f;       // costs
    struct Item_s *prev, *next;
    // struct Item_s *parent; // needed for tree structure in game searc
    //int depth;
    int movesjoueur1;
    int movesjoueur2;
    int *joueur1;
    int *joueur2; 

} Item;

#endif