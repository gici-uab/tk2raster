#include "dacs.h"
#include "../utils.h"

/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

#define FACT_RANK 20

ushort * optimizationk(uint * list, int listLength, uint *nkvalues){
	
	int t,i,m,k;

	uint maxInt = 0;
	//int maxInt = -2147483648; //-32768;
	
	for (i = 0; i < listLength; i++)
		if (maxInt < list[i]) 
			maxInt = list[i];

	///printf("maxInt: %u\n", maxInt);
	uint nBits = bits(maxInt)-1;
	////printf("nBits: %u\n", nBits);
	
	uint tamAux = nBits+2;
	
	uint * weight = (uint *)calloc(maxInt+1, sizeof(uint)); //malloc(sizeof(uint)*(maxInt+1));

	for(i=0;i<listLength;i++)
		weight[list[i]]++;
	
	uint * acumFreq = (uint *)calloc(tamAux, sizeof(uint)); //malloc(sizeof(uint)*tamAux);
	uint acumValue = 0;
	for (i = 0; i < 10; i++){
	//for(i=0;i<(int)(maxInt+1);i++){
		acumValue += weight[i];
	}
	
	acumFreq[0] = 0;	
	acumValue   = 0;
	uint cntb   = 1;
	for (i = 0; i < (int)(maxInt+1);i++)
	{
		if (i == (1 << cntb))
		{
			acumFreq[cntb]=acumValue;
			////printf("acumValue: %d\t", acumFreq[cntb]);
			cntb++;
		}
			
		acumValue += weight[i];
		
	}
	free(weight);
	
	
	///printf("\ncntb: %d\n", cntb);
	
	acumFreq[cntb] = listLength;

	long * s = (long *) malloc(sizeof(long)*(nBits+1));
	uint * l = (uint *) malloc(sizeof(uint)*(nBits+1));
	uint * b = (uint *) malloc(sizeof(uint*)*(nBits+1));

	ulong currentSize;
	
	m = nBits;

	s[m]=0;
	l[m]=0;
	b[m]=0;
		
	uint * fc = acumFreq;
	
	ulong minSize;
	uint minPos;
	for (t = m; t >= 0; t--)  // algorithm 1 begins here
	{
		minSize = -1;
		minPos = m;
		for (i = m; i >= t + 1; i--)
		{
			currentSize = s[i] +
			              (fc[m + 1] - fc[t]) * (i - t + 1) +
						  (fc[m + 1] - fc[t]) / FACT_RANK; // s[] holds the subproblems
			if (minSize > currentSize)
			{
				minSize = currentSize;
				minPos = i;
			}
		}

		if (minSize < (fc[m + 1] - fc[t]) * (m - t + 1))
		{
			s[t] = minSize;
			l[t] = l[minPos] + 1;
			b[t] = minPos-t;
		}
		else 
		{
			s[t] = (fc[m + 1] - fc[t]) * (m - t + 1);
			l[t] = 1;
			b[t] = m - t + 1;
		}
	}
	
	int L = l[0];
	
	////printf("\nL (nkvalues): %d\n", L);
	
	ushort *kvalues = (ushort *)malloc(sizeof(ushort) * L);
	
	t = 0;
	for (k = 0; k < L; k++)
	{
		kvalues[k] = b[t];
		////printf("kvalues[%d] = %d\n", k, kvalues[k]);
		t = t + b[t];
	}
	
	free(acumFreq);
	free(l);
	free(b);
	free(s);

	*nkvalues = L;
	return kvalues; // return a uint16_t array with L number of elements
}

