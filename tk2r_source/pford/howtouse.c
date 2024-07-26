#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "pfordelta.h"
#include "s16.h"
#include "coding_policy_helper.h"
#include "coding_policy.h"
#include <libgen.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>


int _block_size = 64;

// Print unsigned integer in binary format with spaces separating each byte.
void print_binary(unsigned int num) {
  int arr[32];
  int i = 0;
  while (i++ < 32 || num / 2 != 0) {
    arr[i - 1] = num % 2;
    num /= 2;
  }

  for (i = 31; i >= 0; i--) {
    printf("%d", arr[i]);
    if (i % 8 == 0)
      printf(" ");
  }
}

void test_compression() {
  int num_blocks = 2;
  int size = 130;
  unsigned int ay[130] = {1,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};
  unsigned int *array=NULL;
  unsigned int *decompressor_out=NULL;
  unsigned int *compressor_out=NULL;

  int a, b, i;

  array = malloc(sizeof(unsigned int)*_block_size*num_blocks);
  memcpy(array, ay, size);
  printf("size array: %ld\n", sizeof(ay)/sizeof(unsigned int));
  printf("size compressor out: %d\n", CompressedOutBufferUpperbound(_block_size) * num_blocks);
  printf("size decompressor out: %d\n", UncompressedOutBufferUpperbound(_block_size * num_blocks));

  compressor_out = malloc( sizeof(unsigned int) * CompressedOutBufferUpperbound(_block_size) * num_blocks);
  a = compress_pfordelta(array, compressor_out, size, _block_size);
  for(i = 0; i < a; i++) {
    //print_binary(compressor_out[i]);
  }
  printf("space used by pfordelta: %d\n", a);
  free(compressor_out);
  free(array);
  return;

  decompressor_out = malloc( sizeof(unsigned int) * UncompressedOutBufferUpperbound(_block_size * num_blocks));
  printf("========== decompressing ==============\n");
  b = decompress_pfordelta(compressor_out, decompressor_out, size, _block_size, 0);
  printf("printing\n");
  for(i = 0; i < size; i++) {
  //  printf("%u -> %u\n", array[i], decompressor_out[i]);
  }

  printf("a: %d\nb: %d\n", a, b);
}

void test_s16() {
  unsigned int input[] = {2322231,2,3,4,5,6,7,8,9,10};
  unsigned int *coded;
  unsigned int *output;

  int size = 10;
  int newsize;
  int finalsize;
  int i;

  coded = (unsigned int *) malloc( size * sizeof(unsigned int) );
  output = (unsigned int *) malloc( size * sizeof(unsigned int) );
  //printf("coded = %X\n", coded);
  //printf("output = %X\n", output);
  newsize = s16_compress(input, coded, size);
  finalsize = s16_decompress(coded, output, size);

  printf("Normal size: %d\n", size);
  printf("Compress size: %d\n", newsize);
  printf("Consumed size: %d\n", finalsize);
  for(i = 0; i < size; i++) {
    printf("%u -> %X -> %u\n", input[i], coded[i], output[i]);
  }
}

uint64_t get_file_size(const char *fname)
{
	uint64_t fsize = 0L;
	FILE *fp = fopen(fname, "rb");

    if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		fclose(fp);
	}
	return fsize;
}

int get_base_name(char *input_path_name, char *output_base_name)
{
	char* local_file = input_path_name;

	//char* ts1 = strdup(local_file);
	char* ts2 = strdup(local_file);

	//char* dir = dirname(ts1);
	char* fname = basename(ts2);
	if (fname == NULL)
		return -1;
	strcpy(output_base_name, fname);

	printf("File name: %s\n", output_base_name);
	
	free(ts2);
	return 0;
}


int file_exists(const char* fname)
{
	return access(fname, F_OK) != -1 ? 0 : -1;
}

