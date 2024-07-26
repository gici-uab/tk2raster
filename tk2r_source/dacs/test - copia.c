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

int main(int argc, char* argv[])
{


	uint bytesFile;
	uint listLength = 512 * 680;
	uint i;
	uint k;
	FILE * flist;	

	uint *list;
	uint16_t *list_uint16;
	list_uint16 = (uint16_t *) calloc(listLength, sizeof(uint16_t));
	list = (uint *) calloc(listLength, sizeof(uint32_t));
	uint outputSize;

	char buf[1024];

	showMessage();


	if (argc < 3) {
		fprintf(stderr,"\n Wrong call to main program\n");
		fprintf(stderr," Use:  %s  <integer list file>  <out file>\n",argv[0]);
		return 0;
	}

	bytesFile = 512*680*2; //fileSize(argv[1]);
	//listLength = 512*680;//bytesFile / sizeof(uint16_t);
	printf("No of bytes: %d\n", bytesFile);
	if (!bytesFile) {
		fprintf(stderr, "\nFILE EMPTY OR FILE NOT FOUND !!\nConstruction aborted\n");
		exit(0);
	}
	
	for (k = 0; k < 224; ++k) {
		sprintf(buf, "../data/data_%d.dat", k);
		flist = fopen(buf,"r");
		fread(list_uint16, sizeof(uint16_t) * listLength, 1, flist);		
		for (i = 0; i < listLength; ++i) {
			list[i] = list_uint16[i];
		}	
		FTRep* listRep = createFT(list,listLength);
		/*
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
*/
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
