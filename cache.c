#include "low_cache.h"
#include "strategy.h"

// Compteur d'accès (lecture/écriture)
int nacces;

struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,size_t recordsz, unsigned nderef){
	int i;	
	struct Cache *cache = malloc(sizeof(struct Cache));
	cache->fp = fopen(fic, "r+");
	cache->file = basename(fic);
	cache->nblocks = nblocks;
	cache->nrecords = nrecords;
	cache->recordsz = recordsz;
	cache->blocksz = nrecords * recordsz;
	cache->nderef = nderef;
	cache->pstrategy = Strategy_Create(cache);
	Cache_Get_Instrument(cache);
	cache->headers = malloc(sizeof(struct Cache_Block_Header)*nblocks);
	
	for(i = 0 ; i < nblocks ; i++){
		cache->headers[i].data = malloc(recordsz * nrecords);
		cache->headers[i].ibcache = i;
		cache->headers[i].flags &= 0x0;
	}
	cache->pfree = &(cache->headers[0]);
	nacces = 0;

	return cache;	
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
	free(pcache->headers);
	free(pcache);

	return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
	int i;
	nacces = 0;
	pcache->instrument.n_syncs++;

	for(i = 0 ; i < pcache->nblocks ; i++){
		if((pcache->headers[i].flags & MODIF) > 0){
			if(fseek(pcache->fp, pcache->headers[i].ibfile * pcache->blocksz, SEEK_SET) != 0)
				return CACHE_KO;

			if(fputs(pcache->headers[i].data, pcache->fp) == EOF)
				return CACHE_KO;
			pcache->headers[i].flags &= ~MODIF;
		}
	}

	return CACHE_OK;
}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){
	int i;
	for(i = 0 ; i < pcache->nblocks ; i++)
		pcache->headers[i].flags &= ~VALID;
	pcache->pfree = &(pcache->headers[0]);
	Strategy_Invalidate(pcache);	

	return CACHE_OK;
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){


	return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){


	return CACHE_OK;
}

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache){
	struct Cache_Instrument *instr = malloc(sizeof(struct Cache_Instrument));
	*instr = pcache->instrument;

	pcache->instrument.n_reads = 0;
	pcache->instrument.n_writes = 0;
	pcache->instrument.n_hits = 0;
	pcache->instrument.n_syncs = 0;
	pcache->instrument.n_deref = 0;

	return instr;
}




