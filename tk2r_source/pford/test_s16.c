#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/limits.h>
#include "s16.h"

void test_s16()
{
	char     buf[PATH_MAX] = {0};
	uint32_t k = 0;
	uint32_t i = 0;
	size_t list_size = 12888; //357; //12888; // 357
	uint32_t *list = NULL;
	uint32_t size;
	
	char prog_name[PATH_MAX] = "airs_gran9.1501_135_90_2_0_16_0_0_0.raw";

	sprintf(buf, "../k2raster/data/%s_k6_max_size.dat", prog_name);
	
	FILE *fp100 = fopen(buf, "r");
	
	int32_t total_size = 0;
	
	for (k = 0; k < 1501; ++k) {
printf("A\n");
		fread(&size, sizeof(uint32_t), 1, fp100);
		list_size = size;
printf("B\n");		
		list = calloc(list_size, sizeof(uint32_t));
		uint32_t *output = calloc(list_size, sizeof(uint32_t));
		
		sprintf(buf, "../data/nodacs_%s_k6_Max_%03d.dat", prog_name, k);
		FILE *fp1 = fopen(buf, "r");
		fread(list, list_size, sizeof(uint32_t), fp1);
		fclose(fp1);
printf("C\n");		
		// for (i = 0; i < list_size; i++) {
			// printf("%d\t", list[i]);
		// }
		// printf("\n");
		
		int32_t output_size = s16_compress(list, output, size);
		
		total_size += output_size;
		
		//printf("output_size: %d\n", output_size);
		
		// sprintf(buf, "../data/simple9_%s_k6_Min_%03d.dat", prog_name, k);
		// FILE *fp2 = fopen(buf, "w");
		// simple9_encode(list, list_size, fp2);
		// fclose(fp2);

		// FILE *fp3 = fopen(buf, "r");
		// assert(fp3);
		// simple9_decode(&list, &list_size, fp3);
		// fclose(fp3);	
		free(list);
		free(output);
	}
	
	
	printf("total size: %d\n", total_size);
	
	fclose(fp100);
}

int main(int argc, char **argv)
{
	
	test_s16();
	
	
	return (EXIT_SUCCESS);
}