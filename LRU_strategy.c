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
 * //! Algorithme de remplacement de bloc.
 * @author Ulysse Riccio
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{

    struct Cache_Block_Header *pbh;
    struct Cache_List *lru_list = (struct Cache_List *) ( (pcache)->pstrategy );

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL)
    {
        /*! Insertion d'un élément à la fin */
        // Les blocs invalides a mettre dans la queue
        Cache_List_Append(lru_list, pbh);
        return pbh;
    }

    /*! Retrait du premier élément */
    // Sinon on prend le premier bloc de la liste LRU et on le déplace à la fin
    pbh = Cache_List_Remove_First(lru_list);
    /*! Insertion d'un élément à la fin */
    Cache_List_Append(lru_list, pbh);

    return pbh;
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
 * //! Fonction "réflexe" lors de l'écriture.
 * @author Ulysse Riccio
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    // on fait la meme chose que dans la lecture
    Cache_List_Move_To_End(  (struct Cache_List *) ( (pcache)->pstrategy ) , pbh);
}

//@author Ulysse Riccio
//Retourne nom de la stratégie utilisée ici lru
char *Strategy_Name()
{
    return "LRU";
}
