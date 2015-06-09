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
	 struct Cache_List* cur = list;
	
	while(cur = cur->next){/* on va à la fin de la liste */}
	
	while(cur = cur->prev){
		//On part de la fin de la liste pour supprimer tous les éléments 
		free(cur->next);
	}
	
	free(cur); 
}

/*! Insertion d'un élément à la fin */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	struct Cache_List *cur = list;
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List));

	if(list->pheader==NULL)
			printf("Liste null\n");
	
	if (Cache_List_Is_Empty(list)) {
		printf("Premier element\n");
		list->pheader = pbh;
		if(list->pheader!=NULL)
			printf("Liste pas null\n");
		return;
	}
	
	while (cur->next) {
		cur  = cur->next;
		/*on va à la fin de la liste*/
	}
	printf("test");
	//on refait les chaînages
	new->next = NULL;
	new->prev = cur;
	new->pheader = pbh;
	cur->next=new;
	if(!cur->next)
		printf("Le next est null\n");
}

/*! Insertion d'un élément au début*/
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh) {
	struct Cache_List *cur = list;
	struct Cache_List *new = (struct Cache_List*) malloc(sizeof(struct Cache_List)); //création nouvel élément
	
	//on refait les chaînages
	new->pheader = pbh;
	new->next = list;
	new->prev = NULL;
	
	while (cur = cur->prev) {/*on se place tout au début de la liste*/}
	cur->prev = new;
}

/*! Retrait du premier élément */
struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list) {
		struct Cache_List *cur = list;
		struct Cache_Block_Header *header; //header du premier élément de la liste

		while (cur = cur->prev) {/*on se place tout au début de la liste*/}
		
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
		struct Cache_List *cur = list;
		struct Cache_Block_Header *header; //header du dernier élément de la liste
		
		if (Cache_List_Is_Empty(list))
			return NULL;
		
		while (cur = cur->next) {/*on va à la fin de la liste */}

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
	return (list->pheader == NULL);
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
	printf("%d\n", x);
	printf("Ajout élément\n");
	struct Cache_Block_Header *pbh = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	struct Cache_Block_Header *pbh2 = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	//struct Cache_Block_Header *pbh3 = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header));
	x = Cache_List_Is_Empty(c);
	Cache_List_Append(c, pbh);
	Cache_List_Append(c, pbh2);
	//Cache_List_Append(c, pbh3);
	printf("LOL\n\n");
	
	int compteur =0;
	while(!c->next){
		printf("%deme element\n", compteur++);
		c=c->next;
	}
		
	printf	("%d\n", x);
}