FTRep* createFT(uint *list, uint listLength)
{
	FTRep *rep = (FTRep *) malloc(sizeof(struct sFTRep));
	uint *levelSizeAux;
	uint *cont;	
	uint *contB;
	
	ushort* kvalues;
	uint nkvalues;

	///printf("%d\n", listLength);
	
	rep->listLength = listLength;
	//register uint i;
	uint i;
	int j, k;
	uint value, newvalue;
	uint bits_BS_len = 0;

// Begin optimization
	kvalues = optimizationk(list, listLength, &nkvalues);
	
	// printf("kvalues: %d, nkvalues: %d\n", *kvalues, nkvalues);
	// for (i = 0; i < nkvalues; ++i) {
		// printf("%d ", *(kvalues+i));
	// }
	// printf("\n");
	
	uint kval;
	uint oldval = 0;
	uint newval = 0;

	i = 0;
	uint multval=1;
	do {
		oldval = newval;
		if (i >= nkvalues)
			kval = 1 << (kvalues[nkvalues-1]);
		else
			kval = 1 << (kvalues[i]);	
		multval *= kval;
		newval = oldval + multval;
		////printf("multval: %u\tkval: %u\tnewval: %u\toldval: %u\n", multval, kval, newval, oldval);
		i++;
	} while (oldval < newval);
	
	rep->tamtablebase = i; // i is the size of the table base but where did i come from???
	///printf("tamtablebase (i) = %d\n", rep->tamtablebase);
	rep->tablebase = (uint *)calloc(rep->tamtablebase, sizeof(uint));
	levelSizeAux   = (uint *)calloc(rep->tamtablebase, sizeof(uint));
	cont           = (uint *)calloc(rep->tamtablebase, sizeof(uint));
	contB          = (uint *)calloc(rep->tamtablebase, sizeof(uint));

	oldval  = 0;
	newval  = 0;
	multval = 1;
	for (i = 0; i < rep->tamtablebase; i++) {
		oldval = newval;
		if (i >= nkvalues)
			kval = 1 << (kvalues[nkvalues-1]);
		else
			kval = 1 << (kvalues[i]);	
		multval *= kval;
		newval = oldval + multval;
		rep->tablebase[i] = oldval;
	}	

	for (i = 0; i < listLength; i++) {
		value = list[i];
		for (j = 0; j < (int)rep->tamtablebase; j++) // modifed by Kevin
			if (value >= rep->tablebase[j]) {
				//printf("levelSizeAux[%d] before: %d\ttablebase[%d]: %d\n", j, levelSizeAux[j], j, rep->tablebase[j]);
				levelSizeAux[j]++;
				//printf("levelSizeAux[%d] after: %d\n", j, levelSizeAux[j]);
			}
	}
	
// Done optimization

//////////

// Begin the real stuff
	j = 0;

	while ((j < (int)rep->tamtablebase) && (levelSizeAux[j] != 0)) { // modified by Kevin
		j++;
	}
	rep->nLevels = j;

	rep->levelsIndex = (uint   *)calloc(rep->nLevels + 1, sizeof(uint));
	rep->base        = (uint   *)calloc(rep->nLevels,     sizeof(uint));
	rep->base_bits   = (ushort *)calloc(rep->nLevels,     sizeof(ushort));
	rep->iniLevel    = (uint   *)calloc(rep->nLevels,     sizeof(uint));
	// bits_BS_len = 0;

	for (i = 0; i < rep->nLevels; i++){
		if (i >= nkvalues) {
			rep->base[i] = 1 << (kvalues[nkvalues-1]);
			rep->base_bits[i] = kvalues[nkvalues-1];
		} else {
			rep->base[i] = 1 << (kvalues[i]);
			rep->base_bits[i] = kvalues[i];
		}
	}

	uint tamLevels =0;

	tamLevels=0;
	for (i = 0; i < rep->nLevels; i++)
		tamLevels += rep->base_bits[i] * levelSizeAux[i];

	//rep->iniLevel = (uint *)malloc(sizeof(uint) * rep->nLevels);		
	rep->tamCode  = tamLevels;
	
	uint indexLevel = 0;
	rep->levelsIndex[0] = 0;
	for (j = 0; j < rep->nLevels; j++) {
		rep->levelsIndex[j+1] = rep->levelsIndex[j] + levelSizeAux[j];
		rep->iniLevel[j] = indexLevel;
		cont[j] = rep->iniLevel[j];
		indexLevel += levelSizeAux[j] * rep->base_bits[j];
		contB[j] = rep->levelsIndex[j];
	}

	//rep->levels = (uint *) malloc(sizeof(uint)*(tamLevels/W+1));
	rep->levels = (uint *) calloc(tamLevels/W+1, sizeof(uint));

	bits_BS_len = rep->levelsIndex[rep->nLevels-1] + 1; 

	uint *bits_BS = (uint *)calloc(bits_BS_len / W + 1, sizeof(uint));

// writes e[p..p+len-1] = s, len <= W
// void bitwrite (register uint *e, register uint p, 
// register uint len, register uint s)

	for (i = 0; i < listLength; i++){
		value = list[i]; // examine each value
		
		// for (j = rep->nLevels-1; j >= 0; j--) {
			// printf("%d\t%d\t%d\t%d\n", i, j, value, rep->tablebase[j]);
		// }
		// printf("**********\n");
		for (j = rep->nLevels-1; j >= 0; j--) {
			if (value >= rep->tablebase[j]) {
				newvalue = value - rep->tablebase[j];
				for (k = 0; k < j; k++) {
					// printf("%d\t%x\t%d\tlen=%d\t%d\t%d\n", k, rep->levels[0], cont[k], rep->base_bits[k], newvalue%rep->base[k], tamLevels);
					// printf("------> ");
					// for (uint m = 0; m < tamLevels/W+1; m++)
						// printf("%x\t", rep->levels[m]);
					// printf("\n");
					bitwrite(rep->levels, cont[k], rep->base_bits[k], newvalue%rep->base[k]);
					cont[k] += rep->base_bits[k];
					contB[k]++;

					newvalue = newvalue/rep->base[k];
				}
				//k = j;
				bitwrite(rep->levels, cont[j], rep->base_bits[j], newvalue%rep->base[j]);
				cont[j] += rep->base_bits[j];
				contB[j]++;
				if (j < rep->nLevels - 1){
					bitset(bits_BS,contB[j]-1);
				}					
				break;
			}
		}
	}

	bitset(bits_BS, bits_BS_len - 1);

	rep->bS = createBitRankW32Int(bits_BS, bits_BS_len, 1, 20); 	

	rep->rankLevels = (uint *)calloc(rep->nLevels, sizeof(uint));
	for (j = 0; j < rep->nLevels; j++) {
		rep->rankLevels[j] = rank(rep->bS, rep->levelsIndex[j] - 1);
		////printf("createFT: rankLevels - %d\n", rep->rankLevels[j]);
	}
	
	// printf("uint listLength: %d\n",    rep->listLength );
	// printf("byte nLevels: %d\n",       rep->nLevels );
	// printf("uint tamCode: %d\n",       rep->tamCode );
	// printf(printf("uint * levels: %d\n",      *rep->levels );
	// printf("uint * levelsIndex: %d\n", *rep->levelsIndex );
	// printf("uint * iniLevel: %d\n",    *rep->iniLevel );
	// printf("uint * rankLevels: %d\n",  *rep->rankLevels );
	// printf("bitRankW32Int * bS: %d\n", *(*rep->bS).data );	
	// printf("uint * base: %d\n",        *rep->base );
	// printf("ushort * base_bits: %d\n", *(rep->base_bits) );
	// printf("uint * tablebase: %d\n",   *rep->tablebase );
	// printf("uint tamtablebase: %d\n",  rep->tamtablebase );

	free(cont);
	free(contB);
	free(levelSizeAux);
	free(kvalues);

	return rep;
}
/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

