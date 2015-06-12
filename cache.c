#include "low_cache.h"
#include "strategy.h"
#define debugFonc(text) //printf("\e[1;31m%s\e[0m\n",text)
#define debug(text) //printf("\e[1;34m-->%s\e[0m\n",text)

// Compteur d'accès (lecture/écriture)
int nbAccess;

/*
 *	Cette fonction cherche si un block à un indice donné est dans le cache.
 *	On parcours tous les header du cache (nblocks) pour trouver celui à l'index donné.
 *	Une fois le header trouvé deux solutions:
 *		- Le header est Valide (V=0) et il contient bien le block à l'indice donné
 *		- Soit on return NULL
 *	Dans tous les autres cas on return NULL.
 */
struct Cache_Block_Header *Cache_Is_Present(struct Cache *cache, int index){
	int i;

	for(i = 0 ; i < cache->nblocks ; i++)
		if(cache->headers[i].ibfile == index){
			// Si V == 0 ou que l'index n'est pas le bon : return NULL
			if(!(cache->headers[i].flags & VALID) || cache->headers[i].ibfile != index	) 
				return NULL;
			// Sinon, on retourne le header correspondant
			return &(cache->headers[i]);
		}

	return NULL;
}

/*
 *	Cette fonction initialise un nouveau Cache avec des paramètres.
 *	Entre autre, cette fonction fait:
 *		- Allocation dynamique d'une structure Cache
 *		- Ouverture d'un fichier en mode WB+
 *		- Creation des instruments de consultation du cache
 *		- Creation des header
 *	Retour: Cache correctement alloué
 */
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,size_t recordsz, unsigned nderef){
	
	int i;	
	
	// Allocation dynamique du nouveau cache
	struct Cache *cache = malloc(sizeof(struct Cache));

	// Ouverture du fichier en mode wb+
	cache->fp = fopen(fic, "wb+");

	// Affectation des paramètres
	cache->file = basename(fic);
	cache->nblocks = nblocks;
	cache->nrecords = nrecords;
	cache->recordsz = recordsz;
	cache->blocksz = nrecords * recordsz;
	cache->nderef = nderef;

	// Creation de la stratégie
	cache->pstrategy = Strategy_Create(cache);

	// Initialisation des instruments
	Cache_Get_Instrument(cache);

	// Initialisation des headers
	cache->headers = malloc(sizeof(struct Cache_Block_Header)*nblocks);
	for(i = 0 ; i < nblocks ; i++){
		cache->headers[i].data = malloc(recordsz * nrecords);
		cache->headers[i].ibcache = i;
		cache->headers[i].flags = 0x0;
	}

	// Obtention du premier block disponible
	cache->pfree = &(cache->headers[0]);

	// Initialisation de la variable d'accès mémoire à 0
	nbAccess = 0;

	return cache;	
}

/*
 *	Cette fonction efface le cache et libère toute la mémoire
 *	
 *	Pour se faire nous effectuons les opérations suivantes:
 *		- Synchronisation du cache pour éviter la perte de donnée
 *		- Libération des headers et des données
 *		- Libération du cache
 *	Retour: Si l'opération s'est bien effectuée (CACHE_OK ou CACHE_KO)
 */
Cache_Error Cache_Close(struct Cache *pcache){

	int i;

	// Synchro du cache
	Cache_Sync(pcache);

	// Libération des données
	for (i = 0; i < pcache->nblocks; i++)
        free(pcache->headers[i].data);   

    // Libération des headers
	free(pcache->headers);

	// Libération du cache
	free(pcache);

	return CACHE_OK;
}

/*
 *	Cette fonction synchronise ce qu'il y a sur la cache vers le fichier
 *	La synchronisation est une opération qui permet de transférer les changements
 *	qui ont été faits localement dans le cache vers le fichier.
 *	Cette fonction va donc:
 *		- Rechercher parmi tous les headers du cache ce qui ont été modifiés
 *		- Si un header est modifié on recherche l'index du block
 *		- On se positionne dans le fichier à l'emplacement du block
 *		- On écrit dans le fichier les données présentes dans le champs data du header
 *		- On remet le flag de la modification à 0
 *	Retour: Si l'opération s'est bien effectuée (CACHE_OK ou CACHE_KO)
 */
Cache_Error Cache_Sync(struct Cache *pcache){
	
	int i;

	// Recherche sur tous les blocks du cache
	for(i = 0 ; i < pcache->nblocks ; i++){

		// Si un header a été modifié
		if((pcache->headers[i].flags & MODIF) > 0){

			// Placement du SEEK
			if(fseek(pcache->fp, pcache->headers[i].ibfile * pcache->blocksz, SEEK_SET) != 0)
				return CACHE_KO;

			// Ecriture dans le fichier
			if(fputs(pcache->headers[i].data, pcache->fp) == EOF)
				return CACHE_KO;

			// Modification du flag
			pcache->headers[i].flags &= ~MODIF;
		}
	}

	// Remise à 0 du compteur d'accès et incrémentation de n_syncs
	nbAccess = 0;
	pcache->instrument.n_syncs++;

	return CACHE_OK;
}

