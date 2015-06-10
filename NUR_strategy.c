/*!
 * \file NUR_strategy.c
 *
 * \brief  Stratégie de remplacement NUR.
 * 
 * \author Ulysse RICCIO
 *
 * $Id: RAND_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"

typedef struct
{
    unsigned int nderef;
    unsigned int compteur;
} NUR;

/*!
  * //! Creation et initialisation de la stratégie (invoqué par la création de cache).
 * //@author Ulysse Riccio
*/
void *Strategy_Create(struct Cache *pcache)
{
    NUR * newNur = malloc( sizeof (NUR) * 1 );

    newNur->nderef = pcache->nderef; //!< période de déréférençage pour NUR
    newNur->compteur = pcache->nderef; //!< période de déréférençage pour NUR

    return newNur;
}

/*!
 * //! Fermeture de la stratégie.
 * //@author Ulysse Riccio
 */
void Strategy_Close(struct Cache *pcache)
{
    //libere l'allocation de la structure qui se trouve dans pstrategy
    free(pcache->pstrategy);
}

/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
}

/*! 
 * RAND : On prend le premier bloc invalide. S'il n'y en a plus, on prend un bloc au hasard.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    int ib;
    struct Cache_Block_Header *pbh;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    /* Sinon on tire un numéro de bloc au hasard */
    ib = RANDOM(0, pcache->nblocks);
    return &pcache->headers[ib];
}


/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

//@author Ulysse Riccio
//Retourne nom de la stratégie utilisée ici nur
char *Strategy_Name()
{
    return "NUR";
}