void print_bits(unsigned int num) 
{
	unsigned int size = sizeof(unsigned int);
    //unsigned int maxPow = 1 << (size*8-1);
    uint32_t i;
    for (i = 0; i < size * 8; ++i) {
		// print last bit and shift left.
		///printf("%u", num & maxPow ? 1 : 0);
		num = num << 1;
    }
}

uint accessFT(FTRep *listRep, uint param) {
	// printf("PRINT_BITS:\n");
	// print_bits(32);
	// printf("\n");
	
	uint mult       = 0;
	uint partialSum = 0;
	//uint ini        = param - 1;
	uint nLevels    = listRep->nLevels;
	uint *level     = listRep->levels;
	uint readByte;
	uint cont;
	uint pos;
	uint rankini;
	register uint j = 0;
	//int  i;
	
	//param -= 1;

	///printf("\nstart: accessFT:nLevels = %d\n", nLevels);

	pos = listRep->levelsIndex[j] + param;
	
	///printf("levelsIndex1[j]: %d\n", listRep->levelsIndex[j]);

	///printf("pos1: %d\n", pos);

	///printf("iniLevel1: %d\n", listRep->iniLevel[j]);
	cont = listRep->iniLevel[j] + param * listRep->base_bits[j];
	
	///printf("cont: %d\n", cont);
	///for (i = 0; i < listRep->nLevels; ++i)
		///printf("base_bits[%d]: %d\n", i, listRep->base_bits[i]);
	///for (i = 0; i < listRep->nLevels; ++i)
		///printf("iniLevel[%d]: %d\n", i, listRep->iniLevel[i]);
	///for (i = 0; i < listRep->nLevels + 1; ++i)
		///printf("levelsIndex[%d]: %d\n", i, listRep->levelsIndex[i]);	
	
//int sizeLevels = listRep->tamCode/W + 1;
	///printf("sizeLevels: %d\n", sizeLevels);
	///for (i = 0; i < sizeLevels; ++i) {
		//printf("level[%d] =\t", i);
		///print_bits(level[i]);
		///printf("\n");
	///}
	///printf("\n");
	
	readByte = bitread(level, cont, listRep->base_bits[j]);
	
	///printf("readByte1: %d\n", readByte);
	
	if (nLevels == 1)
		return readByte;

	///printf("accessFT: bitget: %d\n", bitget(listRep->bS->data, pos));	
	while (!bitget(listRep->bS->data, pos)) {
		rankini = rank(listRep->bS, listRep->levelsIndex[j] + param - 1) - listRep->rankLevels[j];
		
		///printf("param: %d\trankini: %d\n", param, rankini);
		param = param - rankini;

		///printf("mult %d\n", mult);

		partialSum += (readByte << mult);

		mult += listRep->base_bits[j];
		j++;

		cont = listRep->iniLevel[j] + param * listRep->base_bits[j];
		///printf("iniLevel2: %d\n", listRep->iniLevel[j]);
		///printf("cont2: %d\n", cont);
		pos = listRep->levelsIndex[j] + param;
		///printf("pos2: %d\n", pos);
		///printf("levelsIndex2[j]: %d\n", listRep->levelsIndex[j]);

		///printf("base_bits2: %d\n", listRep->base_bits[j]);
		readByte = bitread(level, cont, listRep->base_bits[j]);
		///printf("readByte2: %d\n", readByte);

		if (j == nLevels - 1)
			break;
	}

	///printf("partialSum3 = %d\tmult = %d\tlistRep->tablebase[j] = %d\n", partialSum, mult, listRep->tablebase[j]);

	partialSum = partialSum + (readByte << mult) + listRep->tablebase[j];

	///printf("end: accessFT:nLevels = %d\n", nLevels);

	return partialSum;
}


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */

