#include "low_cache.h"


// Recherche d'un bloc libre.
struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache){
	struct Cache_Block_Header *cbh;
	int i ;
	for(i = 0; i<pcache->nblocks; i++){
		cbh =  &(pcache->headers[i]);
		if((cbh->flags & VALID) == 0)
			return cbh;
	}

	return NULL;
}