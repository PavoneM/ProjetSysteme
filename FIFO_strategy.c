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
 *
 * En fait, nous initialisons le germe
 * (seed) du générateur aléatoire à quelque chose d'éminemment variable, pour
 * éviter d'avoir la même séquence à chque exécution...
 */
void *Strategy_Create(struct Cache *pcache)
{

    return NULL;
}

/*!
 *
 */
void Strategy_Close(struct Cache *pcache)
{
}

/*!
 *
 */
void Strategy_Invalidate(struct Cache *pcache)
{
}

/*!
 *
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    int ib;
    struct Cache_Block_Header *pbh;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    /* Sinon on tire un numéro de bloc le plus vieux */
    // A remplir
        return NULL;
}


/*!
 *
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

/*!
 *
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

char *Strategy_Name()
{
    return "FIFO";
}
