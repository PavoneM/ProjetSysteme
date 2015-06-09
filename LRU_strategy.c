/*!
 * \file LRU_strategy.c
 *
 * \brief  Stratégie de remplacement LRU..
 * 
 * \author Ulysse RICCIO
 *
 * $Id: RAND_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"

//on utilise les fonctions de cache_list
#include "cache_list.h"

/*!
 * //! Creation et initialisation de la stratégie (invoqué par la création de cache).
 * //@author Ulysse Riccio
 * */
void *Strategy_Create(struct Cache *pcache)
{
    /** Crée et initialise une nouvelle liste (vide) et retourne un pointeur dessus */
    return Cache_List_Create();
}

/*!
 * //! Fermeture de la stratégie.
 * //@author Ulysse Riccio
 */
void Strategy_Close(struct Cache *pcache)
{
    /*! Destruction d'une liste de blocs */
    // prend en parametre cache_list donc typage depuis pstrategy
    // //!< Structure de données dépendant de la stratégie
    Cache_List_Delete( (struct Cache_List *) ( (pcache)->pstrategy ) );
    //(struct Cache_List *) ( (pcache)->pstrategy ) extrement utile
}

/*!
 * //! Fonction "réflexe" lors de l'invalidation du cache.
 * @author Ulysse Riccio
 */
void Strategy_Invalidate(struct Cache *pcache)
{
    /*! Remise en l'état de liste vide */
    Cache_List_Clear( (struct Cache_List *) ( (pcache)->pstrategy ) );
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
 * //! Fonction "réflexe" lors de la lecture.
 * @author Ulysse Riccio
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    /*! Transférer un élément à la fin */
    Cache_List_Move_To_End(  (struct Cache_List *) ( (pcache)->pstrategy ) , pbh);
}

/*!
 * RAND : Rien à faire ici.
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

//@author Ulysse Riccio
//Retourne nom de la stratégie utilisée ici lru
char *Strategy_Name()
{
    return "LRU";
}
