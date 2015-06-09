/*!
 * \file FIFO_strategy.c
 *
 * \brief  Stratégie de remplacement Fifo..
 *
 * \author Thomas
 *
 * $Id: FIFO_strategy.c
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "random.h"
#include "time.h"

/*!
 *
 *Création et initialisation d'une nouvelle liste vide en retournant un pointeur dessus
 */
void *Strategy_Create(struct Cache *pcache) {
    return Cache_List_Create();
}

/*!
 * Fermeture de la stratégie
 */
void Strategy_Close(struct Cache *pcache) {
    Cache_List_Delete((struct Cache_List *)((pcache)->pstrategy));
}

/*!
 *
 */
void Strategy_Invalidate(struct Cache *pcache) {
    Cache_List_Clear((struct Cache_List *)((pcache)->pstrategy));
}

/*!
 *
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    struct Cache_Block_Header *pbh;
    struct Cache_List *fifo_list = (struct Cache_List *)((pcache)->pstrategy);

    // On regarde si il reste un bloc libre (invalide)

    if ((pbh = Get_Free_Block(pcache)) != NULL) {

        // On rajoute un élèment dans la liste car il reste de la place dans la liste
        // On insert l'élément à la fin
        Cache_List_Append(fifo_list, pbh);
        return pbh;
    }

    // Dans le cas ou la liste est complète, on déplace le premier bloc de fifo_list à la fin de la liste

    pbh = Cache_List_Remove_First(fifo_list);
    Cache_List_Append(fifo_list, pbh);

    return pbh;
}


/*!
 * Aucun changement lorsqu'on read
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

/*!
 *  Aucun changement lorsqu'on write
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

char *Strategy_Name()
{
    return "FIFO";
}