uint * decompressFT(FTRep * listRep, uint n){
	///printf("\nStarting decompressFT...\n");
	uint mult=0;
	register uint i;
	register uint j;
	uint partialSum = 0;
	uint nLevels = listRep->nLevels;
	uint *level = listRep->levels;
	uint readByte;
	uint *list = (uint *) malloc(sizeof(uint) * n);
	uint *cont = (uint *) malloc(sizeof(byte*) * listRep->nLevels);
	uint *pos  = (uint *) malloc(sizeof(uint)  * listRep->nLevels);

 	for (j = 0; j < nLevels; j++){
		cont[j] = listRep->iniLevel[j];
		pos[j]  = listRep->levelsIndex[j];

	}

	for (i = 0; i < n; i++){
		partialSum = 0;
		j=0;
			
		mult=0;
		readByte = bitread(level, cont[j], listRep->base_bits[j]);
		///printf("readByte: %d\n", readByte);
		cont[j] += listRep->base_bits[j];
		///printf("cont[j] = %d, listRep->base_bits[j] = %d\n", cont[j], listRep->base_bits[j]);

		while (!bitget(listRep->bS->data,pos[j])) {
			///printf("decompressFT: bitget: %d\n", bitget(listRep->bS->data,pos[j]));

			pos[j]++;

			partialSum = partialSum + (readByte<<mult);
			mult += listRep->base_bits[j];
			j++;

			readByte = bitread(level, cont[j], listRep->base_bits[j]);
			cont[j]+=listRep->base_bits[j];

			if(j==nLevels-1){
				break;
			}
					
		}

		if(j < nLevels - 1){
			pos[j]++;
		}
		
		
		partialSum = partialSum + (readByte << mult) + listRep->tablebase[j];

		list[i] = partialSum;
	}
	free(cont);
	free(pos);
	
	///printf("Ending decompressFT...\n\n");
	return list;
}


/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */
void destroyFT(FTRep * rep){
	free(rep->levelsIndex);
	free(rep->iniLevel);
	free(rep->rankLevels);
	free(rep->tablebase);
	free(rep->levels);
	destroyBitRankW32Int(rep->bS);
	free(rep->base);
	free(rep->base_bits);
	free(rep);
}

/*-----------------------------------------------------------------

 ---------------------------------------------------------------- */
