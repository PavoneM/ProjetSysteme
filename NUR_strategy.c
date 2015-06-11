/*!
 * \file NUR_strategy.c
 *
 * \brief  Stratégie de remplacement NUR.
 *
 * \author Ulysse Riccio / Thomas Moras
 *
 * $Id: NUR_strategy.c
 */

#include "strategy.h"
#include "low_cache.h"

/* Valeur de vérité d'un entier */
#define BOOLVAL(v) ((v) ? 1 : 0)

/* Prend 2 bits m et r et en fait un entier m*2 + r */
#define MAKE_RM(r, m) ((BOOLVAL(r)<<1) | BOOLVAL(m))


typedef struct
{
    int unsigned nderef;	/* Période de déréférençage */
    int unsigned compteur;	/* compteur */
}NUR;

/*!
 * Creation et initialisation de la stratégie (invoqué par la création de cache).
 * @author Ulysse Riccio
 */
void *Strategy_Create(struct Cache *pcache)
{
    NUR *pstrat = malloc(sizeof(NUR));

    pstrat->nderef = pcache->nderef;
    pstrat->compteur = pcache->nderef;

    return pstrat;
}

/* FeRMeture de la stratégie
 * @author Ulysse Riccio
 */

void Strategy_Close(struct Cache *pcache)
{
    //libere l'allocation de la structure qui se trouve dans pstrategy
    free(pcache->pstrategy);
}

/* Fonction "réflexe" lors de l'invalidation du cache.
 * @author Ulysse Riccio
 */

void Strategy_Invalidate(struct Cache *pcache)
{
    NUR *pstrat =  (NUR *)(pcache)->pstrategy;
    int ib;

    if (pstrat->nderef != 0)
    {
        pstrat->compteur = 1;

        // si nderef = 0 on sort
        if (!(pstrat->nderef == 0 || --pstrat->compteur > 0)) {
            for (ib = 0; ib < pcache->nblocks; ib++)
                pcache->headers[ib].flags &= ~0x4;

            // On reinitialise le compteur
            pstrat->compteur = pstrat->nderef;
            ++pcache->instrument.n_deref;
        }
    }
}

/*  Stratégie de remplacement de bloc
 *  @author Ulysse Riccio
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    int ib;
    int minimun;
    struct Cache_Block_Header *pbh_save = NULL;
    struct  Cache_Block_Header *pbh;
    int RM;


    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    char inter1,inter2;

    //la valeur de rm la plus petite
    if ( 1 == 0 )
        inter1 = 0;
    if ( 1 == 1 )
        inter1 = 1;
    if ( 1 == 0 )
        inter2 = 0;
    if ( 1 == 1 )
        inter2 = 1;

    minimun = (( inter1 << 1 ) | ( inter2 )) + 1;


    for ( ib = 0; ib < pcache->nblocks; ib++)
    {
        //minimun = MAKE_RM(1, 1) + 1,
        pbh = &pcache->headers[ib];


        if ( (pbh->flags & 0x4) == 0 )
            inter1 = 0;
        if ( (pbh->flags & 0x4) == 1 )
            inter1 = 1;
        if ( (pbh->flags & 0x2) == 0 )
            inter2 = 0;
        if ( (pbh->flags & 0x2) == 1 )
            inter2 = 1;

        RM = ( inter1 << 1 ) | ( inter2 );

        // RM = MAKE_RM(pbh->flags & 0x4, pbh->flags & MODIF);

        if ( RM == 0 ) return pbh;
        else if (RM < minimun)
        {
            minimun = RM;
            pbh_save = pbh;
        }
    }
    return pbh_save;
}

/*! Fonction "réflexe" lors de la lecture.
 *  @author Ulysse Riccio
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    NUR *pstrat =  (NUR *)(pcache)->pstrategy;
    int ib;

    // si nderef = 0 on sort
    if (!(pstrat->nderef == 0 || --pstrat->compteur > 0)) {
        for (ib = 0; ib < pcache->nblocks; ib++) pcache->headers[ib].flags &= ~0x4;

        // On reinitialise le compteur
        pstrat->compteur = pstrat->nderef;
        ++pcache->instrument.n_deref;
    }
    pbh->flags = pbh->flags | 0x4;
}

/*! Fonction "réflexe" lors de l'écriture.
 *  @author Ulysse Riccio
 *  meme chose que pour read
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
    NUR *pstrat =  (NUR *)(pcache)->pstrategy;
    int ib;

    // si nderef = 0 on sort
    if (!(pstrat->nderef == 0 || --pstrat->compteur > 0)) {
        for (ib = 0; ib < pcache->nblocks; ib++) pcache->headers[ib].flags &= ~0x4;

        // On reinitialise le compteur
        pstrat->compteur = pstrat->nderef;
        ++pcache->instrument.n_deref;
    }
    pbh->flags = pbh->flags | 0x4;
}

/*! Retourne le nom de la stratégie ici nur
 *  @author Ulysse Riccio
 */
char *Strategy_Name()
{
    return "NUR";
}
