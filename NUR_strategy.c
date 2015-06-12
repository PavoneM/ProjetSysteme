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

//On se sert de pstrategy comme compteur et on l'initialise à 0

void *Strategy_Create(struct Cache *pcache) {
    return pcache->pstrategy = (int*) 0;
}


void Strategy_Close(struct Cache *pcache) {}

//On réinitialise tous les bits R à 0 puis le compteur

void Strategy_Invalidate(struct Cache *pcache) {
    int ib;
    for(ib =0; ib<pcache->nblocks; ib++) {
        pcache->headers[ib].flags = pcache->headers[ib].flags & ~REFER;
    }
    // Initialisation du compteur puis incrementation du nombre de déréférençages.
    pcache->pstrategy = 0;
    ++pcache->instrument.n_deref;
}

// Determine la valeur qui permet de choisir le bloc à utiliser (moins utilisé)

int calcul(struct Cache_Block_Header *bloc) {

    int n = 0;
    int m = 0;
    if ((bloc->flags & REFER) > 0) {
        n = 1 << 1;
    }
    if ((bloc->flags & MODIF) > 0) {
        m = 1;
    }

    return m | n;
}

struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache) {

    struct Cache_Block_Header *bloc_vide = NULL;
    struct Cache_Block_Header *bloc_non_vide;
    int min=4;
    int ib;

    // On cherche d'abord un bloc invalide

    if ((bloc_vide = Get_Free_Block(pcache)) != NULL) return bloc_vide;

    // Sinon on détermine le bloc qui à été le moins utilisé

    for(ib =0; ib < pcache->nblocks; ib++) {

        int n=calcul(&pcache->headers[ib]);
        if(n==0) {
            bloc_non_vide = &pcache->headers[ib];
            return bloc_non_vide;

        }
        else {
            if(n<min) {
                min = n;
                bloc_non_vide = &pcache->headers[ib];
            }
        }
    }
    return bloc_non_vide;

}

//On passe le bit R (REFER) à 1 lors d'une lecture et on incremente le compteur

void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh) {

    int i;
    // Si le compteur atteint la periode de déréferencement, on reinitialise tous les blocs R à 0

    if(pcache->pstrategy >= pcache->nderef) {

        for (i = 0; i < pcache->nblocks; ++i) {
            pcache->headers[i].flags = pcache->headers[i].flags & ~REFER;
        }
        pcache->pstrategy = 0;
        ++pcache->instrument.n_deref;
    }

    // On met le bit R à 1
    pbh->flags = pbh->flags | REFER;
    ++pcache->pstrategy;
}

//On passe le bit R (REFER) à 1 lors d'une écriture et on incremente le compteur

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh) {

    int i;
    // Si le compteur atteint la periode de déréferencement, on reinitialise tous les blocs R à 0

    if(pcache->pstrategy >= pcache->nderef) {
        for (i = 0; i < pcache->nblocks; ++i) {
            pcache->headers[i].flags = pcache->headers[i].flags & ~REFER;
        }
        pcache->pstrategy = 0;
        ++pcache->instrument.n_deref;
    }

    // On met le bit R à 1
    pbh->flags = pbh->flags | REFER;
    ++pcache->pstrategy;
}

char *Strategy_Name()
{
    return "NUR";
}