/*
 *	Cette fonction réinitialise le cache (c'est à dire on met tous les bits V à 0)
 *	Attention ! Il est necessaire de synchroniser le cache avant de l'invalider
 *	car on risque une perte de donnée! 
 *	Il faut donc:
 *		- Synchroniser le cache
 *		- Faire une boucle sur tous les blocks et les rendre invalides (V=0)
 *		- Appeler la Callback de Strategy pour invalider le cache
 *	Retour: Si l'opération s'est bien effectuée (CACHE_OK ou CACHE_KO)
 */
Cache_Error Cache_Invalidate(struct Cache *pcache){

	// Synchronisation du cache
	if(Cache_Sync(pcache) == CACHE_KO) return CACHE_KO;

	int i;

	// Remise à V=0
	for(i = 0 ; i < pcache->nblocks ; i++)
		pcache->headers[i].flags &= ~VALID;

	// Changement du premier block disponible
	pcache->pfree = &(pcache->headers[0]);

	// Appel de la Callback de Strategy
	Strategy_Invalidate(pcache);	

	return CACHE_OK;
}

/*
 *	Cette fonction est utilisée par Cache_Read et Cache_Write.
 *	Elle est issue de la factorisation de l'algorithme d'écrite et de lecture dans le cache.
 *	En effet elle permet vérifier si un block est présent ou non dans le cache. Si le block n'y est
 *	pas, on l'ajoute dans l'emplacement demandé par la stratégie:
 *		- Incrémentation du nombre d'accès et vérification pour la synchronisation
 *		- Vérification si le block est présent ou pas
 *		- Si le block n'est pas présent on l'ajoute
 *	Retour: Pointeur sur le header que nous venons d'ajouter
 */
struct Cache_Block_Header* Cache_Block_Create(struct Cache *pcache, int index){

	// Initialisation de la variable 
	struct Cache_Block_Header *block;

	// Synchronisation du cache
	if(++nbAccess >= NSYNC)
		Cache_Sync(pcache);

	// Vérification si le header est présent
	if(block = Cache_Is_Present(pcache, index))
		pcache->instrument.n_hits++;
	else{

		// Callback de la stratégie pour remplacer le header
		block = Strategy_Replace_Block(pcache);
		pcache->pfree = Get_Free_Block(pcache);

		// Si le block a été modifié
		if(block->flags & MODIF){

			// On se positionne au bit où le block commence
			if(fseek(pcache->fp, DADDR(pcache, block->ibfile), SEEK_SET) != 0)
				return CACHE_KO;

			// On ajoute les données à data
			if(fputs(block->data, pcache->fp) == EOF)
				return CACHE_KO;
		}

		// On écrit dans le fichier
		if(fseek(pcache->fp, DADDR(pcache, index), SEEK_SET)){
			return CACHE_KO;
		}

		fgets(block->data, pcache->blocksz, pcache->fp);

		// Flag VALID à 1
		block->flags = VALID;
		block->ibfile = index;
	}
	return block;
}

/*
 *	Cette fonction permet d'écrire dans le fichier depuis une information stoquée dans le cache
 *	Tout d'abord nous récupérons l'information stoquée dans precord pour ensuite l'écrire dans le cache.
 *		- Récupération de l'infomation dans le precord
 *		- Recherche du header correspondant
 *		- Ecriture de l'information dans le cache à l'index indiqué
 *		- Modification des flags 
 *	Retour: Si l'opération s'est bien effectuée (CACHE_OK ou CACHE_KO)
 */
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){

	// Cration des données et du header
	char *buff = (char*)precord;
	struct Cache_Block_Header *block;

	// Récupération du header
	if((block=Cache_Block_Create(pcache, irfile / pcache->nrecords)) == NULL)
		return CACHE_KO;

	// Copie de l'information dans le header
	if(snprintf(ADDR(pcache, irfile, block), pcache->recordsz, "%s", buff) < 0)
		return CACHE_KO;

	// Incrémentation des instruments et modification des flags 
	pcache->instrument.n_writes++;	
	block->flags |= MODIF;
	Strategy_Write(pcache, block);

	return CACHE_OK;
}

/*
 *	Cette fonction permet de lire une information depuis le cache
 *	Tout d'abord nous récupérons l'information du cache pour la retransmettre au buffer
 *		- Récupération de l'infomation dans le precord
 *		- Recherche du header correspondant
 *		- Recopie de l'information dans precord
 *		- Modification des flags 
 *	Retour: Si l'opération s'est bien effectuée (CACHE_OK ou CACHE_KO)
 */
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){

	// Creation du header
	struct Cache_Block_Header *block;

	// Recupération du header du cache
	if((block=Cache_Block_Create(pcache, irfile / pcache->nrecords)) == NULL)
		return CACHE_KO;

	// Copie dans precord
    memcpy(precord, ADDR(pcache, irfile, block) , pcache->recordsz);

    // Incrémentation des variables
	pcache->instrument.n_reads++;	

	Strategy_Read(pcache, block);

	return CACHE_OK;
}


/*
 *	Récupération de la liste des instruments puis remise à 0 des instruments
 *	Retour: Instruments
 */
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache){

	// Instruments que vont être renvoyés 
	struct Cache_Instrument *instr = malloc(sizeof(struct Cache_Instrument));
	
	// Copie des instruments
	*instr = pcache->instrument;

	// Remise à zéro
	pcache->instrument.n_reads = 0;
	pcache->instrument.n_writes = 0;
	pcache->instrument.n_hits = 0;
	pcache->instrument.n_syncs = 0;
	pcache->instrument.n_deref = 0;

	return instr;
}