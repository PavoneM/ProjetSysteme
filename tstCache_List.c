#include <stdio.h>
#include <stdlib.h>
#include "cache_list.h"
#include "low_cache.h"
#include "stdbool.h"

/** Crée et initialise une nouvelle liste (vide) et retourne un pointeur dessus */
struct Cache_List *Cache_List_Create() {
	struct Cache_List *cache_list = (struct Cache_List*) malloc(sizeof(struct Cache_List));
	
	//cache_list est vide			
	cache_list->pheader = NULL;
	cache_list->prev = NULL; 
	cache_list->next = NULL;
	return cache_list;
}

/** Détruit la liste pointée par list */
void Cache_List_Delete(struct Cache_List *list) {
	struct Cache_List *tmp;
	struct Cache_List *cur = list;
	
	while (cur->next != NULL){
		cur = tmp;
		cur = tmp->next;
		free(tmp);
	}
	
	if (cur->next == NULL && cur != NULL)
		free(cur);
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	struct Cache_List *cur;
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List));

	for (cur = list; cur->next != NULL; cur = cur->next) {/*on va à la fin de la liste */}
	
	//on refait les chaînages
	new->next = NULL;
	new->prev = cur;
	new->pheader = pbh;
	cur->next=new;
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List)); //création nouvel élément
	
	//on refait les chaînages
	new->pheader = pbh;
	new->next = list;
	new->prev = NULL;
	list = new;
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list) {
		struct Cache_List *cur = list;
		struct Cache_Block_Header *header; //header du premier élément de la liste

		while (cur) {
			/*on se place tout au début de la liste*/
			cur = cur->prev;
		}
		
		if (Cache_List_Is_Empty(list))
			return NULL;
		
		//on refait les chaînages
		header = cur->pheader;
		(cur->next)->prev = NULL;
		free(cur);
		return header;
}

/*! Retrait du dernier élément */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list) {
		struct Cache_List *cur;
		struct Cache_Block_Header *header; //header du dernier élément de la liste
		
		if (Cache_List_Is_Empty(list))
			return NULL;
		
		for (cur = list; cur->next != NULL; cur = cur->next) {/*on va à la fin de la liste */}
		
		//on refait les chaînages...
		header = cur->pheader;
		(cur->prev)->next = NULL;
		free(cur);
		return header;
}

/*! Retrait d'un élément quelconque */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh) {
	struct Cache_List *cur;
	struct Cache_Block_Header *header;
	
	if ((Cache_List_Is_Empty(list)) || (!pbh))
		return NULL;
			
	for (cur = list->next; cur->pheader != pbh; cur = cur->next) {/*on ne fait rien*/}
	header = cur->pheader;
	(cur->prev)->next = cur->next;
	(cur->next)->prev = cur->prev;
	free(cur);
	
	return header;
}

/*! Remise en l'état de liste vide */
void Cache_List_Clear(struct Cache_List *list) {
	list->pheader = NULL;
	list->next = NULL;
	list->prev = NULL;
}

/*! Test de liste vide */
bool Cache_List_Is_Empty(struct Cache_List *list) {
	return ((list->next == NULL) || (list->next->pheader == NULL));
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
	printf	(x ? "liste vide\n" : "liste pas vide\n");
	printf("Ajout élément\n");
	struct Cache_Block_Header *pbh = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	struct Cache_Block_Header *pbh2 = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	Cache_List_Prepend(c, pbh);
	x = Cache_List_Is_Empty(c);
	printf("%d\n", pbh);
	printf	(x ? "liste vide\n" : "liste pas vide\n");
	printf("Ajout élément\n");
	Cache_List_Prepend(c, pbh2);
	x = Cache_List_Is_Empty(c);
		printf("%d\n", pbh2);
	printf	(x ? "liste vide\n" : "liste pas vide\n");
	
	for (struct Cache_List *list = c; list != NULL; list = list->next) {
			printf("%d\n", list->pheader);
	}
	
	
}
