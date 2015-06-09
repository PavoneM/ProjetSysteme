#include "low_cache.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>


//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){

	struct Cache *cache = (struct Cache*) malloc(sizeof(struct Cache));
	int i;

	cache->file=basename(fic);
	cache->fp=fopen(fic, "r+");
	cache->nblocks=nblocks;
	cache->nrecords=nrecords;
	cache->recordsz=recordsz;
	cache->nderef=nderef;

	// pstrategy???

	struct Cache_Instrument instru;

	instru.n_reads=0;
	instru.n_writes=0;
	instru.n_hits=0;
	instru.n_syncs=0;
	instru.n_deref=0;

	cache->instrument=instru;

	struct Cache_Block_Header *headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);
	for(i=0 ; i< nblocks ; i++){
		(headers+i)->flags=VALID;
		(headers+i)->ibfile=0;
		(headers+i)->ibcache=0;
	}

	cache->headers=headers;

	cache->pfree=Get_Free_Block(cache);

	return CACHE_OK;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
	free(pcache->headers);
	free(pcache->pfree);
	free(pcache);
	return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){

	int fd = fileno(pcache->fp), i;

	for(i=0 ; i< pcache->nblocks ; i++){

		struct Cache_Block_Header *header = pcache->headers+i;
		if( header->flags & MODIF){

			char* data = header->data;

			lseek(fd, pcache->recordsz*header->ibfile, SEEK_SET);

			write(fd, data, pcache->recordsz);

		}
	}

	return CACHE_OK;
}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){
	
	int i;

	for(i=0 ; i< pcache->nblocks ; i++)
		if((pcache->headers+i)->flags & VALID)
			(pcache->headers+i)->flags-=VALID;

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