void saveFT(FTRep *rep, char *filename){
	FILE *flist;
	flist = fopen(filename,"w");
	
	fwrite(&rep->listLength,   sizeof(uint),                  1, flist);
	fwrite(&rep->nLevels,      sizeof(byte),                  1, flist);
	fwrite(&rep->tamCode,      sizeof(uint),                  1, flist);
	fwrite(&rep->tamtablebase, sizeof(uint),                  1, flist);
	fwrite(rep->tablebase,     sizeof(uint),  rep->tamtablebase, flist);	//printf("rep->tamtablebase:\t%d\n", rep->tamtablebase);
	fwrite(rep->base_bits,     sizeof(ushort),     rep->nLevels, flist);	//printf("rep->nLevels:\t%d\n", rep->nLevels);
	fwrite(rep->base,          sizeof(uint),       rep->nLevels, flist);
	fwrite(rep->levelsIndex,   sizeof(uint),     rep->nLevels+1, flist);
	fwrite(rep->iniLevel,      sizeof(uint),       rep->nLevels, flist);
	fwrite(rep->rankLevels,    sizeof(uint),       rep->nLevels, flist);
    fwrite(rep->levels,        sizeof(uint),   rep->tamCode/W+1, flist);	//printf("rep->tamCode/W+1:\t%d\n", rep->tamCode/W+1);
	
	// for (uint m = 0; m < rep->tamCode/W+1; m++) {
		// printf("rep->levels[%d] = 0x%08x\n", m, rep->levels[m]);
	// }

	save(rep->bS, flist);
	
	fclose(flist);
}

FTRep* loadFT(char * filename){
	FILE * flist;
	size_t result = 0;
	FTRep * rep = (FTRep *) malloc(sizeof(struct sFTRep));
	flist = fopen(filename,"r");
	
	result = fread(&rep->listLength, sizeof(uint), 1, flist);
	if (result != 1) {fputs ("Reading error1", stderr); exit (3);}
	
	result = fread(&rep->nLevels, sizeof(byte), 1, flist); if (result != 1)
	{fputs ("Reading error2",stderr); exit (3);}

	result = fread(&rep->tamCode, sizeof(uint), 1, flist); if (result != 1)
	{fputs ("Reading error3",stderr); exit (3);}

	result = fread(&rep->tamtablebase, sizeof(uint), 1, flist); if (result != 1)
	{fputs ("Reading error4",stderr); exit (3);}
	
	rep->tablebase = (uint *) malloc(sizeof(uint) * rep->tamtablebase);
	result = fread(rep->tablebase, sizeof(uint), rep->tamtablebase, flist);
	if (result != rep->tamtablebase) {fputs ("Reading error5",stderr); exit (3);}
	
	rep->base_bits = (ushort *) malloc(sizeof(ushort) * rep->nLevels);
	result = fread(rep->base_bits, sizeof(ushort), rep->nLevels, flist);
	if (result != rep->nLevels) {fputs ("Reading error6",stderr); exit (3);}
	
	rep->base = (uint *) malloc(sizeof(uint) * rep->nLevels);
	result = fread(rep->base, sizeof(uint), rep->nLevels,flist);
	if (result != rep->nLevels) {fputs ("Reading error7",stderr); exit (3);}
	
	rep->levelsIndex = (uint *) malloc(sizeof(uint) * (rep->nLevels+1));
	result = fread(rep->levelsIndex, sizeof(uint), rep->nLevels+1, flist);
	if (result != (size_t)(rep->nLevels+1)) {fputs ("Reading error8",stderr); exit (3);}
	
	rep->iniLevel = (uint *) malloc(sizeof(uint) * rep->nLevels);
	result = fread(rep->iniLevel, sizeof(uint), rep->nLevels, flist);
	if (result != rep->nLevels) {fputs ("Reading error9",stderr); exit (3);}

	rep->rankLevels = (uint *) malloc(sizeof(uint)*(rep->nLevels));
	result = fread(rep->rankLevels, sizeof(uint), rep->nLevels, flist);
	if (result != rep->nLevels) {fputs ("Reading error10",stderr); exit (3);}
	
	rep->levels = (uint *) malloc(sizeof(uint)*(rep->tamCode/W+1));	
	result = fread(rep->levels, sizeof(uint), (rep->tamCode/W+1), flist);
	if (result != (rep->tamCode/W+1)) {fputs ("Reading error11",stderr); exit (3);}

	rep->bS = (bitRankW32Int *) malloc(sizeof(struct sbitRankW32Int));
	load(rep->bS,flist);	
	
	fclose(flist);
	
	return rep;
}

uint memoryUsage(FTRep* rep) {
	return sizeof(uint)*rep->tamtablebase 
	+ sizeof(ushort)*rep->nLevels
	+ sizeof(ushort)*rep->nLevels
	+ sizeof(uint)*(rep->nLevels+1)
	+ sizeof(uint)*(rep->nLevels)
	+ sizeof(uint)*(rep->nLevels)
	+ sizeof(uint)*(rep->tamCode/W+1)
	+ spaceRequirementInBits(rep->bS)/8
	+ sizeof(struct sFTRep);
}
