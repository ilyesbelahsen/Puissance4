#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#define true 1
#define false 0

Item *nodeAlloc()
{
    Item *node;

    node = (Item *)malloc(sizeof(Item));
    assert(node);

    node->board = NULL;
    node->joueur1 = NULL;
    node->joueur2 = NULL;
    node->prev = NULL;
    node->next = NULL;
    node->f = 0;

    return node;
}

void initList(list_t *list)
{
    assert(list);
    list->numElements = 0;
    list->first = NULL;
    list->last = NULL;
}

void freeItem( Item *node )
{
  if (node && node->board) free(node->board);
	if (node) free(node);
}


// Add item to queue
 // add item in queue
void addLast( list_t *list, Item *node ) // add in tail
{
  if(list->numElements == 0){
    list->first = node;
    list->last = node;  
  }
  else{
    list->last->next = node;
    node->prev = list->last;
    list->last = node;
  }
    list->numElements++;
}


// Remove node from list
void delList(list_t *list, Item *node)
{
    assert(list);
    assert(node);
    if (node->prev)
    {
        node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
    }
    else
    {
        assert(list->first == node);
        if (node->next)
        {
            list->first = node->next;
            if (list->first)
                list->first->prev = NULL;
        }
    }
    if (node->next == NULL)
    {
        assert(list->last == node);
        list->last = node->prev;
    }
    node->next = node->prev = NULL;
    list->numElements--;
    if (list->numElements == 0)
    {
        list->first = NULL;
        list->last = NULL;
    }
}

// Remove first occurence of item with specific f-value
Item *popValue(list_t *list, int f)
{
    assert(list->first);
    Item *item = list->first;
    while (item)
    {
        if (item->f == f)
        {
            delList(list, item);
            return item;
        }
        item = item->next;
    }
}

void printList(list_t list)
{
    Item *item = list.first;
    while (item)
    {
        printf(" %d ; ", item->f);
        item = item->next;
    }
    printf(" (nb_items: %d)\n", list.numElements);
}

// j'ai ajouter cette fonction car j'aurai besoin dans la fonction on list pour faire une comparaison entre les boards
/*
int chaine_egaux(char *c1, char *c2, int size){
int i = 0;
while(i<size){
  if(*(c1+i) != *(c2+i)){
    return false;
  }
  i++;
  }
return true;
}

*/

// return an item with corresponding board , or null
/*
Item* onList( list_t *list, char *board)
{
  Item* tmp = list->first;
  for(int i=0; i<list->numElements; i++){ 
    if(chaine_egaux(tmp->board, board, tmp->size)) {
        return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}
*/

Item *popFirst( list_t *list ) //
{
	Item *tmp; 
  tmp = list->first;
  delList(list,list->first);
  return tmp;
}

// return and remove last item
Item *popLast( list_t *list ) //
{
	Item *tmp; 
  tmp = list->last;
  delList(list,list->last);
  return tmp;
}

// le min d'une liste
int min_List (list_t *list){
  float min = 10000;
  Item *tmp = list->first; 
  for(int i=0; i<list->numElements; i++){
        if(tmp->f < min){ min = tmp->f; }
        tmp = tmp->next;
  }
  return min;
}

Item *popBest( list_t *list ) // and remove the best board from the list.
{ 
  float min = min_List(list);
  Item *tmp = list->first;
  for(int i=0; i<list->numElements; i++){ 
    if(tmp->f == min){
      delList(list,tmp);
      return tmp;
    }
     tmp = tmp->next;
  }
  return NULL;
}

void addFirst( list_t *list, Item *node ) // add in head
{
    if(list->numElements == 0) {
    list->first = node;
    list->last = node;  
  }
   else {
    list->first->prev = node;
    node->next = list->first;
    list->first = node;
  }
   list->numElements++;
}

void cleanUpList(list_t *list)
{
    Item *current = list->first;
    Item *tmp;
    while (current)
    {
        tmp = current->next;
        freeItem(current);
        current = tmp;
    }
    list->first = NULL;
    list->last = NULL;
    list->numElements = 0;
}





// TEST LIST
/*
int main()
{
  Item *item;
	char str[3];

	list_t openList;
  
	initList( &openList );

	for (int i=0; i<10; i++) {
		item = nodeAlloc();
		item->f = i;
		sprintf(str, "%2d", i);
        item->size = 3;
		item->board = strdup(str);
		addLast( &openList, item );
	}
  printList(openList);
	printf("\n");

	for (int i=20; i<25; i++) {
        item = nodeAlloc();
        item->f = i;
        sprintf(str, "%2d", i);
        item->size = 3;
        item->board = strdup(str);
        addFirst( &openList, item );
    }
	printList(openList);
	printf("\n");

    ///
    ///
	Item *node = popBest( &openList );
	printf("best node = %.2f\n", node->f);
	printList(openList);
	printf("\n");

	strcpy(str, "23");
	node = onList(&openList, str);
	if (node) printf("found %s: %.2f!\n", str, node->f); 
	printList(openList);
	printf("\n");

	
    item = nodeAlloc();
    item->f = 50;
    sprintf(str, "50");
    item->size = 3;
    item->board = strdup(str);
    addLast( &openList, item );

	node = popFirst(&openList);
	if (node) printf("first: %.2f\n", node->f); 
	printList(openList);
  printf("\n");

	node = popLast(&openList);
	if (node) printf("last: %.2f\n", node->f); 
	printList(openList);
  printf("\n");

	printf("clean\n");	
	cleanupList(&openList);
	printList(openList);
	printf("\n");
  
	return 0;
}
*/