int main(int argc, char *argv[]) {

	//unsigned int input[] = {2322231,2,3,4,5,6,7,8,9,10};
	//unsigned int input[128] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,10,10,10,10,10,10,10};
 
	//int j;
 
	//int bloc_size = 128;
	if (argc < 7) {
		fprintf(stderr, "Required parameters missing.\n");
		fprintf(stderr, "Usage: howtouse file_name k_val begin_band end_band [block_size] [padded/unpadded]\n");
		fprintf(stderr, "\n[padded/unpadded]: padded = 0, unpadded = 1\n");
		return (EXIT_FAILURE);
	}
 
	char buf[PATH_MAX]       = {0};
	char data_file_name[PATH_MAX] = {0};
	char data_file_name_without_path[PATH_MAX] = {0};	
	
	strcpy(data_file_name, argv[1]);
	uint16_t k_val     = atoi(argv[2]);
	int32_t  begin     = atoi(argv[3]);
	int32_t  end       = atoi(argv[4]);
	int32_t  bloc_size = atoi(argv[5]);
	int32_t  padded    = atoi(argv[6]); // 0 = padded, 1 = unpadded
 
	int size;
	
	int newsize = 0;
	int finalsize;
	int i;
	uint32_t k;
	uint32_t total_size = 0;
	uint32_t *output = NULL;	
 	uint32_t *compressed = NULL;
	

	strcpy(buf, data_file_name);
	if (file_exists(buf) < 0)
	{
		fprintf(stderr, "\nFile does not exist. ");
		return -1;
	}
	if (get_base_name(buf, data_file_name_without_path) < 0)
	{
		fprintf(stderr, "\nCannot get file name. ");
		return -1;
	}
	
	//uint32_t **s = (uint32_t **) calloc((end - begin), sizeof(uint32_t *));
	uint32_t **input = (uint32_t **) calloc((end - begin), sizeof(uint32_t *));
	
	// MAX
	for (k = begin; k < end; ++k)
	{
		if (padded == 0)
			sprintf(buf, "../k2raster/data/nodacs_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);
		else
			sprintf(buf, "../unpadded/data/unpadded_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);

		uint64_t t = get_file_size(buf);
		if (t == 0) {
			// FILE *fp600 = fopen(buf, "w");
			// fclose(fp600);
			continue;
		}
		size = t / sizeof(uint32_t);
		//printf("size: %d\n", size);
		
		int multiples = ((size + bloc_size) / bloc_size) * bloc_size;
		input[k]   = (uint32_t *)calloc(multiples, sizeof(uint32_t));
		compressed = (uint32_t *)calloc(size,      sizeof(uint32_t));
		output     = (uint32_t *)calloc(multiples, sizeof(uint32_t));
		
		FILE *fp1 = fopen(buf, "r");
		if (fp1 == NULL) {
			continue;
		}
		int ret = fread(input[k], sizeof(uint32_t), size, fp1);

		if (ret != size) {
			fprintf(stderr, "fread error.\n");
			return (EXIT_FAILURE);
		}
		fclose(fp1);

		//printf("k=%d\n", k);

		newsize = compress_pfordelta(input[k], compressed, multiples, bloc_size); // go to pfordelta.c
		finalsize = decompress_pfordelta(compressed, output, multiples, bloc_size, 0);
		
		// write data to file
		if (padded == 0)
			sprintf(buf, "./data_padded/pford%d_%s_k%d_Max_%04d.dat", bloc_size, data_file_name_without_path, k_val, k); // save compressed file
		else
			sprintf(buf, "./data_unpadded/pford%d_%s_k%d_Max_%04d.dat", bloc_size, data_file_name_without_path, k_val, k); // save compressed file
		FILE *fp200 = fopen(buf, "w");
		fwrite(&size, 1, sizeof(uint32_t), fp200);
		fwrite(compressed, newsize, sizeof(uint32_t), fp200);
		fclose(fp200);
		
		if (newsize != finalsize)
			printf("newsize is not equal to finalsize.\n");
		
		if (memcmp(input[k], output, multiples * sizeof(uint32_t)) != 0)
			printf("memcmp not equal.\n");
		
		for (i = 0; i < multiples; ++i)
		{
			if (input[k][i] == output[i])
				;//printf("same\n");
			else
				printf("not same. %d\n", i);
		}
		
		// pfor_decompress(&compressed[s[k][1]], reconstructed, 0); 
		// for (i = 0; i < 32; i++)
			// printf("%d: %08x\n", i, reconstructed[i]);

		total_size += newsize;

		if (compressed)
			free(compressed);
		if (output)
			free(output);
	}
	printf("(k=%d)\tTotal size Max: %d\n", k_val, total_size * 4);
	
/*
	uint32_t      *reconstructed = (uint32_t *)calloc(bloc_size, sizeof(uint32_t));
	double         elapsed_time;	
	clock_t        t = clock();	
	struct timeval time_begin;
	struct timeval time_end;
	gettimeofday(&time_begin, NULL);

	for (k = begin; k < end; ++k) {

		int orig_size = 0;
		sprintf(buf, "./data/pford%d_%s_k%d_Max_%04d.dat", bloc_size, data_file_name_without_path, k_val, k);
		int compressed_size = get_file_size(buf) / sizeof(uint32_t);
		s[k] = (uint32_t *) calloc(compressed_size, sizeof(uint32_t));

		uint32_t *compressed = (uint32_t *)calloc(compressed_size, sizeof(uint32_t));

		FILE * fp300 = fopen(buf, "r");
		if (fp300 == NULL)
			continue;
		fread(&orig_size, sizeof(uint32_t), 1, fp300);
		fread(compressed, sizeof(uint32_t), compressed_size, fp300);
		fclose(fp300);
		
		int count = 0;
		for (i = 0; i < compressed_size; )
		{
			uint32_t s1 = (compressed[i] >> 16) & 0xFF;
			uint32_t s2 = (compressed[i] >> 24) & 0xFF;

			////printf("%d: %d\n", i, s1);
			s[k][count] = i;
			i += s1 + s2 + 1;

			count++;
		}
	
		for (j = 0; j < count; j++) 
		{
			memset(reconstructed, 0 , bloc_size * sizeof(uint32_t));
			pfor_decompress(&compressed[s[k][j]], reconstructed, bloc_size);
			
			for (i = 0; i < bloc_size; ++i)
			{
				if ((i + j * bloc_size) < orig_size && 
				    reconstructed[i] != input[k][i + j * bloc_size])
					printf("Mismatch at pos %d in band %d: (recon: %d\tinput: %d)\n",
				           i + j * bloc_size,
						   k,
				           reconstructed[i],
						   input[k][i + j * bloc_size]);
			}
			
			// for (i = 0; i < bloc_size; i++)
				// printf("%d: %08x\n", i, reconstructed[i]);
			// if (reconstructed[j % bloc_size] != input[k][j])
				// printf("Doesn't match at pos %d\n", j);
			
		}		

		if (compressed)
			free(compressed);
	}
	
	gettimeofday(&time_end, NULL);
	elapsed_time = (time_end.tv_sec - time_begin.tv_sec) +
				   (time_end.tv_usec - time_begin.tv_usec) * 1.0e-6;
	t = clock() - t;
	printf("Elapsed time (using gettimeofday()): %9f milliseconds\n", elapsed_time * 1000);
	printf("Elapsed time (using clock()):        %9f milliseconds\n", ((double)t/CLOCKS_PER_SEC) * 1000);
	
	printf("Done. %s\t%s\n", __DATE__, __TIME__);
	
	if (reconstructed)
		free(reconstructed);
	
	for (k = 0; k < (end-begin); ++k) {  
		free(input[k]);
		free(s[k]);
	}
		
	free(input);
	free(s);
*/
	for (k = 0; k < end-begin; k++)
		if (input[k])
			free(input[k]);
		
	// MIN
	total_size = 0;
	newsize = 0;
	for (k = begin; k < end; ++k)
	{
		if (padded == 0)
			sprintf(buf, "../k2raster/data/nodacs_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);
		else
			sprintf(buf, "../unpadded/data/unpadded_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);
		uint64_t t = get_file_size(buf);
		if (t == 0) {
			// FILE *fp600 = fopen(buf, "w");
			// fclose(fp600);
			continue;
		}
		size = t / sizeof(uint32_t);
		
		int multiples = ((size + bloc_size) / bloc_size) * bloc_size;
		input[k]   = (uint32_t *)calloc(multiples, sizeof(uint32_t));
		compressed = (uint32_t *)calloc(size, sizeof(uint32_t));
		output     = (uint32_t *)calloc(multiples, sizeof(uint32_t));

		//printf("%d\tsize: %d\n", k, size);
	
		FILE *fp1 = fopen(buf, "r");
		int ret = fread(input[k], sizeof(uint32_t), size, fp1);
		if (ret != size) {
			fprintf(stderr, "fread error.\n");
			return (EXIT_FAILURE);
		}
		fclose(fp1);

		newsize = compress_pfordelta(input[k], compressed, size, bloc_size);
		finalsize = decompress_pfordelta(compressed, output, size, bloc_size, 0);
		
		if (padded == 0)
			sprintf(buf, "./data_padded/pford%d_%s_k%d_Min_%04d.dat", bloc_size, data_file_name_without_path, k_val, k); // save compressed file
		else
			sprintf(buf, "./data_unpadded/pford%d_%s_k%d_Min_%04d.dat", bloc_size, data_file_name_without_path, k_val, k); // save compressed file
		FILE *fp200 = fopen(buf, "w");
		fwrite(&size, 1, sizeof(uint32_t), fp200);
		fwrite(compressed, newsize, sizeof(uint32_t), fp200);
		fclose(fp200);
		
		if (newsize != finalsize)
			printf("newsize is not equal to finalsize.\n");
		
		if (memcmp(input[k], output, multiples * sizeof(uint32_t)) != 0)
			printf("memcmp not equal.\n");
		
		//printf("%d\t%d\n", newsize, finalsize);
		
		total_size += newsize;

		if (compressed != NULL)
			free(compressed);
		if (output != NULL)
			free(output);
	}
	printf("(k=%d)\tTotal size Min: %d\n", k_val, total_size * 4);	


  // printf("Normal size: %d\n", size);
  // printf("Compress size: %d\n", newsize);
  // printf("Consumed size: %d\n", finalsize);
  // for(i = 0; i < size; i++) {
    // printf("%u -> %X -> %u\n", input[i], coded[i], output[i]);
  // }
  
  //test_compression();
  printf("*************************\n");  
  printf("*************************\n");
  /*test_s16();
  printf("*************************\n");
  int i;
  int a;
  unsigned int p[128];
  unsigned int t[128];
  p[0] = 2;
  p[1] = 2;
  p[2] = 3;
  p[3] = 0xffff;  
  a = compress_pfordelta(p,t,4,32);
  
  printf("new size: %d\n",a);
  for (i = 0; i < a; i++ ) { print_binary(t[i]); printf("\n"); };*/
  return 0;
}
