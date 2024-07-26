#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "simple16.h"

#define MIN(a,b)		(((a)<(b))?(a):(b))

uint64_t get_file_size(const char *fname)
{
	uint64_t fsize = 0L;
	FILE *fp = fopen(fname, "rb");

    if (fp) {
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		fclose(fp);
	}
	return fsize;
}

void print_size(const char *data_file_name_without_path, uint32_t nz, uint16_t k_val)
{
	char buf[PATH_MAX] = {0};
	uint64_t size1 = 0;
	uint64_t size2 = 0;
	uint64_t size3 = 0;
	uint32_t k;

	printf("==== SIZE (k=%d) ====\n", k_val);

	sprintf(buf, "../k2raster/data/%s_k%d_data.dat", data_file_name_without_path, k_val);
	size1 = get_file_size(buf);

	printf("T+rMax+rMin:   %10ld Bytes\n", size1);

	// Max
	for (k = 0; k < nz; ++k) {
		sprintf(buf, "../k2raster/data/simple9_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);
		size2 += get_file_size(buf);
	}

	printf("Max:           %10ld Bytes\n", size2);

	// Min
	for (k = 0; k < nz; ++k) {
		sprintf(buf, "../k2raster/data/simple9_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);
		size3 += get_file_size(buf);
	}

	printf("Min:           %10ld Bytes\n", size3);
	

	printf("==================================================\n");
	printf("Total Size:    %10ld Bytes (%2.3f MB) [k=%d] [File: %s]\n", size1+size2+size3, (double)(size1+size2+size3)/(double)1024/(double)1024, k_val, data_file_name_without_path);
}


void remove_data(const char *file_name, uint32_t nz, uint16_t k_val)
{
	uint32_t k;
	char     buf[PATH_MAX] = {0};
	
	for (k = 0; k < nz; k++) {
		sprintf(buf, "../k2raster/data/simple9_%s_k%d_Max_%04d.dat", file_name, k_val, k);
		remove(buf);
		sprintf(buf, "../k2raster/data/simple9_%s_k%d_Min_%04d.dat", file_name, k_val, k);
		remove(buf);
	}
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

int main(int argc, char **argv)
{
	char     buf[PATH_MAX] = {0};
	uint32_t k = 0;
	size_t   list_size = 0; //357; //12888; // 357
	uint32_t *list1 = NULL;
	uint32_t *output = NULL;
	FILE     *fp100 = NULL;
	uint32_t size;
	uint32_t nz = 224;
	uint16_t k_val = 2;
	uint32_t padded = 0;

	if (argc < 5) {
		fprintf(stderr, "Required parameters missing.\n");
		fprintf(stderr, "Usage: test_simple16 [data file] [nz] [k value] [padded/unpadded]\n");
		fprintf(stderr, "\n[padded/unpadded]: padded = 0, unpadded = 1\n");
		return (EXIT_FAILURE);
	}

	int64_t total_size = 0;	
	char data_file_name[PATH_MAX] = {0};
	char data_file_name_without_path[PATH_MAX] = {0};
	
	strcpy(data_file_name, argv[1]);
	nz         = atoi(argv[2]);
	k_val      = atoi(argv[3]);
	padded     = atoi(argv[4]);
	
	printf("%u\t%d\n", nz, k_val);

	strcpy(buf, data_file_name);
	if (file_exists(buf) < 0) {
		fprintf(stderr, "\nFile does not exist. ");
		return -1;
	}
	if (get_base_name(buf, data_file_name_without_path) < 0) {
		fprintf(stderr, "\nCannot get file name. ");
		return -1;
	}

	// MAX
	printf("Doing MAX....\n");

	if (padded == 0)
		sprintf(buf, "../k2raster/data/%s_k%d_max_size.dat", data_file_name_without_path, k_val);
	else
		sprintf(buf, "../unpadded/data/unpadded_%s_k%d_max_size.dat", data_file_name_without_path, k_val);

	fp100 = fopen(buf, "r");

	for (k = 0; k < nz; ++k)
	{
		fread(&size, sizeof(uint32_t), 1, fp100);
		//printf("size: %d\n", size);
		list_size = size;
		
		list1 = calloc(list_size, sizeof(uint32_t));
		output = calloc(list_size, sizeof(uint32_t));
		
		if (padded == 0)
			sprintf(buf, "../k2raster/data/nodacs_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);
		else
			sprintf(buf, "../unpadded/data/unpadded_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);

		FILE *fp1 = fopen(buf, "r");
		if (fp1 != NULL)
		{
			fread(list1, list_size, sizeof(uint32_t), fp1);
			fclose(fp1);
			
			int32_t output_size = s16_compress(list1, output, size);
			if (k < 100)
				printf("%d: %d\n", k, output_size);
			total_size += output_size + output_size / 100;
			
			if (padded == 0)
				sprintf(buf, "./data_padded/simple16_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);
			else
				sprintf(buf, "./data_unpadded/simple16_%s_k%d_Max_%04d.dat", data_file_name_without_path, k_val, k);
			
			FILE *pf2 = fopen(buf, "w");
			fwrite(&list_size, sizeof(uint32_t), 1, pf2);
			fwrite(output, sizeof(uint32_t), output_size, pf2);
			fclose(pf2);
			
			//decode and see if we get everything back
			/*int processed = s16_decompress(output, orig, list_size);
			
			//printf("%d\t%d\t%d\n", list_size, output_size, processed);
			
			for (i = 0; i < list_size; ++i) {
				if (orig[i] != list1[i])
					printf("NOT SAME\n");
			}*/
		}

		free(list1);
		free(output);
	}
	
	fclose(fp100);
	
	printf("Total Size: Max %ld Bytes\n", total_size * 4);


	// MIN
	printf("Doing MIN....\n");
	
	total_size = 0;

	if (padded == 0)
		sprintf(buf, "../k2raster/data/%s_k%d_min_size.dat", data_file_name_without_path, k_val);
	else
		sprintf(buf, "../unpadded/data/unpadded_%s_k%d_min_size.dat", data_file_name_without_path, k_val);

	fp100 = fopen(buf, "r");

	for (k = 0; k < nz; ++k) {

		fread(&size, sizeof(uint32_t), 1, fp100);
		//printf("size: %d\n", size);
		list_size = size;
		
		list1 = calloc(list_size, sizeof(uint32_t));
		output = calloc(list_size, sizeof(uint32_t));
		
		if (padded == 0)
			sprintf(buf, "../k2raster/data/nodacs_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);
		else
			sprintf(buf, "../unpadded/data/unpadded_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);

		FILE *fp1 = fopen(buf, "r");
		if (fp1 != NULL) {
			fread(list1, list_size, sizeof(uint32_t), fp1);
			fclose(fp1);
			
			int32_t output_size = s16_compress(list1, output, size);
			
			total_size += output_size + output_size / 100;

			if (padded == 0)
				sprintf(buf, "./data_padded/simple16_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);
			else
				sprintf(buf, "./data_unpadded/simple16_%s_k%d_Min_%04d.dat", data_file_name_without_path, k_val, k);
			
			FILE *pf2 = fopen(buf, "w");
			fwrite(&list_size, sizeof(uint32_t), 1, pf2);
			fwrite(output, sizeof(uint32_t), output_size, pf2);
			fclose(pf2);
			
		}
		free(list1);
		free(output);
	}

	fclose(fp100);
	
	printf("Total Size: Min %ld Bytes\n", total_size * 4);

	return (EXIT_SUCCESS);
}