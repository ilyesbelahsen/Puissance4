#ifndef LIST_H
#define LIST_H
#include "item.h"

/*Essentially the same object as
linked list for the chess problem */

typedef struct {
  int numElements;
  char *name; // \0 la fin d'une chaine de caractère 
  Item *first;
  Item *last;
} list_t;


Item *nodeAlloc();
void initList(list_t *list);
void freeItem(Item *node);
void cleanUpList(list_t *list);
void addLast(list_t *list, Item *node);
void delList(list_t *list, Item *node);
Item *popValue(list_t *list, int f);
void printList(list_t list);


//Item* onList( list_t *list, char *board); // return an item with same board , or null
Item *popFirst( list_t *list ); // return and remove first item utilise la fonction delList
Item *popLast( list_t *list ); // return and remove last item 
Item *popBest( list_t *list ); // return and remove best item with minimal f value utilise pour UCS
void addFirst( list_t *list, Item *node ); // add item in top
void addLast( list_t *list, Item *node ); // add item in queue
void delList( list_t *list, Item *node); // remove a node from list

#endif