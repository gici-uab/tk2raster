/*-----------------------------------------------------------------------
 File       : Test. main file.
 ------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <malloc.h>
#include <stdio.h>
#include <stdint.h>

#include "constants.h"
#include "dacs.h"

/*------------------------------------------------------------------
 Obtains the size of the file to compress.
 ------------------------------------------------------------------ */
unsigned long fileSize (char *filename) {
	FILE *fpText;
	unsigned long fsize;
	fpText = fopen(filename,"rb");
	fsize=0;
	if (fpText) {
		fseek(fpText,0,2);
		fsize= ftell(fpText);
		fclose(fpText);
	}
	return fsize;
}

/* Shows a message with info of this version of the code */
void showMessage() {
	printf("\n*Directly Addressable Codes (DACs)");
	printf("\n*CopyRight(c)2011  Nieves R. Brisaboa, Susana Ladra and Gonzalo Navarro\n");
	
    printf("\n  #DACs come with ABSOLUTELY NO WARRANTY; ");
    printf("\n  #This is free software, and you are welcome to redistribute it ");
    printf("\n  #under certain conditions\n\n");
}

/*------------------------------------------------------------------ */
/*
 *  TEST
 */
/*------------------------------------------------------------------ */
/*
uint32_t matrix[4*4*4] = 
{
	 1000000,  2000000,  3000000, 4000000,
	 5000000,  6000000,  7000000, 8000000,
	 9000000, 10000000, 11000000, 12000000,
	13000000, 14000000, 15000000, 16000000,
                                        
	17000000, 18000000, 19000000, 20000000,
	21000000, 22000000, 23000000, 24000000,
	25000000, 26000000, 27000000, 28000000,
	29000000, 30000000, 31000000, 32000000,
	                                    
	33000000, 34000000, 35000000, 36000000,
	37000000, 38000000, 39000000, 40000000,
	41000000, 42000000, 43000000, 44000000,
	45000000, 46000000, 47000000, 48000000,
	                                    
	49000000, 50000000, 51000000, 52000000,
	53000000, 54000000, 55000000, 56000000,
	57000000, 58000000, 59000000, 60000000,
	61000000, 62000000, 63000000, 64000000
};
*/

// uint32_t matrix[4*4*4] = 
// {
	 // 100,  200,  300,  400,
	 // 500,  600,  700,  800,
	 // 900, 1000, 1100, 1200,
	// 1300, 1400, 1500, 1600,
                          
	// 17000, 18000, 19000, 20000,
	// 21000, 22000, 23000, 24000,
	// 25000, 26000, 27000, 28000,
	// 29000, 30000, 31000, 32000,
	                          
	// 33000, 34000, 35000, 36000,
	// 37000, 38000, 39000, 40000,
	// 41000, 42000, 43000, 44000,
	// 45000, 46000, 47000, 48000,
	                          
	// 49000, 50000, 51000, 52000,
	// 53000, 54000, 55000, 56000,
	// 57000, 58000, 59000, 60000,
	// 61000, 62000, 63000, 64000
// };

uint32_t matrix[4*4*4] = 
{
0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,
       
0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,
       
0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,
       
0,0,0,0,
0,0,0,0,
0,0,0,0,
0,0,0,0,
};



int main(int argc, char* argv[])
{

	uint bytesFile;
	uint listLength = 4*4*4;
	uint i;
	uint k;
	FILE * flist;	

	uint *list;
	//uint16_t *list_uint16;
	//list_uint16 = (uint16_t *) calloc(listLength, sizeof(uint16_t));
	list = (uint *) calloc(listLength, sizeof(uint32_t));
	uint outputSize;

	FTRep* listRep = createFT(matrix, listLength);
	
	//decompressFT(listRep, listLength);
	
	for (i = 0; i < listLength; ++i)
		printf("%d: %d\n", i+1, accessFT(listRep, i));
	
	saveFT(listRep, "data1.dat");
	destroyFT(listRep);	

}



	// if (argc < 3) {
		// fprintf(stderr,"\n Wrong call to main program\n");
		// fprintf(stderr," Use:  %s  <integer list file>  <out file>\n",argv[0]);
		// return 0;
	// }

	// bytesFile = 512*680*2; //fileSize(argv[1]);
	// //listLength = 512*680;//bytesFile / sizeof(uint16_t);
	// printf("No of bytes: %d\n", bytesFile);
	// if (!bytesFile) {
		// fprintf(stderr, "\nFILE EMPTY OR FILE NOT FOUND !!\nConstruction aborted\n");
		// exit(0);
	// }

	// for (i = 0; i < listLength; ++i)
		// list[i] = matrix[i];

/*
	for (k = 0; k < 224; ++k) {
		sprintf(buf, "../data/data_%d.dat", k);
		flist = fopen(buf,"r");
		fread(list_uint16, sizeof(uint16_t) * listLength, 1, flist);		
		for (i = 0; i < listLength; ++i) {
			list[i] = list_uint16[i];
		}	
		FTRep* listRep = createFT(list,listLength);
		
		uint * listaux;

		//Test: decompression
		listaux = decompressFT(listRep,listLength);
		uint conterrors = 0;
		for(i=0;i<listLength;i++)
			if(listaux[i]!=list[i]){
				fprintf(stderr,"Error on decompressFT, the %d-th decompressed element does not match with the original list\n",i+1);
				conterrors ++;
			}
		fprintf(stderr," Test: decompression -> %d mismatches\n", conterrors);
		free(listaux);
		
		//Test: direct access
		uint value;
		conterrors = 0;
		for(i=0;i<listLength;i++){
			value = accessFT(listRep, i+1);
			if(value!=list[i]){
				conterrors ++;
				fprintf(stderr,"Error on decompressFT, the %d-th decompressed element does not match with the original list\n",i+1);
			}
		}
		fprintf(stderr," Test: direct access -> %d mismatches\n", conterrors);

		sprintf(buf, "./data/%d.dat", k);

		saveFT(listRep, buf);
		fprintf(stderr,"\n\n Representation saved.\n");
		outputSize = fileSize(buf);
		printf("Output file size: %d\n", outputSize);

		printf("%0.4f\n", (double)outputSize / (double)bytesFile);

		destroyFT(listRep);
		fclose(flist);
	}
	
	free(list);
	free(list_uint16);

}
*/

/*
	bytesFile = fileSize(argv[1]);
	listLength = bytesFile / sizeof(uint);
	if (!bytesFile) {
		fprintf(stderr, "\nFILE EMPTY OR FILE NOT FOUND !!\nConstruction aborted\n");
		exit(0);
	}

	flist = fopen(argv[1],"r");
	list = (uint *) malloc(sizeof(uint)*listLength);
	fread(list,sizeof(uint),listLength,flist);
	fclose(flist);
*/
