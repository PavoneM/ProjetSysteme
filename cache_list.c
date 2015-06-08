/*!
 * \file low_cache.c
 *
 * \brief  
 * 
 * \author 
 */

#include <stdio.h>
#include "cache_list.h"
#include "stdbool.h"

static Cache_List *cache_list = NULL;

/** Crée et initialise une nouvelle liste (vide) et retourne un pointeur dessus */
struct Cache_List *Cache_List_Create() {

}

/** Détruit la liste pointée par list */
void Cache_List_Delete(struct Cache_List *list) {}

/**Crée une nouvelle cellule de liste contenant le pointeur pbh et l’insère en queue de liste */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh) {}

/**Crée une nouvelle cellule de liste contenant le pointeur pbh et l’insère en tête de liste */
void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh) {}

/** Retourne le pointeur sur le premier entête de la liste et détruit la
 * cellule correspondante de la liste. Retourne NULL si la liste est vide. */
struct Cache_Block_Header*Cache_List_Remove_First(struct Cache_List *list)  {}

/** Retourne le pointeur sur le dernier entête de la liste et détruit la
 * cellule correspondante de la liste. Retourne NULL si la liste est vide. */
struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list, struct Cache_Block_Header *pbh) {}

/** Cherche la cellule contenant le pointeur pbh et la retire de la liste. 
 * Retourne pbh (ou NULL si la liste est vide ou si elle ne contient pas pbh). */
struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list, struct Cache_Block_Header *pbh) {}

/** Détruit toutes les cellules de la liste, qui redevient donc vide. */
void Cache_List_Clear(struct Cache_List *list) {}

/** Retourne true si la liste est vide, false sinon. */
boolean Cache_List_Is_Empty(struct Cache_List *list) {}

/** Cherche la cellule contenant le pointeur pbh et la transfère en queue de liste. 
 * Si la liste ne contient pas pbh, ce dernier est ajouté à la fin de la liste. Si
pbh est déjà à sa position de destination, rien ne se produit. */
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh) {}

/** Cherche la cellule contenant le pointeur pbh et la transfère en tête de liste. 
 * Si la liste ne contient pas pbh, ce dernier est ajouté en tête de la liste. Si
pbh est déjà à sa position de destination, rien ne se produit. */
void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh) {}
