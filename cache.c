#include "low_cache.h"
#include "strategy.h"
#define debugFonc(text) //printf("\e[1;31m%s\e[0m\n",text)
#define debug(text) //printf("\e[1;34m-->%s\e[0m\n",text)

// Compteur d'accès (lecture/écriture)
int nacces;

struct Cache_Block_Header *Cache_Is_Present(struct Cache *cache, int index){
	int i;

	for(i = 0 ; i < cache->nblocks ; i++)
		if(cache->headers[i].ibfile == index){
			if(!(cache->headers[i].flags & VALID)) return NULL;
			return &(cache->headers[i]);
		}

	return NULL;
}

struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,size_t recordsz, unsigned nderef){
	
	debugFonc("Cache_Create");

	int i;	
	struct Cache *cache = malloc(sizeof(struct Cache));
	cache->fp = fopen(fic, "rw+");
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
		cache->headers[i].flags = 0x0;
	}
	cache->pfree = &(cache->headers[0]);
	nacces = 0;

	return cache;	
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
	
	debugFonc("Cache Close");

	free(pcache->headers);
	free(pcache);

	return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
	
	debugFonc("Cache sync");

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

	debugFonc("Cache Invalidate");

	int i;
	for(i = 0 ; i < pcache->nblocks ; i++)
		pcache->headers[i].flags &= ~VALID;
	pcache->pfree = &(pcache->headers[0]);
	Strategy_Invalidate(pcache);	

	return CACHE_OK;
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){

	debugFonc("Cache read");

	struct Cache_Block_Header *block;
	int index = irfile / pcache->nrecords;

	if(block = Cache_Is_Present(pcache, index)){
		pcache->instrument.n_hits++;
		//printf("LOL\n");
		debug("Present dans cache");
	}
	else{
		debug("Pas présent dans cache");
		block = Strategy_Replace_Block(pcache);
		pcache->pfree = Get_Free_Block(pcache);

		if(block->flags & MODIF){
			if(fseek(pcache->fp, DADDR(pcache, block->ibfile), SEEK_SET) != 0)
				return CACHE_KO;

			if(fputs(block->data, pcache->fp) == EOF)
				return CACHE_KO;
		}

		if(fseek(pcache->fp, DADDR(pcache, index), SEEK_SET) != 0)
				return CACHE_KO;

		fgets(block->data, pcache->blocksz, pcache->fp);

		block->flags = VALID;
		block->ibfile = index;
	}

	return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){

	debugFonc("Cache Write");
	char *buff = (char*)precord;
	struct Cache_Block_Header *block;
	int index = irfile / pcache->nrecords;

	if(block = Cache_Is_Present(pcache, index)){
		debug("Present dans cache");
		pcache->instrument.n_hits++;
	}
	else{
		debug("Pas present dans cache");
		debug("Replace Bloc Strategy");
		block = Strategy_Replace_Block(pcache);
		debug("Recupération du premier block libre");
		pcache->pfree = Get_Free_Block(pcache);

		if(block->flags & MODIF){
			debug("Le block était un block modifié");
			if(fseek(pcache->fp, DADDR(pcache, block->ibfile), SEEK_SET) != 0)
				return CACHE_KO;

			if(fputs(block->data, pcache->fp) == EOF)
				return CACHE_KO;
		}

		debug("Je place le pointeur à l'index");
		//printf("IrFile: %d     Index: %d\n",irfile, index);
		//printf("LOOOL %d",pcache->fp);
		if(fseek(pcache->fp, DADDR(pcache, index), SEEK_SET)){
				debug("Je bug");
				return CACHE_KO;
			}

		debug("On lit les lignes à partir du fichier");
		fgets(block->data, pcache->blocksz, pcache->fp);

		block->flags = VALID;
		block->ibfile = index;
	}
	if(snprintf(ADDR(pcache, irfile, block), pcache->recordsz, "%s", buff) < 0)
		return CACHE_KO;

	block->flags |= MODIF;

	pcache->instrument.n_writes++;
	
	if(++nacces == NSYNC)
		Cache_Sync(pcache);

	Strategy_Write(pcache, block);


	return CACHE_OK;
}

//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache){
	struct Cache_Instrument *instr = malloc(sizeof(struct Cache_Instrument));
	pcache->instrument.n_reads = 0;
	pcache->instrument.n_writes = 0;
	pcache->instrument.n_hits = 0;
	pcache->instrument.n_syncs = 0;
	pcache->instrument.n_deref = 0;
	*instr = pcache->instrument;

	return instr;
}




