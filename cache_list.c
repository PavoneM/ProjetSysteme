/*!
 * \file low_cache.c
 * 
 * \author Cindy Najjar
 * \author Manuel Pavone
 */

#include <stdio.h>
#include <stdlib.h>
#include "cache_list.h"
#include "stdbool.h"

/** Crée et initialise une nouvelle liste (vide) et retourne un pointeur dessus */
struct Cache_List *Cache_List_Create() {
	
	struct Cache_List *cache_list = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	struct Cache_Block_Header header;
	
	//cache_list est vide			
	cache_list->pheader = header;
	cache_list->prev = cache_list; 
	cache_list->next = cache_list;
	return cache_list;
}

/** Détruit la liste pointée par list */
void Cache_List_Delete(struct Cache_List *list) {
	struct Cache_List *cur, *suiv;

	//d'abord on vide la liste
    for (cur = list->next; cur != list; cur = suiv) {
        suiv = cur->next; //on enregistre le pointeur dans suiv
        free(cur); //on supprime l'élément courant
    }
    
    //ensuite on détruit la liste
    free(list);
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	
	//on refait les chainages
	(list->prev)->next = new;
	new->prev = list->prev;
	list->prev = new;
	new->next = list;
	new->pheader = pbh;
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	struct Cache_List *new = malloc(sizeof(struct Cache_List));S
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list) {}
/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list) {}
/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh) {}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list) {}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list) {}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list,
                            struct Cache_Block_Header *pbh) {}
/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list,
                              struct Cache_Block_Header *pbh) {}
