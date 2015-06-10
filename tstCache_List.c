#include <stdio.h>
#include <stdlib.h>
#include "cache_list.h"
#include "low_cache.h"
#include "stdbool.h"

/** Crée et initialise une nouvelle liste (vide) et retourne un pointeur dessus */
struct Cache_List *Cache_List_Create() {
	
	struct Cache_List *cache_list = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	
	//cache_list : liste tournante
	cache_list->pheader = NULL;
	cache_list->prev = cache_list;
	cache_list->next = cache_list;
	return cache_list;
}

/** Détruit la liste pointée par list */
void Cache_List_Delete(struct Cache_List *list) {
	
	struct Cache_List *cur = list->next;
	
	while (cur != list){
		(cur->prev)->next = cur->next;
		(cur->next)->prev = cur->prev;
		free(cur);
		cur = cur->next;
	}
	free(list);
	
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	
	struct Cache_List *cur;
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	
	for (cur = list->next; cur != list; cur = cur->next) {/*on va à la fin de la liste */}
	
	new->pheader = pbh;
	new->prev = cur->prev;
    (cur->prev)->next = new;
    cur->prev = new;
    new->next = cur;
	
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh) { 
	
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	struct Cache_List *cur = list->next;
	
	//on refait les chaînages
	new->pheader=pbh;
	new->prev = cur->prev;
	(cur->prev)->next = new;
	cur->prev = new;
	new->next = cur;
	
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list) {
	
	struct Cache_List *cur = list->next;
    
    (cur->next)->prev=cur->prev;
    (cur->prev)->next=cur->next;

    return cur->pheader;

}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list) {
	
	struct Cache_List *cur;
	struct Cache_Block_Header *header;
           
    for (cur = list->next; cur != list; cur=cur->next){/*on va à la fin de la liste */}
    header = cur->pheader;
	(cur->next)->prev = cur->prev;        
    (cur->prev)->next = cur->next;
    return header;

}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh) {
	struct Cache_List *cur = list;
	struct Cache_Block_Header *header;
	
	if ((Cache_List_Is_Empty(list)) || (!pbh))
		return NULL;
	
	for (cur = list->next; cur->pheader != pbh; cur = cur->next) {/*on ne fait rien*/}
	header = cur->pheader;
	(cur->next)->prev = cur->prev;
	(cur->prev)->next = cur->next;
	return header;

}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list) {

    struct Cache_List *cur;

    for (cur = list->next; cur != list; cur = cur->next) {
        (cur->prev)->next = cur->next;
        (cur->next)->prev = cur->prev;
        free(cur);
    }
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list) {
	
	if (list->prev != list->next)
		return false;
	return true;
	
}

/*! Transférer un élément à la fin */
void Cache_List_Move_To_End(struct Cache_List *list,
                            struct Cache_Block_Header *pbh) {
	
	struct Cache_Block_Header *header = Cache_List_Remove(list, pbh);
	Cache_List_Append(list, header);

}

/*! Transférer un élément  au début */
void Cache_List_Move_To_Begin(struct Cache_List *list,
                              struct Cache_Block_Header *pbh) {

	struct Cache_Block_Header *header = Cache_List_Remove(list, pbh);
	Cache_List_Prepend(list, header);							  

}

int main() {
	
	printf("Création cache_list\n");
	struct Cache_List *c = Cache_List_Create();	
	bool x = Cache_List_Is_Empty(c);
	printf(x ? "liste vide\n" : "liste non vide\n");
	
	printf("Ajout élément\n");
	struct Cache_Block_Header *pbh = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	struct Cache_Block_Header *pbh2 = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	Cache_List_Append(c,pbh);
	Cache_List_Append(c,pbh2);
	
	int cpt = 0;
	for (c; cpt != 2; c = c->next) {
		printf("%d\n",c->pheader);
		cpt++;
	}

}
