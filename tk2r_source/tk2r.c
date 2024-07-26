#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>   // gettimeofday()
#include <netinet/ip.h>
#include <sys/utsname.h>
#include <libgen.h>		// dirname()
#include <iostream>     // std::cout
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand

#include "./dacs/dacs.h"
#include "tk2r.h"
#include "utils.h"
#include "../simple16/simple16.h"
#include "../pford/pfordelta.h"
#include "../pford/coding_policy_helper.h"
#include "../pford/coding_policy.h"
#include "../pford/unpack.h"

#define FACT_RANK	20
#define WORD_SIZE	32

#define CF_DIFF(x, y) (cf[x] - cf[y])

#define CEILING_INT(NUMERATOR, DENOMINATOR)	((NUMERATOR + (DENOMINATOR - 1)) / DENOMINATOR)

#define NUM_OF_ITERATIONS	1000000

extern int32_t M[3][64];

uint32_t zigzag_encode(int32_t num)
{
	return (num >> 31) ^ (num << 1);
}

int32_t zigzag_decode(uint32_t num) // uint32_t
{
	return (num >> 1) ^ -(num & 1);
}

int get_base_name(char *input_path_name, char *output_base_name)
{
	char* local_file = input_path_name;

	char* ts = strdup(local_file);

	char* fname = basename(ts);
	if (fname == NULL)
		return -1;
	strcpy(output_base_name, fname);

	printf("File name: %s\n", output_base_name);
	
	free(ts);
	return 0;
}

int get_dir(char *dir)
{
	char pBuf[PATH_MAX];
	size_t len = sizeof(pBuf);
	char szTmp[32];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	int bytes = MIN((int)readlink(szTmp, pBuf, len), (int)(len - 1));
	if (bytes >= 0)
		pBuf[bytes] = '\0';
	
	
	printf("%s\n", pBuf);
	
	char* ts1 = strdup(pBuf);

	strcpy(dir, dirname(ts1));
	
	printf("dir: %s\n", dir);
	
	if (ts1 != NULL)
		free(ts1);
	
	return (EXIT_SUCCESS);
}

int file_exists(const char* fname)
{
	return access(fname, F_OK) != -1 ? 0 : -1;
}

void create_random_locations(uint32_t *nx_random,
                             uint32_t *ny_random,
							 uint32_t *nz_random,
							 uint32_t nx_old,
                             uint32_t ny_old,
							 uint32_t nz_old,
							 char     *data_file_without_path)
{
	//int i = 0;
	int j = 0;
	
	char buf[PATH_MAX] = {0};
	
	sprintf(buf, "random_%s.dat", data_file_without_path);

	srand(time(0));
	
	printf("nx_old: %d\n", nx_old);
	printf("ny_old: %d\n", ny_old);
	printf("nz_old: %d\n", nz_old);
	
	FILE *fp = fopen(buf, "w");
	
	for (j = 0; j < NUM_OF_ITERATIONS; ++j)
	{
		nx_random[j] = rand() % nx_old;
		ny_random[j] = rand() % ny_old;
		nz_random[j] = rand() % nz_old;
		fwrite(&nx_random[j], sizeof(uint32_t), 1, fp);
		fwrite(&ny_random[j], sizeof(uint32_t), 1, fp);
		fwrite(&nz_random[j], sizeof(uint32_t), 1, fp);
	}
	fclose(fp);
}


maxmin build_s(uint32_t	k_val,
              int32_t   *vec1,
              uint32_t  nx_new,
              uint32_t  n,
              uint32_t  l,
              uint32_t  r,
              uint32_t  c,
			  kdata     *pkdata,
			  uint32_t  band)
{
	maxmin ret;
	int32_t maxval;
	int32_t minval;
	
	if (pkdata->data_type == 2)
	{
		maxval = 0;
		minval = UINT16_MAX;
	}
	else
	{
		maxval = INT16_MIN;
		minval = INT16_MAX;
	}
	// int32_t	maxval = INT32_MIN;
	// int32_t	minval = INT32_MAX;	 

	int32_t	i  = 0;
	int32_t	j  = 0;
	int32_t k  = 0;
	int32_t	x  = 0;
	int32_t y  = 0;

	/////printf("******************************************************************************\n");
	
	int32_t counter_max_begin = pkdata->vmax_count[band][l];
	int32_t	counter_min_begin = pkdata->vmin_count[band][l];
	
	for (j = 0; j < (int32_t) k_val; ++j)
	{
		for (i = 0; i < (int32_t) k_val; ++i)
		{
			if (k_val == n) // last level
			{
				////printf("begin last level \n");
				y = r+j;
				x = c+i;
				////printf("1\tk_delta = %d\n", k_delta);

				if (minval > vec1[y * nx_new + x])
					minval = vec1[y * nx_new + x];
				if (maxval < vec1[y * nx_new + x])
					maxval = vec1[y * nx_new + x];
				pkdata->vmax[band][l][pkdata->vmax_count[band][l]] = vec1[y * nx_new + x];
				pkdata->vmax_count[band][l]++;		
				////printf("last level: l = %d\tchild.max_u16 = %d\n", l, vec1[y * nx_new + x]);				

				////printf("end last level \n");
			}
			else // in-between levels
			{
				////printf("begin internal node \n");
				////printf("internal\tk_delta = %d\n", k_delta);
				maxmin child = build_s(k_val, vec1, nx_new, n/k_val, l+1, r+j*(n/k_val), c+i*(n/k_val), pkdata, band);
				
				// k_delta == 0
				pkdata->vmax[band][l][pkdata->vmax_count[band][l]] = child.max;
				//pkdata->vmin[band][l][pkdata->vmin_count[band][l]] = child.min;
				//pkdata->vmin_count[band][l]++;
				////printf("in-between levels: l = %d\tchild.max_u16 = %d\tchild.min_u16 = %d\n", l, child.max, child.min);
				////printf("******************************************************************************\n");

				if (child.max != child.min)
				{
					pkdata->t[band][l][pkdata->vmax_count[band][l]] = 1;
					pkdata->vmin[band][l][pkdata->vmin_count[band][l]] = child.min;
					pkdata->vmin_count[band][l]++;
				}

				pkdata->vmax_count[band][l]++;

				if (maxval < child.max)
					maxval = child.max;	
				if (minval > child.min)
					minval = child.min;			
			
			}
		}
	}
	
	int32_t counter_max_end = pkdata->vmax_count[band][l];
	int32_t	counter_min_end = pkdata->vmin_count[band][l];

	if (minval == maxval)
	{
		pkdata->vmax_count[band][l] -= k_val*k_val;
		//pkdata->vmin_count[band][l] -= k_val*k_val;
	}

	for (k = counter_max_begin; k < counter_max_end; ++k)
		pkdata->vmax[band][l][k] = maxval - pkdata->vmax[band][l][k];
	
	for (k = counter_min_begin; k < counter_min_end; ++k)
		pkdata->vmin[band][l][k] = pkdata->vmin[band][l][k] - minval;

	ret.max = maxval;
	ret.min = minval;
	
	return ret;
}

child build_t(uint32_t k_val,
			  int32_t  *vec_s,
			  int32_t  *vec_t,
			  uint32_t new_size,
			  uint32_t n,
			  uint32_t l,
			  uint32_t r,
			  uint32_t c,
			  kdata    *pkdata,
			  uint32_t band)
{
	child		ret;
	int32_t		i;
	int32_t		j;

	int32_t		maxval_s;
	int32_t		maxval_t;
	int32_t		minval_s;	
	int32_t		minval_t;
	int32_t		diff;

	if (n == 1)
	{
		if ((r == 22 && c == 3) || (r == 23 && c == 2) || (r == 23 && c == 3))
		{
			printf("r=%d\tc=%d\tvec_t=%d\tvec_s=%d\n", r, c, vec_t[r * new_size + c], vec_s[r * new_size + c]);
			printf("A --> l = %d\tvmax_count = %d\tvec_t - vec_s = %d\n", l, pkdata->vmax_count[band][l], vec_t[r * new_size + c] - vec_s[r * new_size + c]);
		}
		pkdata->vmax[band][l][pkdata->vmax_count[band][l]] = vec_t[r * new_size + c] - vec_s[r * new_size + c];
		pkdata->vmax_count[band][l]++;
		
		ret.maxval_s = vec_s[r * new_size + c];
		ret.maxval_t = vec_t[r * new_size + c];
		ret.minval_s = vec_s[r * new_size + c];
		ret.minval_t = vec_t[r * new_size + c];
		ret.diff  = vec_t[r * new_size + c] - vec_s[r * new_size + c];

		return ret;
	}
	else
	{
		maxval_s = INT32_MIN;
		minval_s = INT32_MAX;
		maxval_t = INT32_MIN;
		minval_t = INT32_MAX;
		diff     = INT32_MIN;

		for (i = 0; i < (int32_t) k_val; i++)
		{
			for (j = 0; j < (int32_t) k_val; j++)
			{
				////printf("Enter build_t i = %d, j = %d\n", i, j);
				ret = build_t(k_val,
								vec_s,
								vec_t,
								new_size,
								n / k_val,
								l + 1,
								r + i * (n / k_val),
								c + j * (n / k_val),
								pkdata,
								band);
				////printf("Leave build_t\n");
				
				if (diff == INT32_MIN)
					diff = ret.diff;
				else if (diff != ret.diff)
					diff = INT32_MAX;
				
				if (ret.maxval_s > maxval_s)
					maxval_s = ret.maxval_s;
				if (ret.minval_s < minval_s)
					minval_s = ret.minval_s;
				if (ret.maxval_t > maxval_t)
					maxval_t = ret.maxval_t;
				if (ret.minval_t < minval_t)
					minval_t = ret.minval_t;
				////printf("For loop end\n");
			}
		}
		
		if (maxval_t == minval_t) // all values are equal
		{
			////printf("inside maxval_t == minval_t\n");
			for (i = 0; i < (int32_t) (k_val * k_val); i++)
			{
				// Lmax[l+1].pop
				pkdata->vmax_count[band][l+1]--;
				if (n > k_val)
				{
					pkdata->t_count[band][l+1]--; // T_t[l+1].pop 
					pkdata->eqB_count[band][l+1]--; // eqB[l+1].pop
				}
			}
			
			pkdata->t[band][l][pkdata->t_count[band][l]] = 0; //T_t[l].push(0);
			pkdata->t_count[band][l]++;
			
			pkdata->eqB[band][l][pkdata->eqB_count[band][l]] = 0; // eqB[l].push(0);
			pkdata->eqB_count[band][l]++;
			
			pkdata->vmax[band][l][pkdata->vmax_count[band][l]] = maxval_t - maxval_s; //Lmax_t[l].push(maxval_t - maxval_s);
			pkdata->vmax_count[band][l]++;
			////printf("B --> l = %d\tvmax_count = %d\tvec_t - vec_s = %d\n", l, pkdata->vmax_count[band][l], maxval_t - maxval_s);
		}
		else if (diff != INT32_MAX) // quadbox with same structure as that of snapshot
		{
			////printf("inside diff != INT32_MAX\n");
			for (i = 0; i < (int32_t) (k_val * k_val); i++)
			{	
				pkdata->vmax_count[band][l+1]--; // Lmax[l+1].pop
				if (n > k_val)
				{
					pkdata->t_count[band][l+1]--; // T_t[l+1].pop
					pkdata->eqB_count[band][l+1]--; // eqB[l+1].pop
				}
			}			
			pkdata->t[band][l][pkdata->t_count[band][l]] = 0; // T_t[l].push(0);
			pkdata->t_count[band][l]++;
			
			pkdata->eqB[band][l][pkdata->eqB_count[band][l]] = 1; // eqB[l].push(1);
			pkdata->eqB_count[band][l]++;
			
			pkdata->vmax[band][l][pkdata->vmax_count[band][l]] = maxval_t - maxval_s; // Lmax_t[l].push(maxval_t - maxval_s);
			pkdata->vmax_count[band][l]++;
			////printf("C --> l = %d\tvmax_count = %d\tvec_t - vec_s = %d\n", l, pkdata->vmax_count[band][l], maxval_t - maxval_s);
		}
		else // node is not a leaf
		{
			////printf("inside else 1\t%d\t%d\n", pkdata->t_count[band][l], pkdata->t[band][l][pkdata->t_count[band][l]]);
			pkdata->t[band][l][pkdata->t_count[band][l]] = 1; // T_t[l].push(1);
			pkdata->t_count[band][l]++;
			
			////printf("inside else 2\n");
			////printf("D --> l = %d\tvmax_count = %d\tvec_t - vec_s = %d\n", l, pkdata->vmax_count[band][l], maxval_t - maxval_s);
			pkdata->vmax[band][l][pkdata->vmax_count[band][l]] = maxval_t - maxval_s; // Lmax_t[l].push(maxval_t - maxval_s);
			pkdata->vmax_count[band][l]++;
			
			////printf("inside else 3\n");
			pkdata->vmin[band][l][pkdata->vmin_count[band][l]] = minval_t - minval_s; // Lmin_t[l].push(minval_t - maxval_s);
			pkdata->vmin_count[band][l]++;			
		}
		ret.maxval_s = maxval_s;
		ret.maxval_t = maxval_t;
		ret.minval_s = minval_s;
		ret.minval_t = minval_t;
		ret.diff     = diff;
		return ret;
	}
}

int initialize(kdata *pkdata)
{
	uint32_t	i   = 0;
	uint32_t	j   = 0;
	uint32_t	k   = 0;
	uint32_t	num = 1;
	uint32_t    T_size = 0;
	
	//delta *= 2;
	uint32_t    delta_local = pkdata->tau + 1;

	if (pkdata->td <= 0)
	{
		printf("Tree depth cannot be 0 or less than 0.\n");
		return (EXIT_FAILURE);
	}

	//*tree_depth = td;
	printf("Tree Depth: %d (Tree Height: %d)\n", pkdata->td, pkdata->td - 1);
	
	num = 1;
	for (i = 0; i < (uint32_t)pkdata->td; ++i)
	{
		if (i != 0 && i != (uint32_t)(pkdata->td - 1))
			T_size += num;
		pkdata->num_elements += num;
		num *= pkdata->k_val * pkdata->k_val;
	}

	pkdata->t_size_packed = (uint32_t)CEILING_INT(T_size, WORD_SIZE);
	printf("k_val:\t%d\n", pkdata->k_val);
	printf("pkdata->t_size_packed: %d\n", pkdata->t_size_packed);
	printf("num_elements: %d\n", pkdata->num_elements);
	
	// =====> initialize vmax, vmin, t, eqB
	if ((pkdata->vmax = (int32_t ***) calloc(delta_local, sizeof(int32_t **))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->vmin = (int32_t ***) calloc(delta_local, sizeof(int32_t **))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->t = (uint8_t ***) calloc(delta_local, sizeof(uint8_t **))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->eqB = (uint8_t ***) calloc(delta_local, sizeof(uint8_t **))) == NULL)
		return (EXIT_FAILURE);	
	
	for (j = 0; j < delta_local; j++)
	{
		if ((pkdata->vmax[j] = (int32_t **)calloc(pkdata->td, sizeof(int32_t *))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->vmin[j] = (int32_t **)calloc(pkdata->td, sizeof(int32_t *))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->t[j] = (uint8_t **)calloc(pkdata->td, sizeof(uint8_t *))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->eqB[j] = (uint8_t **)calloc(pkdata->td, sizeof(uint8_t *))) == NULL)
			return (EXIT_FAILURE);
		
		num = 1;
		for (i = 0; i < (uint32_t)pkdata->td; i++)
		{
			if ((pkdata->vmax[j][i] = (int32_t *) calloc(num, sizeof(int32_t))) == NULL)
				return (EXIT_FAILURE);
			if ((pkdata->vmin[j][i] = (int32_t *) calloc(num, sizeof(int32_t))) == NULL)
				return (EXIT_FAILURE);
			if ((pkdata->t[j][i] = (uint8_t *) calloc(num, sizeof(uint8_t))) == NULL)
				return (EXIT_FAILURE);
			if ((pkdata->eqB[j][i] = (uint8_t *) calloc(num, sizeof(uint8_t))) == NULL)
				return (EXIT_FAILURE);
			num *= pkdata->k_val * pkdata->k_val;
		}
	}

	// =====> initialize lmax, lmin, lmax_final, lmin_final, lmax_temp, lmin_temp, T_final, t_size_final
	if ((pkdata->lmax = (int32_t **) calloc(delta_local, sizeof(int32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->lmin = (int32_t **) calloc(delta_local, sizeof(int32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->lmax_final = (int32_t **) calloc(pkdata->nz, sizeof(int32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->lmin_final = (int32_t **) calloc(pkdata->nz, sizeof(int32_t *))) == NULL)
		return (EXIT_FAILURE);	
	if ((pkdata->lmax_len_final = (uint32_t *)calloc(pkdata->nz, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->lmin_len_final = (uint32_t *)calloc(pkdata->nz, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);

	for (j = 0; j < delta_local; j++)
	{
		if ((pkdata->lmax[j] = (int32_t *)calloc(pkdata->num_elements, sizeof(int32_t))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->lmin[j] = (int32_t *)calloc(pkdata->num_elements, sizeof(int32_t))) == NULL)
			return (EXIT_FAILURE);
	}
	for (j = 0; j < pkdata->nz; j++)
	{
		if ((pkdata->lmax_final[j] = (int32_t *)calloc(pkdata->num_elements, sizeof(int32_t))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->lmin_final[j] = (int32_t *)calloc(pkdata->num_elements, sizeof(int32_t))) == NULL)
			return (EXIT_FAILURE);
	}
	
	for (j = 0; j < 2; j++)
	{
		if ((pkdata->lmax_temp[j] = (int32_t *)calloc(pkdata->num_elements, sizeof(int32_t))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->lmin_temp[j] = (int32_t *)calloc(pkdata->num_elements, sizeof(int32_t))) == NULL)
			return (EXIT_FAILURE);	
		if ((pkdata->T_temp[j] = (uint32_t *)calloc(pkdata->t_size_packed, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);	
	}

	// =====> initialize vmax_count, vmin_count, t_count, eqB_count
	if ((pkdata->vmax_count = (uint32_t **) calloc(delta_local, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->vmin_count = (uint32_t **) calloc(delta_local, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->t_count = (uint32_t **) calloc(delta_local, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->eqB_count = (uint32_t **) calloc(delta_local, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	
	for (j = 0; j < delta_local; j++)
	{
		if ((pkdata->vmax_count[j] = (uint32_t *)calloc(pkdata->td, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->vmin_count[j] = (uint32_t *)calloc(pkdata->td, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->t_count[j] = (uint32_t *)calloc(pkdata->td, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);
		if ((pkdata->eqB_count[j] = (uint32_t *)calloc(pkdata->td, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);
	}
	
	// =====> initialize T, T_final, T_temp
	if ((pkdata->T_packed = (uint32_t **)calloc(delta_local, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->eqB_packed = (uint32_t **)calloc(delta_local, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->T_packed_final = (uint32_t **)calloc(pkdata->nz, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->eqB_packed_final = (uint32_t **)calloc(pkdata->nz, sizeof(uint32_t *))) == NULL)
		return (EXIT_FAILURE);	
	
	for (k = 0; k < delta_local; ++k)
	{
		if ((pkdata->T_packed[k] = (uint32_t *)calloc(pkdata->t_size_packed, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);	
		if ((pkdata->eqB_packed[k] = (uint32_t *)calloc(pkdata->t_size_packed, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);
	}
	for (k = 0; k < pkdata->nz; ++k)
	{
		if ((pkdata->T_packed_final[k] = (uint32_t *)calloc(pkdata->t_size_packed, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);	
		if ((pkdata->eqB_packed_final[k] = (uint32_t *)calloc(pkdata->t_size_packed, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);	
	}
	for (j = 0; j < 2; j++)
	{
		if ((pkdata->T_temp[j] = (uint32_t *)calloc(pkdata->t_size_packed, sizeof(uint32_t))) == NULL)
			return (EXIT_FAILURE);	
	}

	// =====> initialize rMaxMin, t_size, lmax_len, lmin_len
	if ((pkdata->rMaxMin = (maxmin *)calloc(pkdata->nz, sizeof(maxmin))) == NULL)
		return (EXIT_FAILURE);	
	if ((pkdata->t_size = (uint32_t *)calloc(delta_local, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->t_size_final = (uint32_t *)calloc(pkdata->nz, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);	
	if ((pkdata->eqB_counter_final = (uint32_t *)calloc(pkdata->nz, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);	
	if ((pkdata->lmax_len = (uint32_t *)calloc(delta_local, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->lmin_len = (uint32_t *)calloc(delta_local, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->T_counter = (uint32_t *)calloc(delta_local, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->eqB_counter = (uint32_t *)calloc(delta_local, sizeof(uint32_t))) == NULL)
		return (EXIT_FAILURE);
	if ((pkdata->sB = (sB_data *)calloc(pkdata->nz, sizeof(sB_data))) == NULL)
		return (EXIT_FAILURE);
	
	if ((pkdata->reordered_bands = (reorder *)calloc(pkdata->nz, sizeof(reorder))) == NULL)
		return (EXIT_FAILURE);

	if ((pkdata->average_array = (int32_t *)calloc(pkdata->nz, sizeof(int32_t))) == NULL)
		return (EXIT_FAILURE);

	return (EXIT_SUCCESS);
}

int clear_data(kdata    *pkdata,
			   int32_t  band)
{
	int32_t	i   = 0;
	int32_t	j   = 0;
	int32_t	k   = 0;
	uint32_t	num = 1;

	if (pkdata->td <= 0)
	{
		printf("Tree depth cannot be 0 or less than 0.\n");
		return (EXIT_FAILURE);
	}

	// =====> initialize vmax, vmin, tl
	
	uint32_t begin;
	uint32_t end;
	if (band == -1)
	{
		begin = 0;
		end = pkdata->tau + 1;
	}
	else
	{
		begin = band;
		end = band + 1;
	}
	
	for (k = begin; k < (int32_t) end; k++)
	{
		num = 1;

		for (j = 0; j < (int32_t) pkdata->t_size_packed; j++)
		{
			pkdata->T_packed[k][j] = 0;
			pkdata->eqB_packed[k][j] = 0;
		}		
		
		for (j = 0; j < (int32_t) pkdata->td; j++)
		{
			pkdata->vmax_count[k][j] = 0;
			pkdata->vmin_count[k][j] = 0;
			pkdata->t_count[k][j] = 0;
			pkdata->eqB_count[k][j] = 0;
			
			for (i = 0; i < (int32_t) num; i++)
			{
				pkdata->vmax[k][j][i] = 0;
				pkdata->vmin[k][j][i] = 0;
				pkdata->t[k][j][i]    = 0;
				pkdata->eqB[k][j][i]  = 0;

			}
			num *= pkdata->k_val * pkdata->k_val;
		}
		
		for (j = 0; j < (int32_t) pkdata->num_elements; j++)
		{
			pkdata->lmax[k][j] = 0;
			pkdata->lmin[k][j] = 0;
		}
		
		pkdata->t_size[k] = 0;
		
		pkdata->lmax_len[k] = 0;
		pkdata->lmin_len[k] = 0;
		pkdata->T_counter[k] = 0;
		pkdata->eqB_counter[k] = 0;
		//pkdata->rMaxMin[k].max_u16 = 0;
		//pkdata->rMaxMin[k].min_u16 = 0;
		
		// for (j = 0; j < 2; j++)
		// {
			// for (i = 0; i < (int32_t)pkdata->t_size_packed; i++)
			// {
				// pkdata->T_temp[j][i] = 0;
			// }
			// for (i = 0; i < (int32_t)pkdata->num_elements; i++)
			// {
				// pkdata->lmax_temp[j][i] = 0;
				// pkdata->lmin_temp[j][i] = 0;				
			// }
			// pkdata->lmax_len_temp[j] = 0;
			// pkdata->lmin_len_temp[j] = 0;
			// pkdata->t_size_temp[j] = 0;
		// }
		
		pkdata->eqB_counter[k] = 0;
		for (k = 0; k < (int32_t)pkdata->tau; ++k)
		{
			for (j = 0; j < (int32_t)pkdata->t_size_packed; j++)
				pkdata->eqB_packed[k][j] = 0;
		}
		
	}

	return (EXIT_SUCCESS);
}

void free_memory(kdata *pkdata)
{
	uint32_t i;
	uint32_t j;
	uint32_t k;
	
	pkdata->tau++;
	
	// vmax, vmin, lmax, lmin, vmax_count, vmin_count
	for (j = 0; j < pkdata->tau; j++)
	{
		for (i = 0; i < (uint32_t) pkdata->td; i++)
		{
			free(pkdata->vmax[j][i]);
			free(pkdata->vmin[j][i]);
			free(pkdata->t[j][i]);
			free(pkdata->eqB[j][i]);
		}
		free(pkdata->vmax[j]);
		free(pkdata->vmin[j]);
		free(pkdata->lmax[j]);
		free(pkdata->lmin[j]);
		free(pkdata->vmax_count[j]);
		free(pkdata->vmin_count[j]);
		free(pkdata->t[j]);
		free(pkdata->eqB[j]);
	}
	
	free(pkdata->vmax);
	free(pkdata->vmin);
	free(pkdata->lmax);
	free(pkdata->lmin);
	free(pkdata->vmax_count);
	free(pkdata->vmin_count);
	free(pkdata->t);
	free(pkdata->eqB);	
	
	// free T, rMaxMin
	for (k = 0; k < pkdata->tau; k++)
	{
		if (pkdata->T_packed[k] != NULL)
			free(pkdata->T_packed[k]);
	}
	
	if (pkdata->T_packed != NULL)
		free(pkdata->T_packed);
	if (pkdata->rMaxMin != NULL)
		free(pkdata->rMaxMin);
	if (pkdata->sB != NULL)
		free(pkdata->sB);
	if (pkdata->lmax_len != NULL)
		free(pkdata->lmax_len);	
	if (pkdata->lmin_len != NULL)
		free(pkdata->lmin_len);	
	if (pkdata->T_counter != NULL)
		free(pkdata->T_counter);	
	if (pkdata->eqB_counter != NULL)
		free(pkdata->eqB_counter);
	
	for (i = 0; i < 2; i++)
	{
		if (pkdata->lmax_temp[i] != NULL)
			free(pkdata->lmax_temp[i]);		
		if (pkdata->lmin_temp[i] != NULL)
			free(pkdata->lmin_temp[i]);	
		if (pkdata->T_temp[i] != NULL)
			free(pkdata->T_temp[i]);	
	}
	
	for (j = 0; j < pkdata->nz; j++)
	{
		if (pkdata->lmax_final[j] != NULL)
			free(pkdata->lmax_final[j]);
		if (pkdata->lmin_final[j] != NULL)
			free(pkdata->lmin_final[j]);
		if (pkdata->T_packed_final[j] != NULL)
			free(pkdata->T_packed_final[j]);
		if (pkdata->eqB_packed_final[j] != NULL)
			free(pkdata->eqB_packed_final[j]);
	}
	
	free(pkdata->lmax_final);
	free(pkdata->lmin_final);
	free(pkdata->lmax_len_final);
	free(pkdata->lmin_len_final);
	free(pkdata->T_packed_final);
	free(pkdata->eqB_packed_final);
	free(pkdata->t_size_final);
	free(pkdata->eqB_counter_final);
	free(pkdata->reordered_bands);
}

int32_t all_zeros(int32_t *v, uint32_t v_len)
{
	if (v_len == 0)
		return 1;

	int32_t i;

	for (i = 0; i < (int32_t)v_len; i++)
	{
		if (v[i] != 0)
		{
			return 0;
		}
	}

	return 1;
}

int64_t get_file_size(const char *fname)
{
	int64_t fsize = -1L;
	FILE *fp = fopen(fname, "rb");

    if (fp)
	{
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);
		rewind(fp);
		fclose(fp);
	}
	
	return fsize;
}

uint32_t get_rank_zero(const uint32_t *list, uint32_t pos)
{
	uint32_t count = 0;
	uint32_t i;

	if (pos + 1 == 0)
		return 0;

	++pos;

	for (i = 0; i < (pos >> 5); i++) // pos >> 5 == pos / 32
		count += 32 - __builtin_popcount(list[i]);

	if ((pos & MASK_31) != 0) // pos & MASK_31 == pos % 32
		count += (pos & MASK_31) - __builtin_popcount(list[pos >> 5] & ((1 << (pos & MASK_31)) - 1));

	return count;
}

uint32_t get_rank_one(const uint32_t *list, uint32_t pos)
{
	uint32_t count = 0;
	uint32_t i;

	if (pos + 1 == 0)
		return 0;

	++pos;

	for (i = 0; i < (pos >> 5); i++) // pos >> 5 == pos / 32
		count += __builtin_popcount(list[i]);

	if ((pos & MASK_31) != 0) // pos & MASK_31 == pos % 32
		count += __builtin_popcount(list[pos >> 5] & ((1 << (pos & MASK_31)) - 1));

	return count;
}

int32_t get_cell_s(kdata     *kd,
                   uint32_t  encoder_type, 
                   uint32_t  *T_packed,
				   void      *compressed_array,
				   //int32_t   *Lmax_packed,
                   int32_t   T_unpacked_size,
				   uint32_t  n,
				   uint32_t  r,
				   uint32_t  c,
				   int32_t   z,
				   int32_t   max_val,
				   int8_t    print1,
				   uint32_t  band
				   )
{
	z = (get_rank_one(T_packed, z) - 1) * kd->k_val * kd->k_val + 1;
	
	z = z + r / (n / kd->k_val) * kd->k_val + c / (n / kd->k_val);
	
	int32_t val = 0;

	if (encoder_type == UNCOMPRESSED)
	{
		val = ((int32_t *)compressed_array)[z];
	}
	else if (encoder_type == DACS)
	{
		val = accessFT((FTRep *)compressed_array, z);
	}

	if (print1)
		printf("\tz = %d\tr = %d\tc = %d\tval = %d\tmax_val = %d\tmax_val - val = %d\n", z, r, c, val, max_val, max_val - val);
	
	max_val -= val;
	
	if (z >= (int32_t)T_unpacked_size || bitget(T_packed, z) == 0)
		return max_val;
	else 
		return get_cell_s(kd,
	                      encoder_type,
						  T_packed,
						  compressed_array,
						  T_unpacked_size,
						  n / kd->k_val,
						  r % (n / kd->k_val),
						  c % (n / kd->k_val),
						  z,
						  max_val,
						  print1,
				          band);
}

int32_t get_cell_t(kdata     *kd,
                   uint32_t  encoder_type, 
				   uint32_t  k_val,
                   uint32_t  *T_s,
				   uint32_t  *T_t,
				   uint32_t  *eqB,
				   void      *Lmax_s,
				   void      *Lmax_t,
				   // int32_t   *Lmax_s,
				   // int32_t   *Lmax_t,
                   int32_t   T_size_s,
				   int32_t   T_size_t,
				   uint32_t  n,
				   uint32_t  r,
				   uint32_t  c,
				   int32_t   z_s,
				   int32_t   z_t,
				   int32_t   max_val_s,
				   int32_t   max_val_t,
				   int8_t    print1,
				   uint32_t  band)
{
	int32_t new_n = n / k_val;
	
	if (z_s != -1)
	{
		z_s  = (get_rank_one(T_s, z_s) - 1) * k_val * k_val + 1;
		
		z_s += r / new_n * k_val + c / new_n; //z_s += floor((double)r / (double)new_n) * k_val + floor((double)c / (double)new_n);
		
		if (encoder_type == UNCOMPRESSED)
		{
			max_val_s -= ((int32_t *)Lmax_s)[z_s];
		}
		else if (encoder_type == DACS)
		{
			max_val_s -= accessFT((FTRep *)Lmax_s, z_s);
		}		
		if (print1)
			printf("A%d\tz_s = %d\n", band, z_s);
	}
	
	if (z_t != -1)
	{
		z_t  = (get_rank_one(T_t, z_t) - 1) * k_val * k_val + 1;
		
		z_t += r / new_n * k_val + c / new_n; //z_t += floor((double)r / (double)new_n) * k_val + floor((double)c / (double)new_n);

		if (encoder_type == UNCOMPRESSED)
		{
			max_val_t = ((int32_t *)Lmax_t)[z_t];
		}
		else if (encoder_type == DACS)
		{
			max_val_t = accessFT((FTRep *)Lmax_t, z_t);
		}		
		if (print1)
			printf("B%d\tz_t = %d\n", band, z_t);
	}
	
	if ((z_s > T_size_s || z_s == -1 || bitget(T_s, z_s) == 0) &&
	    (z_t > T_size_t || z_t == -1 || bitget(T_t, z_t) == 0)) // Both are leaves
	{
		if (print1)
			printf("C%d\n", band);
		
		return max_val_s + zigzag_decode(max_val_t);
	}
	else if (z_s > T_size_s || z_s == -1 || bitget(T_s, z_s) == 0) // Leaf in snapshot
	{ 
		if (print1)
			printf("D%d\tz_s = %d\n", band, z_s);
		
		z_s = -1;
		
		return get_cell_t(kd, encoder_type, k_val, T_s, T_t, eqB, Lmax_s, Lmax_t, T_size_s, T_size_t,
		                new_n, r % new_n, c % new_n, z_s, z_t, max_val_s, max_val_t, print1, band);
	}
	else if (z_t > T_size_t || z_t == -1 || bitget(T_t, z_t) == 0) // Leaf in time instant
	{
		if (print1)
			printf("E%d\tz_t = %d\n", band, z_t);
		if (z_t != -1 && bitget(T_t, z_t) == 0)
		{
			int8_t eq = bitget(eqB, z_t + 1 - get_rank_one(T_t, z_t) - 1);
			if (print1)
				printf("E%d\tz_t = %d\teq-index = %d\teq = %d\tbiget(T_t, z_t) = %d\n", band, z_t, z_t + 1 - get_rank_one(T_t, z_t), eq, bitget(T_t, z_t));	
			if (eq == 1)
			{
				z_t = -1;
			}
			else
			{
				if (print1)
					printf("E%d\tz_t = %d\teq = %d\tmax_val_s = %d\tzigzag_decode = %d\n", band, z_t, eq, max_val_s, zigzag_decode(max_val_t));
				return max_val_s + zigzag_decode(max_val_t);
			}
		}
		
		return get_cell_t(kd, encoder_type, k_val, T_s, T_t, eqB, Lmax_s, Lmax_t, T_size_s, T_size_t,
		                new_n, r % new_n, c % new_n, z_s, z_t, max_val_s, max_val_t, print1, band);		
	}
	else // Both are internal nodes
	{
		if (print1)
			printf("F%d\n", band);
		
		return get_cell_t(kd, encoder_type, k_val, T_s, T_t, eqB, Lmax_s, Lmax_t, T_size_s, T_size_t,
		                new_n, r % new_n, c % new_n, z_s, z_t, max_val_s, max_val_t, print1, band);
	}
}

int access_data(kdata     *pkdata,
                uint32_t  r,
		        uint32_t  c,
		        uint32_t  band,
				int32_t   k)
{
	int32_t val = 0; 

	int print1 = (k == 30 || k == 31) && (
											// (r == 0 && c == 0) || 
											// (r == 15 && c == 23) ||
                                            // (r == 23 && c == 23) ||	
                                          (r == 22 && c == 2) ||											
	                                      (r == 22 && c == 3) || 
							              (r == 23 && c == 2) ||
					                      (r == 23 && c == 3)
										  ) ? 1 : 0;

	if (band % pkdata->tau == 0)
	{
		// if (band == 2)
			// printf("Calling get_cell_s\tpkdata->delta = %d\n", pkdata->delta);
		//printf("get_cell_s: %d\n", pkdata->t_size[band]);
		val = get_cell_s(pkdata,
		                 UNCOMPRESSED,
						 pkdata->T_packed[band],
						 pkdata->lmax[band],
						 pkdata->t_size[band],
						 pkdata->nx_new,
						 r,
						 c,
						 0,
						 pkdata->lmax[band][0],
						 print1,
						 band);
	}
	else
	{
		// printf("\tk = %d\tpkdata->t_size[0] = %d\tpkdata->t_size[%d] = %d\n",
		       // k, pkdata->t_size[0], band, pkdata->t_size[band]);
		
		val = get_cell_t(pkdata,
		                 UNCOMPRESSED,
		                 pkdata->k_val,
						 pkdata->T_packed[0],
						 pkdata->T_packed[band],
						 pkdata->eqB_packed[band],
						 pkdata->lmax[0],
						 pkdata->lmax[band],
						 pkdata->t_size[0],
						 pkdata->t_size[band],
						 pkdata->nx_new,
						 r,
						 c,
						 0, // z_s
						 0, // z_t
		                 pkdata->lmax[0][0],
						 pkdata->lmax[band][0],
						 print1,
						 band);
	}
	
	return val;
}

int32_t get_prev_snapshot(sB_data *sB, int32_t curr_pos)
{
	int32_t prev_pos = -1;
	int32_t i;
	
	for (i = curr_pos - 1; i >= 0; i--)
	{
		if (sB[i].snapshot == 1)
		{
			return i;
		}
	}

	return prev_pos;
}

int32_t get_prev_snapshot_random(sB_data               *sB, 
								 std::vector<uint16_t> &my_vector, 
								 uint16_t              nz,
								 int32_t               curr_pos,
								 int8_t                prev_index)
{
	int32_t i;	
	
	int32_t found = 0;
	int32_t found_index = -1;
	
	for (i = 0; i < nz; i++)
	{
		if (my_vector[i] == curr_pos)
		{
			found = 1;
			found_index = i;
			break;
		}
	}
	
	if (found)
	{
		if (prev_index)
		{
			return my_vector[found_index - 1];
		}
		else
		{		
			for (i = found_index - 1; i >= 0; i--)
			{
				if (sB[my_vector[i]].snapshot == 1)
				//if (sB_snapshot == 1)
				{
					return my_vector[i];
				}
			}
		}
	}

	return -1;
}

int32_t print_help_message()
{
	fprintf(stderr, "Required parameters missing.\n");
	fprintf(stderr, "Usage: ./tk2r [data file] [nz] [ny] [nx] [unsigned/signed] [k_val] [delta] [get_cell_checking] [build_type] [reorder_type]\n");
	fprintf(stderr, "       [nz]                : number of bands.\n");
	fprintf(stderr, "       [ny]                : height of matrix.\n");
	fprintf(stderr, "       [nx]                : width of matrix.\n");
	fprintf(stderr, "       [unsigned/signed]   : 2 for unsigned integer (uncalilbrated),\n");
	fprintf(stderr, "                             3 for signed integer (calibrated).\n");
	fprintf(stderr, "       [k_val]             : k value where k >= 2.\n");
	fprintf(stderr, "       [delta]             : delta value for regular-grouping T-k2raster build (1 for k2raster build).\n");
	fprintf(stderr, "       [get_cell_checking] : 0 for no checking,\n");
	fprintf(stderr, "                             1 for checking\n");
	fprintf(stderr, "       [build_type]        : 0 for T-k2raster-heuristic regular-grouping build,\n");
	fprintf(stderr, "                             1 for T-k2raster-heuristic irregular-grouping build.\n");
	fprintf(stderr, "       [reorder_type]      : 0 for bands ordered in the original order,\n");
	fprintf(stderr, "                             1 for bands reordered randomly,\n");
	fprintf(stderr, "                             2 for bands reordered based on average data,\n");
	fprintf(stderr, "                             3 for bands reordered based on entropy,\n");
	fprintf(stderr, "                             4 for bands reordered based on maximum data,\n");
	fprintf(stderr, "                             5 for bands reordered based on minimum data,\n");
	fprintf(stderr, "                             6 for bands reordered based on the difference between max and min data,\n");	
	fprintf(stderr, "                             7 for bands reordered based on percentage of non-zero data,\n");
	fprintf(stderr, "                             8 for bands reordered based on percentage of zero data,\n");
	fprintf(stderr, "                             Default is 0 (in original order).\n");
	
	fprintf(stderr, "\nA program for building a T-k2raster compact data structure. (Build %s %s)\n", __DATE__, __TIME__);
	// // fprintf(stderr, "Optimal k values - \n");
	// // fprintf(stderr, "airs:\t\t\t6\n");
	// // fprintf(stderr, "aviris_calibrated:\t6\n");
	// // fprintf(stderr, "aviris_uncalibrated:\t9\n");
	// // fprintf(stderr, "crism:\t\t\t5, 6\n");
	// // fprintf(stderr, "hyperion_calibrated:\t8\n");
	// // fprintf(stderr, "hyperion_uncalibrated:\t8\n");
	return (EXIT_FAILURE);
}

int32_t build_snapshot_raster(kdata *pkdata, int32_t *vector, uint32_t element, uint32_t band)
{
	int32_t i;
	int32_t j;
	char	buf[PATH_MAX] = "";
	FTRep 	*dacMax = NULL;
	FTRep 	*dacMin = NULL;

	clear_data(pkdata, element);
	pkdata->rMaxMin[element] = build_s(pkdata->k_val, vector, pkdata->nx_new, pkdata->nx_new, 1, 0, 0, pkdata, element);				

	printf("rMaxMin.max = %d\trMaxMin.min = %d\t\n", pkdata->rMaxMin[element].max, pkdata->rMaxMin[element].min);

	// Stuffing the t array elements into T
	pkdata->T_packed[element][0] = 1;
	pkdata->T_counter[element] = 1;
	for (j = 1; j < (int32_t)(pkdata->td - 1); ++j) 
	{
		for (i = 0; i < (int32_t)pkdata->vmax_count[element][j]; i++)
		{
			// // printf("s: pkdata->t[%d][%d][%d] = %d\n", element, j, i, pkdata->t[element][j][i]);
			pkdata->T_packed[element][pkdata->T_counter[element] / WORD_SIZE] |= (pkdata->t[element][j][i] << (pkdata->T_counter[element] % WORD_SIZE));
			pkdata->T_counter[element]++;
		}
	}

	// printf("*** Num: %d\n", pkdata->T_counter[element]);
	// for (j = 0; j < (int32_t)CEILING_INT(pkdata->T_counter[element], WORD_SIZE); ++j)
	// {
		// printf("*** %d\t0x%08X\n", j, pkdata->T_packed[element][j]);
	// }


	pkdata->vmax[element][0][0] = pkdata->rMaxMin[element].max;
	pkdata->vmin[element][0][0] = pkdata->rMaxMin[element].min;

	pkdata->vmax_count[element][0] = 1;
	pkdata->vmin_count[element][0] = 1;
	pkdata->t[element][0][0]       = 1;

	//////printf("lmax array:\n");
	pkdata->lmax_len[element] = 0;
	for (i = 0; i < (int32_t) pkdata->td; ++i)
	{
		for (j = 0; j < (int32_t) pkdata->vmax_count[element][i]; j++)
		{
			pkdata->lmax_temp[0][pkdata->lmax_len[element]] = pkdata->lmax[element][pkdata->lmax_len[element]] = pkdata->vmax[element][i][j];
			pkdata->lmax_len[element]++;
			
			// printf("level = %d\tcount = %d\t%d\n", i, j, pkdata->vmax[0][i][j]);
		}
	}
	pkdata->lmax_len_temp[0] = pkdata->lmax_len[element];
	

	//////printf("lmin array:\n");
	pkdata->lmin_len[element] = 0;
	for (i = 0; i < (int32_t) pkdata->td; ++i)
	{
		for (j = 0; j < (int32_t) pkdata->vmin_count[element][i]; j++)
		{
			pkdata->lmin_temp[0][pkdata->lmin_len[element]] = pkdata->lmin[element][pkdata->lmin_len[element]] = pkdata->vmin[element][i][j];
			pkdata->lmin_len[element]++;
			// printf("level = %d\tcount = %d\t%d\n", i, j, pkdata->vmin[0][i][j]);
		}
	}
	pkdata->lmin_len_temp[0] = pkdata->lmin_len[element];	

	pkdata->t_size[element] = 0;
	for (i = 0; i < (int32_t) (pkdata->td - 1); i++)
		pkdata->t_size[element] += pkdata->vmax_count[element][i];
	
	pkdata->t_size_temp[0] = pkdata->t_size[element];
	
	//pkdata->T_counter[element] = (uint16_t) ceil((double) pkdata->T_counter[element] / (double) WORD_SIZE);
	pkdata->T_counter[element] = (uint32_t) CEILING_INT(pkdata->T_counter[element], WORD_SIZE);
	printf("pkdata->T_counter[element] = %d, t_size[%d] = %d\n", pkdata->T_counter[element], pkdata->k_val, pkdata->t_size[element]);

	for (i = 0; i < (int32_t)pkdata->T_counter[element]; i++)
	{
		pkdata->T_temp[0][i] = pkdata->T_packed[element][i];
	}

	printf("Saving all the files \n");	

	// MAX
	sprintf(buf, "./data5/k%d_Max_%04d.dat", pkdata->k_val, band);
	if (all_zeros(pkdata->lmax[element], pkdata->lmax_len[element]))
	{
		printf("HAHAHA...ALL ZEROS MAX: k = %d\n", band);
		FILE * fp3 = fopen(buf, "w");
		fclose(fp3);
		pkdata->sB_test[9].lmax_byte_size = 0;
	}
	else
	{
		if (pkdata->encoder_type == DACS)
		{
			dacMax = createFT((uint32_t *)pkdata->lmax[element], pkdata->lmax_len[element]);
			saveFT(dacMax, buf);
			pkdata->sB_test[9].lmax_byte_size = get_file_size(buf);
			destroyFT(dacMax);
		}
		else if (pkdata->encoder_type == SIMPLE16)
		{
			uint32_t *output = (uint32_t *)calloc(pkdata->lmax_len[element], sizeof(uint32_t));	
			int32_t output_size = s16_compress((uint32_t *)pkdata->lmax[element], output, pkdata->lmax_len[element]);
		
			FILE *pf2 = fopen(buf, "w");
			fwrite(&pkdata->lmax_len[element], sizeof(uint32_t), 1, pf2);
			fwrite(output, sizeof(uint32_t), output_size, pf2);
			fclose(pf2);
			free(output);
			
			pkdata->sB_test[9].lmax_byte_size = get_file_size(buf);
		}
		else if (pkdata->encoder_type == PFD128)
		{
			uint32_t bloc_size = 128;
			uint32_t size = pkdata->lmax_len[element];
			uint32_t multiples = ((size + bloc_size) / bloc_size) * bloc_size;
			//uint32_t *input      = (uint32_t *)calloc(multiples, sizeof(uint32_t));
			uint32_t *compressed = (uint32_t *)calloc(size, sizeof(uint32_t));
			
			int32_t newsize = compress_pfordelta((uint32_t *)pkdata->lmax[element], compressed, multiples, bloc_size);
			
			FILE *fp200 = fopen(buf, "w");
			fwrite(&size, 1, sizeof(uint32_t), fp200);
			fwrite(compressed, newsize, sizeof(uint32_t), fp200);
			fclose(fp200);
			
			//free(input);
			free(compressed);
			
			pkdata->sB_test[9].lmax_byte_size = get_file_size(buf);
		}
	}
	
	// MIN
	sprintf(buf, "./data5/k%d_Min_%04d.dat", pkdata->k_val, band);
	if (all_zeros(pkdata->lmin[element], pkdata->lmin_len[element]))
	{
		printf("HAHAHA...ALL ZEROS MIN: k = %d\n", band);
		FILE * fp3 = fopen(buf, "w");
		fclose(fp3);
		pkdata->sB_test[9].lmin_byte_size = 0;
	}
	else
	{
		if (pkdata->encoder_type == DACS)
		{
			dacMin = createFT((uint32_t *)pkdata->lmin[element], pkdata->lmin_len[element]);
			saveFT(dacMin, buf);
			pkdata->sB_test[9].lmin_byte_size = get_file_size(buf);
			destroyFT(dacMin);
		}
		else if (pkdata->encoder_type == SIMPLE16)
		{
			uint32_t *output = (uint32_t *)calloc(pkdata->lmin_len[element], sizeof(uint32_t));	
			int32_t output_size = s16_compress((uint32_t *)pkdata->lmin[element], output, pkdata->lmin_len[element]);
		
			FILE *pf2 = fopen(buf, "w");
			fwrite(&pkdata->lmin_len[element], sizeof(uint32_t), 1, pf2);
			fwrite(output, sizeof(uint32_t), output_size, pf2);
			fclose(pf2);
			free(output);
			
			pkdata->sB_test[9].lmin_byte_size = get_file_size(buf);
		}
		else if (pkdata->encoder_type == PFD128)
		{
			uint32_t bloc_size = 128;
			uint32_t size = pkdata->lmin_len[element];
			uint32_t multiples = ((size + bloc_size) / bloc_size) * bloc_size;
			uint32_t *compressed = (uint32_t *)calloc(size, sizeof(uint32_t));
			
			int32_t newsize = compress_pfordelta((uint32_t *)pkdata->lmin[element], compressed, multiples, bloc_size);
			
			FILE *fp2 = fopen(buf, "w");
			fwrite(&size, 1, sizeof(uint32_t), fp2);
			fwrite(compressed, newsize, sizeof(uint32_t), fp2);
			fclose(fp2);
			
			free(compressed);
			
			pkdata->sB_test[9].lmin_byte_size = get_file_size(buf);
		}
	}

	pkdata->sB_test[9].snapshot        = 1;
	//pkdata->sB_test[9].t_byte_size     = pkdata->T_counter[element] * sizeof(uint32_t) + sizeof(uint16_t);
	pkdata->sB_test[9].t_byte_size     = pkdata->T_counter[element] * sizeof(uint32_t) + sizeof(uint32_t);
	pkdata->sB_test[9].total_byte_size = pkdata->sB_test[9].t_byte_size +
	                                     pkdata->sB_test[9].lmax_byte_size + 
										 pkdata->sB_test[9].lmin_byte_size;
	printf("Inside build_snapshot %ld %ld\n", pkdata->sB_test[9].lmax_byte_size, pkdata->sB_test[9].lmin_byte_size);

	return (EXIT_SUCCESS);
}

int32_t build_non_snapshot_raster(kdata    *pkdata,
								  int32_t  *vector_s,
								  int32_t  *vector_t,
								  uint32_t element,
								  uint32_t band)
{
	int32_t i;
	int32_t j;
	char	buf[PATH_MAX] = "";
	FTRep 	*dacMax = NULL;
	FTRep 	*dacMin = NULL;

	clear_data(pkdata, element);
	build_t(pkdata->k_val,
			  vector_s,
			  vector_t,
			  pkdata->nx_new,
			  pkdata->nx_new,
			  0,
			  0,
			  0,
			  pkdata,                                            
			  element);

	printf("lmax array:\n");
	pkdata->lmax_len[element] = 0;
	for (i = 0; i < (int32_t) pkdata->td; ++i)
	{
		for (j = 0; j < (int32_t) pkdata->vmax_count[element][i]; j++)
		{
			//////printf("level = %d\tcount = %d\t%d\n", i, j, pkdata->vmax[element][i][j]);
			pkdata->lmax_temp[1][pkdata->lmax_len[element]] = pkdata->lmax[element][pkdata->lmax_len[element]] = zigzag_encode(pkdata->vmax[element][i][j]);
			pkdata->lmax_len[element]++;
		}
		//printf("*******************\n");
	}
	pkdata->lmax_len_temp[1] = pkdata->lmax_len[element];

	//////printf("lmin array:\n");
	pkdata->lmin_len[element] = 0;
	for (i = 0; i < (int32_t) pkdata->td; ++i)
	{
		for (j = 0; j < (int32_t) pkdata->vmin_count[element][i]; j++)
		{
			///////printf("level = %d\tcount = %d\t%d\n", i, j, pkdata->vmin[element][i][j]);
			pkdata->lmin_temp[1][pkdata->lmin_len[element]] = pkdata->lmin[element][pkdata->lmin_len[element]] = zigzag_encode(pkdata->vmin[element][i][j]);
			pkdata->lmin_len[element]++;
		}
		//printf("*******************\n");
	}		
	pkdata->lmin_len_temp[1] = pkdata->lmin_len[element];	

	// stuffing element T
	pkdata->T_counter[element] = 0;
	for (j = 0; j < (int32_t)(pkdata->td - 1); ++j)
	{
		for (i = 0; i < (int32_t)pkdata->t_count[element][j]; ++i)
		{
			//printf("ns: pkdata->t[%d][%d][%d] = %d\n", element, j, i, pkdata->t[element][j][i]);
			pkdata->T_packed[element][pkdata->T_counter[element] / WORD_SIZE] |= (pkdata->t[element][j][i] << (pkdata->T_counter[element] % WORD_SIZE));
			pkdata->T_counter[element]++;
		}
	}
	
	pkdata->T_counter[element] = (uint32_t) CEILING_INT(pkdata->T_counter[element], WORD_SIZE);
	//pkdata->T_counter[element] = (uint16_t) ceil((double) pkdata->T_counter[element] / (double) WORD_SIZE);
	
	for (i = 0; i < (int32_t)pkdata->T_counter[element]; i++)
	{
		pkdata->T_temp[1][i] = pkdata->T_packed[element][i];
	}
	
	// stuffing element eqB
	pkdata->eqB_counter[element] = 0;
	for (j = 0; j < (int32_t)(pkdata->td - 1); ++j)
	{
		for (i = 0; i < (int32_t)pkdata->eqB_count[element][j]; ++i)
		{
			//// printf("ns: pkdata->eqB[%d][%d][%d] = %d\n", element, j, i, pkdata->eqB[element][j][i]);
			pkdata->eqB_packed[element][pkdata->eqB_counter[element] / WORD_SIZE] |= (pkdata->eqB[element][j][i] << (pkdata->eqB_counter[element] % WORD_SIZE));
			pkdata->eqB_counter[element]++;
		}
	}
	pkdata->eqB_counter[element] = (uint32_t) CEILING_INT(pkdata->eqB_counter[element], WORD_SIZE);
	
	pkdata->t_size[element] = 0;
	for (i = 0; i < (int32_t) (pkdata->td - 1); i++)
		pkdata->t_size[element] += pkdata->vmax_count[element][i];
	
	pkdata->t_size_temp[1] = pkdata->t_size[element];

	printf("T_counter[%d] = %d\n", pkdata->k_val, pkdata->t_size[element]);
	printf("eqB_size[%d] = %d\n", pkdata->k_val, pkdata->eqB_counter[element]);


	printf("Saving files............ \n");
	sprintf(buf, "./data5/k%d_Max_%04d.dat", pkdata->k_val, band);
	if (all_zeros(pkdata->lmax[element], pkdata->lmax_len[element]))
	{
		printf("ALL ZEROS MAX: k = %d\n", band);
		FILE * fp3 = fopen(buf, "w");
		fclose(fp3);
	}
	else
	{
		if (pkdata->encoder_type == DACS)
		{
			dacMax = createFT((uint32_t *)pkdata->lmax[element], pkdata->lmax_len[element]);
			saveFT(dacMax, buf);
			pkdata->sB_test[9].lmax_byte_size = get_file_size(buf);
			destroyFT(dacMax);
		}
		else if (pkdata->encoder_type == SIMPLE16)
		{
			uint32_t *output = (uint32_t *)calloc(pkdata->lmax_len[element], sizeof(uint32_t));	
			int32_t output_size = s16_compress((uint32_t *)pkdata->lmax[element], output, pkdata->lmax_len[element]);
		
			FILE *pf2 = fopen(buf, "w");
			fwrite(&pkdata->lmax_len[element], sizeof(uint32_t), 1, pf2);
			fwrite(output, sizeof(uint32_t), output_size, pf2);
			fclose(pf2);
			free(output);
			
			pkdata->sB_test[9].lmax_byte_size = get_file_size(buf);
		}
		else if (pkdata->encoder_type == PFD128)
		{
			uint32_t bloc_size = 128;
			uint32_t size = pkdata->lmax_len[element];
			uint32_t multiples = ((size + bloc_size) / bloc_size) * bloc_size;
			//uint32_t *input      = (uint32_t *)calloc(multiples, sizeof(uint32_t));
			uint32_t *compressed = (uint32_t *)calloc(size, sizeof(uint32_t));
			
			int32_t newsize = compress_pfordelta((uint32_t *)pkdata->lmax[element], compressed, multiples, bloc_size);
			
			FILE *fp200 = fopen(buf, "w");
			fwrite(&size, 1, sizeof(uint32_t), fp200);
			fwrite(compressed, newsize, sizeof(uint32_t), fp200);
			fclose(fp200);
			
			//free(input);
			free(compressed);
			
			pkdata->sB_test[9].lmax_byte_size = get_file_size(buf);
		}
	}
	
	////printf("Min = %d\n", lmin_len);
	sprintf(buf, "./data5/k%d_Min_%04d.dat", pkdata->k_val, band);
	if (all_zeros(pkdata->lmin[element], pkdata->lmin_len[element]))
	{
		printf("ALL ZEROS MIN: k = %d\n", band);
		FILE * fp3 = fopen(buf, "w");
		fclose(fp3);
	}
	else
	{
		if (pkdata->encoder_type == DACS)
		{
			dacMin = createFT((uint32_t *)pkdata->lmin[element], pkdata->lmin_len[element]);
			saveFT(dacMin, buf);
			pkdata->sB_test[9].lmin_byte_size = get_file_size(buf);
			destroyFT(dacMin);
		}
		else if (pkdata->encoder_type == SIMPLE16)
		{
			uint32_t *output = (uint32_t *)calloc(pkdata->lmin_len[element], sizeof(uint32_t));	
			int32_t output_size = s16_compress((uint32_t *)pkdata->lmin[element], output, pkdata->lmin_len[element]);
		
			FILE *pf2 = fopen(buf, "w");
			fwrite(&pkdata->lmin_len[element], sizeof(uint32_t), 1, pf2);
			fwrite(output, sizeof(uint32_t), output_size, pf2);
			fclose(pf2);
			free(output);
			
			pkdata->sB_test[9].lmin_byte_size = get_file_size(buf);
		}
		else if (pkdata->encoder_type == PFD128)
		{
			uint32_t bloc_size = 128;
			uint32_t size = pkdata->lmin_len[element];
			uint32_t multiples = ((size + bloc_size) / bloc_size) * bloc_size;
			uint32_t *compressed = (uint32_t *)calloc(size, sizeof(uint32_t));
			
			int32_t newsize = compress_pfordelta((uint32_t *)pkdata->lmin[element], compressed, multiples, bloc_size);
			
			FILE *fp2 = fopen(buf, "w");
			fwrite(&size, 1, sizeof(uint32_t), fp2);
			fwrite(compressed, newsize, sizeof(uint32_t), fp2);
			fclose(fp2);
			
			free(compressed);
			
			pkdata->sB_test[9].lmin_byte_size = get_file_size(buf);
		}
	}



	pkdata->sB_test[9].snapshot = 0;
	pkdata->sB_test[9].t_byte_size = pkdata->T_counter[element] * sizeof(uint32_t) + pkdata->eqB_counter[element] * sizeof(uint32_t) + sizeof(uint32_t) * 2;
	pkdata->sB_test[9].total_byte_size = pkdata->sB_test[9].t_byte_size + pkdata->sB_test[9].lmax_byte_size + pkdata->sB_test[9].lmin_byte_size;

	return (EXIT_SUCCESS);
}

void structure_add(sB_data *a, sB_data *b)
{
	//a->snapshot         += b->snapshot       ;
	a->t_byte_size      += b->t_byte_size    ;
	a->lmax_byte_size   += b->lmax_byte_size ;
	a->lmin_byte_size   += b->lmin_byte_size ;
	a->total_byte_size  += b->total_byte_size;
}

void fill_final(kdata *pkdata, uint32_t band_index, uint8_t temp_index, uint32_t element_index)
{
	int32_t i;
	
	pkdata->lmax_len_final[band_index] = pkdata->lmax_len_temp[temp_index];
	pkdata->lmin_len_final[band_index] = pkdata->lmin_len_temp[temp_index];
	pkdata->t_size_final[band_index]   = pkdata->t_size_temp[temp_index];
	
	// lmax
	for (i = 0; i < (int32_t)pkdata->lmax_len_final[band_index]; i++)
	{
		pkdata->lmax_final[band_index][i] = pkdata->lmax_temp[temp_index][i];
	}

	// lmin
	for (i = 0; i < (int32_t)pkdata->lmin_len_final[band_index]; i++)
	{
		pkdata->lmin_final[band_index][i] = pkdata->lmin_temp[temp_index][i];
	}

	// t_packed_count
	uint32_t t_size_packed  = (uint32_t)CEILING_INT(pkdata->t_size_final[band_index], WORD_SIZE);

	for (i = 0; i < (int32_t)t_size_packed; i++)
	{
		pkdata->T_packed_final[band_index][i] = pkdata->T_temp[temp_index][i];
	}
	
	if (element_index == 3 || element_index == 1)
	//if (temp_index == 1)
	{
		if (band_index == 246)
			printf("pkdata->eqB_counter[element_index] = %d\n", pkdata->eqB_counter[element_index]);
		pkdata->eqB_counter_final[band_index] = pkdata->eqB_counter[element_index];
		for (i = 0; i < (int32_t)pkdata->eqB_counter_final[band_index]; i++)
		{
			pkdata->eqB_packed_final[band_index][i] = pkdata->eqB_packed[element_index][i];
			if (band_index == 246)
				printf("pkdata->eqB_packed_final[%d][%d] = %08X\n", band_index, i, pkdata->eqB_packed_final[band_index][i]);
		}		
	}
}


int shuffle_array(int nz, std::vector<uint16_t> &my_vector)
{
	std::srand (time_t (std::time(0)));
	//std::vector<uint16_t> my_vector;
	int32_t i;
	
	for (i = 0; i < nz; i++)
		my_vector.push_back(i);
	
	std::random_shuffle(my_vector.begin(), my_vector.end());
	
	return (EXIT_SUCCESS);
}

bool sort_bands(const reorder &a, const reorder &b)
{
	if (a.data <= b.data)
		return false;
	else if (a.data > b.data)
		return true;
	
	return false;
}

double compute_entropy(int32_t *vect, uint32_t vect_len, uint32_t band)
{
	double ret = 0.0;
	
	int32_t hist[1024*64] = {0};
	int32_t histlen = makehist(vect, hist, vect_len);

	ret = entropy(hist, histlen, vect_len);
	
    //printf("Entropy value for band %d = %f\n", band, ret);
	
	return ret;	
}

int read_input(kdata    *pkdata,
               int32_t  k,
			   char     *command_line_file_name, 
               uint16_t *vector_old,
			   int32_t  *vector_new,
			   int32_t  nx_old,
			   int32_t  ny_old,
			   int16_t  row_div,
			   int16_t  col_div)
{
	int32_t i = 0;
	int32_t j = 0;
	int32_t m = 0; // loop index
	
	size_t orig_raster_size = nx_old * ny_old;
	
	FILE *fp = fopen(command_line_file_name, "rb");
	
	if (fp == NULL)
	{
		fprintf(stderr, "Error: Cannot open data file.\n");
		return (EXIT_FAILURE);
	}
	fseek(fp, orig_raster_size * sizeof(uint16_t) * k, SEEK_SET);
	
	memset(vector_new, 0, pkdata->nx_new * pkdata->nx_new * sizeof(int32_t));
	
	for (m = 0; m < (row_div * col_div); m++)
	{
		if ((k * (row_div * col_div) + m) >= (int32_t)pkdata->nz)
			break;
		
		if ((orig_raster_size / (row_div * col_div)) != fread(vector_old, sizeof(uint16_t), orig_raster_size / (row_div * col_div), fp))
		{
			fprintf(stderr, "\nError reading data file.\n");
			return (EXIT_FAILURE);
		}
		
		int32_t row_disp;
		int32_t col_disp;
		
		col_disp = nx_old * (m % col_div) / col_div;
		row_disp = ny_old * (m / col_div) / row_div;
		
		for (j = 0; j < (int32_t) ny_old / row_div; ++j)
		{
			for (i = 0; i < (int32_t) nx_old / col_div; ++i)
			{
				vector_old[j * nx_old / col_div + i] = ntohs(vector_old[j * nx_old / col_div + i]);
				
				if (pkdata->data_type == 2)
				{	
					vector_new[(j + row_disp) * pkdata->nx_new + (i + col_disp)] = vector_old[j * nx_old / col_div + i];
				}						
				else if (pkdata->data_type == 3)
				{	
					//vector_new[(j + row_disp) * pkdata->nx_new + (i + col_disp)] = zigzag_encode(vector_old[j * nx_old / col_div + i]);
					vector_new[(j + row_disp) * pkdata->nx_new + (i + col_disp)] = (int32_t)vector_old[j * nx_old / col_div + i];
					//vector_new[(j + row_disp) * pkdata->nx_new + (i + col_disp)] = (int16_t)vector_old[j * nx_old / col_div + i];
				}
			}
		}
	}

	fclose(fp);	
	
	return (EXIT_SUCCESS);
}

int test_get_cell(int32_t     encoder_type,
                  kdata       *pkdata,
				  const char  *command_line_file_name,
                  const char  *prog_dir,
				  const char  *file_name,
				  uint32_t    k_val,
				  char        data_type,
				  uint32_t    nx_old,
				  uint32_t    ny_old,
				  uint32_t    nx,
				  uint32_t    *nx_random,
				  uint32_t    *ny_random,
				  uint32_t    *nz_random)
{
	int32_t		i;
	int32_t		k;
	uint32_t	**T_local     = NULL;
	FILE		*fp1          = NULL;
	uint32_t	nz_local      = 0;
	uint32_t    delta_local   = 0;
	uint32_t    *T_size_local = 0;
	char		buf[PATH_MAX] = {0};
	size_t      fread_result;
	FTRep       **ftrep_local;
	// int32_t     r;
	// int32_t     c;


	printf("%s\n", command_line_file_name);

	sprintf(buf, "./data5/k%d_data.dat", k_val);
	
	printf("Data file: %s\n", buf);
	fp1 = fopen(buf, "rb");
	fread_result = fread(&nz_local, sizeof(uint32_t), 1, fp1);
	if (fread_result != 1)
	{
		fputs ("Reading error", stderr);
		return (EXIT_FAILURE);
	}
	fread_result = fread(&delta_local, sizeof(uint32_t), 1, fp1);
	if (fread_result != 1)
	{
		fputs ("Reading error", stderr);
		return (EXIT_FAILURE);
	}
	printf("%d\t%d\n", nz_local, delta_local);
	
	
	T_size_local = (uint32_t *)calloc(nz_local, sizeof(uint32_t));
	T_local = (uint32_t **)calloc(nz_local, sizeof(uint32_t *));
	ftrep_local = (FTRep **)calloc(nz_local, sizeof(FTRep *));
	
	for (k = 0; k < (int32_t)nz_local; ++k)
	{
		fread_result = fread(&T_size_local[k], sizeof(uint32_t), 1, fp1);
		if (fread_result != 1)
		{
			fputs ("Reading error", stderr);
			return (EXIT_FAILURE);
		}
		
		printf("T_size_local = %d\n", T_size_local[k]);
		uint32_t T_packed_size = CEILING_INT(T_size_local[k], WORD_SIZE);	
		T_local[k] = (uint32_t *)calloc(T_packed_size, sizeof(uint32_t));
		fread_result = fread(T_local[k], sizeof(uint32_t), T_packed_size, fp1);
		if (fread_result != T_packed_size)
		{
			fputs ("Reading error", stderr);
			return (EXIT_FAILURE);
		}

		// // // printf("T_packed_size = %d\n", T_packed_size);
	
		// // // for (j = 0; j < (int32_t) T_packed_size; ++j)
		// // // {
			// // // printf("*** %d\t0x%08X\n", j, T_local[k][j]);
		// // // }
	
		//sprintf(buf, "%s/data/%s_k%d_Max_%04d.dat", prog_dir, file_name, kd->k_val, k);
		sprintf(buf, "./data5/k%d_Max_%04d.dat", pkdata->k_val, k);
		ftrep_local[k] = loadFT(buf);
		// if (k == 0)
		// {
			// for (j = 0; j < 12888; j++)
			// {
				// int32_t val = accessFT(ftrep_local[k], j);
				// printf("%d\t%d\n", j, val);
			// }
		// }
	}
	fclose(fp1);
	
	struct timeval time_begin;
	struct timeval time_end;
	double         elapsed_time;
	clock_t        t = clock();
	uint16_t       x_val;
	uint16_t       y_val;
	uint16_t       z_val;

	gettimeofday(&time_begin, NULL);
	//printf("Verifying the first 50 iterations of random access in img_vector[%d][%d][%d]...\n", nx, ny, nz);
	//printf("%d\t%d\t%d\n", nx_old, ny_old, nz);
	//printf("[ x ][ y ][ z ]\n");
	
	// test_get_cell
	printf("Start...\n");
	for (i = 0; i < NUM_OF_ITERATIONS; ++i)
	{
		// if (i < 5)
			// printf("RANDOM INSIDE%d:\t%d\t%d\t%d\n", i, nx_random[i], ny_random[i], nz_random[i]);
		x_val = nx_random[i];
		y_val = ny_random[i];
		z_val = nz_random[i];
		int32_t max_val = accessFT(ftrep_local[z_val], 0);
		int32_t val = get_cell_s(pkdata,
								 encoder_type,
								 T_local[z_val],
								 ftrep_local[z_val],
								 T_size_local[z_val],
								 pkdata->nx_new,
								 y_val,
								 x_val,
								 0,
								 max_val,
								 0,
								 z_val);
		val--;
	}
	gettimeofday(&time_end, NULL);
	elapsed_time = (time_end.tv_sec - time_begin.tv_sec) +
				   (time_end.tv_usec - time_begin.tv_usec) * 1.0e-6;

	t = clock() - t;
	printf("Elapsed time (using gettimeofday()): %9f milliseconds\n", elapsed_time * 1000);
	printf("Elapsed time (using clock()):        %9f milliseconds\n", ((double)t/CLOCKS_PER_SEC) * 1000);	
	
	// //for (r = 0; r < (int32_t) pkdata->nx_new; r++) // row
	// for (r = 0; r < 1; r++) // row
	
	// {
		// //for (c = 0; c < (int32_t) pkdata->nx_new; c++)     // column
		// for (c = 0; c < 20; c++)     // column
		// {
			// int32_t max_val = accessFT(ftrep_local[0], 0);
			// int32_t val = get_cell_s(pkdata,
							         // encoder_type,
							         // T_local[0],
							         // ftrep_local[0],
							         // T_size_local[0],
							         // pkdata->nx_new,
							         // r,
							         // c,
							         // 0,
							         // max_val);
			// printf("row = %d\tcol = %d\tval = %d\n", r, c, val);
		// }
	// }	

	for (k = 0; k < (int32_t)nz_local; k++)
	{
		free(T_local[k]);
		free(ftrep_local[k]);
	}
	free(T_local);
	free(T_size_local);

	return (EXIT_SUCCESS);
}


int test_get_cell_v2(int32_t               encoder_type,
                     kdata                 *pkdata,
					 std::vector<uint16_t> &reordered_vector,
				     const char            *command_line_file_name,
                     const char            *prog_dir,
				     const char            *file_name,
				     uint32_t              k_val,
				     char                  data_type,
				     uint32_t              nx_old,
				     uint32_t              ny_old,
				     uint32_t              nx,
				     uint32_t              *nx_random,
				     uint32_t              *ny_random,
				     uint32_t              *nz_random)
{
	char buf[PATH_MAX + 64] = {0};
	uint32_t nz_local     = 0;
	uint32_t tau_local    = 0; 
	size_t   fread_result = 0;
	int16_t	 row_div      = 1;	
	int16_t  col_div      = 1;
	int32_t  i            = 0;
	//int32_t	 i            = 0;
	//int32_t	 j            = 0;
	int32_t  k            = 0;
	int32_t	 r            = 0; // row in get_cell functions
	int32_t	 c            = 0; // column in get_cell function
	
	// begin reloading from files
	sprintf(buf, "./data5/k%d_data.dat", pkdata->k_val);
	FILE *fp_irreg_read = fopen(buf, "r");		
	if (fp_irreg_read == NULL)
	{
		fprintf(stderr, "Error: Cannot open file.\n");
		return (EXIT_FAILURE);		
	}			
	
	// load nz
	fread_result = fread(&nz_local, sizeof(uint32_t), 1, fp_irreg_read);
	if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
	
	// load tau
	fread_result = fread(&tau_local, sizeof(uint32_t), 1, fp_irreg_read); if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}

	// load sB
	uint32_t sB_packed_count_local = (uint32_t)CEILING_INT(nz_local, WORD_SIZE);
	sB_data  *sB_unpacked_local    = (sB_data *)calloc(nz_local, sizeof(sB_data));
	uint32_t *sB_packed_local             = (uint32_t *)calloc(sB_packed_count_local, sizeof(uint32_t));
	fread_result = fread(sB_packed_local, sizeof(uint32_t), sB_packed_count_local, fp_irreg_read);
	if (fread_result != sB_packed_count_local) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
	for (int32_t k = 0; k < (int32_t)nz_local; k++)
		sB_unpacked_local[k].snapshot = bitget(sB_packed_local, k);
	
	uint32_t *sB_snapshot_pos       = (uint32_t *)calloc(nz_local, sizeof(uint32_t));
	
	uint32_t prev_snapshot_pos = 0;
	for (uint32_t k = 0; k < nz_local; k++)
	{
		
		if (sB_unpacked_local[k].snapshot == 1)
		{
			sB_snapshot_pos[k] = k;
			prev_snapshot_pos = k;
		}
		else
		{
			sB_snapshot_pos[k] = prev_snapshot_pos;
		}
	}
	

	// load T
	uint32_t *T_unpacked_count_local = (uint32_t *)calloc(nz_local, sizeof(uint32_t));
	uint32_t **T_packed_local        = (uint32_t **)calloc(nz_local, sizeof(uint32_t *));
	
	// load eqB
	uint32_t *eqB_packed_count_local = (uint32_t *)calloc(nz_local, sizeof(uint32_t));
	uint32_t **eqB_packed_local      = (uint32_t **)calloc(nz_local, sizeof(uint32_t *));
	
	// load Lmax compressed by DACs
	FTRep **ftrep_local              = (FTRep **)calloc(nz_local, sizeof(FTRep *));
	
	for (int32_t k = 0; k < (int32_t)CEILING_INT(nz_local, (row_div * col_div)); k++)
	{
		fread_result = fread(&T_unpacked_count_local[k], sizeof(uint32_t), 1, fp_irreg_read);
		if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		
		uint32_t T_packed_count_local = CEILING_INT(T_unpacked_count_local[k], WORD_SIZE);
		T_packed_local[k] = (uint32_t *)calloc(T_packed_count_local, sizeof(uint32_t));
		
		fread_result = fread(T_packed_local[k], sizeof(uint32_t), T_packed_count_local, fp_irreg_read);
		if (fread_result != T_packed_count_local) {fputs ("Reading error", stderr); return (EXIT_FAILURE);} 
		
		fread_result = fread(&eqB_packed_count_local[k], sizeof(uint32_t), 1, fp_irreg_read);
		if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		
		eqB_packed_local[k] = (uint32_t *)calloc(eqB_packed_count_local[k], sizeof(uint32_t));
		fread_result = fread(eqB_packed_local[k], sizeof(uint32_t), eqB_packed_count_local[k], fp_irreg_read);
		if (fread_result != eqB_packed_count_local[k]) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		
		sprintf(buf, "./data5/k%d_Max_%04d.dat", pkdata->k_val, k);
		if (get_file_size(buf) == 0L)
			ftrep_local[k] = NULL;
		else
			ftrep_local[k] = loadFT(buf);
	}
	
	fclose(fp_irreg_read);
// end reloading from files


	struct timeval time_begin;
	struct timeval time_end;
	double         elapsed_time;
	clock_t        t = clock();
	uint16_t       x_val;
	uint16_t       y_val;
	uint16_t       z_val;
	
	gettimeofday(&time_begin, NULL);

	printf("Start...\n");
	for (i = 0; i < NUM_OF_ITERATIONS; ++i)
	{
		// if (i < 5)
			// printf("RANDOM INSIDE%d:\t%d\t%d\t%d\n", i, nx_random[i], ny_random[i], nz_random[i]);
		x_val = nx_random[i];
		y_val = ny_random[i];
		z_val = nz_random[i];

		if (sB_unpacked_local[z_val].snapshot == 1)
		{
			if (ftrep_local[z_val] == NULL)
				continue;
			
			int32_t max_val = accessFT(ftrep_local[z_val], 0);
			int32_t val = get_cell_s(pkdata,
					 DACS,
					 T_packed_local[z_val], // pkdata->T_packed_final[k], // T_packed_local[k],
					 ftrep_local[z_val],
					 T_unpacked_count_local[z_val],
					 pkdata->nx_new,
					 y_val,
					 x_val,
					 0,
					 max_val,
					 0,
					 z_val);
			val--;
		}
		else
		{
			//int32_t prev = get_prev_snapshot_random(sB_unpacked_local, reordered_vector, nz_local, z_val, 0);
			int32_t prev = sB_snapshot_pos[z_val];
			if (ftrep_local[prev] == NULL || ftrep_local[z_val] == NULL)
				continue;
			int32_t max_val_s = accessFT(ftrep_local[prev], 0);
			int32_t max_val_t = accessFT(ftrep_local[z_val], 0);
			int32_t val = get_cell_t(
					pkdata,
					DACS,
					pkdata->k_val,
					T_packed_local[prev],
					T_packed_local[z_val],
					eqB_packed_local[z_val], // pkdata->eqB_packed_final[k], //eqB_packed_local[k],
					ftrep_local[prev], // pkdata->lmax_final[prev], // ftrep_local[prev],
					ftrep_local[z_val],   // pkdata->lmax_final[k],    // ftrep_local[k],
					T_unpacked_count_local[prev],
					T_unpacked_count_local[z_val],
					pkdata->nx_new,
					r,
					c,
					0,
					0,
					max_val_s,
					max_val_t,
					0,
					k);
			val--;
		}
	}
	gettimeofday(&time_end, NULL);
	elapsed_time = (time_end.tv_sec - time_begin.tv_sec) +
				   (time_end.tv_usec - time_begin.tv_usec) * 1.0e-6;

	t = clock() - t;
	printf("Elapsed time (using gettimeofday()): %9f milliseconds\n", elapsed_time * 1000);
	printf("Elapsed time (using clock()):        %9f milliseconds\n", ((double)t/CLOCKS_PER_SEC) * 1000);	
	

	
		// start cell checking
		printf("Inside get_cell_checking() - Irregular groupings\n");
		
		// //if (pkdata->sB[k].snapshot == 1)
		// if (sB_unpacked_local[k].snapshot == 1)
		// {
			// for (r = 0; r < (int32_t) pkdata->nx_new; r++) // row
			// {
				// for (c = 0; c < (int32_t) pkdata->nx_new; c++)     // column
				// {
					// int32_t max_val = accessFT(ftrep_local[k], 0);
					// int32_t val = get_cell_s(pkdata,
							 // DACS,
							 // T_packed_local[k], // pkdata->T_packed_final[k], // T_packed_local[k],
							 // ftrep_local[k],
							 // T_unpacked_count_local[k],
							 // pkdata->nx_new,
							 // r,
							 // c,
							 // 0,
							 // max_val);
					// val--;
				// }
			// }
		// }
		// else // (pkdata->sB[k].snapshot == 0)
		// {
			// //int32_t prev = get_prev_snapshot(pkdata->sB, k);
			// //int32_t prev = get_prev_snapshot_random(pkdata->sB, reordered_vector, pkdata->nz, k, 0);
			// int32_t prev = get_prev_snapshot_random(sB_unpacked_local, reordered_vector, pkdata->nz, k, 0);
			
			// //int32_t found = 0;
			
			// for (r = 0; r < (int32_t) pkdata->nx_new; r++) // row
			// {
				// for (c = 0; c < (int32_t) pkdata->nx_new; c++)     // column
				// {
					// int32_t max_val_s = accessFT(ftrep_local[prev], 0);
					// int32_t max_val_t = accessFT(ftrep_local[k], 0);
					// int32_t val = get_cell_t(
							// pkdata,
							// DACS,
							// pkdata->k_val,
							// T_packed_local[prev],
							// T_packed_local[k],
							// eqB_packed_local[k], // pkdata->eqB_packed_final[k], //eqB_packed_local[k],
							// ftrep_local[prev], // pkdata->lmax_final[prev], // ftrep_local[prev],
							// ftrep_local[k],    // pkdata->lmax_final[k],    // ftrep_local[k],
							// T_unpacked_count_local[prev],
							// T_unpacked_count_local[k],
							// pkdata->nx_new,
							// r,
							// c,
							// 0,
							// 0,
							// max_val_s,
							// max_val_t,
							// 0,
							// k);
					// val--;

				// }
				
				// // if (found)
					// // break;
			// }
		// }

	// }
	
	// clean up 1
	for (k = 0; k < (int32_t)nz_local; ++k)
	{
		if (T_packed_local[k] != NULL)
			free(T_packed_local[k]);
		
		if (eqB_packed_local[k] != NULL)
			free(eqB_packed_local[k]);
		
		if (ftrep_local[k] != NULL)
			destroyFT(ftrep_local[k]);
	}
	if (sB_unpacked_local != NULL)
		free(sB_unpacked_local);
	if (sB_packed_local != NULL)
		free(sB_packed_local);
	if (sB_snapshot_pos != NULL)
		free(sB_snapshot_pos);
	if (T_unpacked_count_local != NULL)
		free(T_unpacked_count_local);
	if (eqB_packed_count_local != NULL)
		free(eqB_packed_count_local);
	if (T_packed_local != NULL)
		free(T_packed_local);
	if (eqB_packed_local != NULL)
		free(eqB_packed_local);	
	return (EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	char        command_line_file_name[PATH_MAX] = {0};
	char		prog_file_name[PATH_MAX]         = {0}; // File name of the image used in the program, doesn't include path.
	char        prog_dir[PATH_MAX]               = {0};
	char		buf[PATH_MAX + 64]               = {0};
	int32_t		i            = 0;
	int32_t		j            = 0;
	int32_t		k            = 0;
	kdata     	kd           = {0};
	int32_t		r            = 0; // row in get_cell functions
	int32_t		c            = 0; // column in get_cell function
	uint16_t    **vector_old = NULL;
	int32_t     **vector_new = NULL;
	int16_t		row_div      = 1;	
	int16_t     col_div      = 1;
	size_t      fread_result = 0;

	// user options
	char        get_cell_checking = 0;
	char        tk2r_build_type   = 0; // 0 for regularly spaced snapshot and 1 for irregularly spaced snapshot

	kd.data_type    = 2;
	kd.k_val        = 2; // argv[6]
	kd.tau          = 2; // argv[7]
	kd.reorder_type = REORDER_UNCHANGED;
	kd.encoder_type = DACS; // SIMPLE16; // PFD128; // SIMPLE16; // 
	
	std::vector<uint16_t> reordered_vector;
	
	if (argc < 10)
	{
		print_help_message();
		return (EXIT_FAILURE);
	}

	strcpy(command_line_file_name, argv[1]);
	if (file_exists(command_line_file_name) < 0) {
		fprintf(stderr, "\nFile does not exist. ");
		return -1;
	}
	if (get_base_name(command_line_file_name, prog_file_name) < 0) {
		fprintf(stderr, "\nCannot get file name. ");
		return -1;
	}
	get_dir(prog_dir);
	
	kd.nz                = atoi(argv[2]); // get  first command line parameter nz
	kd.ny                = atoi(argv[3]) * row_div; // get second command line parameter ny - height or row
	kd.nx                = atoi(argv[4]) * col_div; // get  third command line parameter nx - width or column
	kd.data_type         = atoi(argv[5]); // get  third command line parameter data_type
	kd.k_val             = atoi(argv[6]); // get k value from command line
	kd.tau               = atoi(argv[7]); // get tau value from command line
	get_cell_checking    = atoi(argv[8]); // get_cell checking or no: 0 for no, 1 for yes
	tk2r_build_type      = atoi(argv[9]);
	if (argc > 10)
		kd.reorder_type      = atoi(argv[10]);

	double x_log = (int)ceil(log((double)(kd.nx)) / log((double)kd.k_val));
	double y_log = (int)ceil(log((double)(kd.ny)) / log((double)kd.k_val));
	
	printf("1st step x_log: %0.19f\n", x_log);
	printf("1st step y_log: %0.19f\n", y_log);

	kd.td = (uint8_t)max(x_log, y_log) + 1;
	
	printf("Tree depth: %d\n", kd.td);

	kd.nx_new = pow((double)kd.k_val, kd.td - 1);
	
	//nx_orig = kd.nx_new;

	printf("nx_new: %d\n", kd.nx_new);
	printf("ny_new: %d\n", kd.nx_new);

	if (tk2r_build_type == 1)
		kd.tau = 6;

	//if (initialize(kd.k_val, kd.td, &kd.num_elements, kd.nz, &kd, kd.tau) == EXIT_FAILURE)
	if (initialize(&kd) == EXIT_FAILURE)
		return (EXIT_FAILURE);

	// initialize vector_old and vector_new
	if ((vector_old = (uint16_t **)calloc(kd.tau, sizeof(uint16_t *))) == NULL)
		return -2;

	for (j = 0; j < (int32_t) kd.tau; j++)
	{
		if ((vector_old[j] = (uint16_t *)calloc(kd.ny * kd.nx, sizeof(uint16_t))) == NULL)
		{
			fprintf(stderr, "\nError allocating memory. ");
			return -2;
		}		
	}

	if ((vector_new = (int32_t **)calloc(kd.tau, sizeof(int32_t *))) == NULL)
		return -2;
	
	for (j = 0; j < (int32_t) kd.tau; j++)
	{
		if ((vector_new[j] = (int32_t *)calloc(kd.nx_new * kd.nx_new, sizeof(int32_t))) == NULL)
		{
			fprintf(stderr, "\nError allocating memory. ");
			return (EXIT_FAILURE);
		}
	}	

	if ((kd.vector_new_2 = (int32_t **)calloc(kd.tau, sizeof(int32_t *))) == NULL)
		return -2;
	
	for (j = 0; j < (int32_t) kd.tau; j++)
	{
		if ((kd.vector_new_2[j] = (int32_t *)calloc((kd.nx_new / kd.k_val) * (kd.nx_new / kd.k_val), sizeof(int32_t))) == NULL)
		{
			fprintf(stderr, "\nError allocating memory. ");
			return (EXIT_FAILURE);
		}
	}	


	size_t	orig_raster_size = kd.nx * kd.ny;	

	switch (kd.reorder_type)
	{
		default:
		case REORDER_UNCHANGED:
			for (i = 0; i < (int32_t)kd.nz; i++)
				reordered_vector.push_back(i);
			break;
		case REORDER_RANDOM:
			shuffle_array(kd.nz, reordered_vector);
			break;
	} // end switch

/*
	if ((fp7 = fopen(command_line_file_name, "rb")) == NULL)
	{
		fprintf(stderr, "\nError opening data file.\n");
		return (EXIT_FAILURE);				
	}
	for (k = 0; k < kd.nz; k++)
	{
		if (orig_raster_size != fread(vector_old[0], sizeof(uint16_t), orig_raster_size, fp7))
		{
			fprintf(stderr, "\nError reading data file.\n");
			return (EXIT_FAILURE);
		}
		int64_t total = 0;
		for (j = 0; j < (int32_t)kd.ny; ++j)
		{
			for (i = 0; i < (int32_t)kd.nx; ++i)
			{
				vector_old[0][j * kd.nx + i] = ntohs(vector_old[0][j * kd.nx + i]);
				if (data_type == 2)
				{
					total += vector_old[0][j * kd.nx + i];	
				}					
				else if (data_type == 3)
				{
					total += (int32_t)vector_old[0][j * kd.nx + i];
				}
			}
		}		
		
		kd.average_array[k] = (int32_t)((double)total / (double)orig_raster_size);
		printf("Average_array[%d] = %d\n", k, kd.average_array[k]);
	}
	fclose(fp7);
*/

	if (tk2r_build_type == 0) // Regular groupings
	{
		sprintf(buf, "./data5/k%d_data.dat", kd.k_val);
		FILE *fp2 = fopen(buf, "w");
		//uint32_t nz_temp = (uint32_t)kd.nz;
		fwrite(&kd.nz, sizeof(uint32_t), 1, fp2);
		fwrite(&kd.tau, sizeof(uint32_t), 1, fp2);

		uint32_t nx_old = kd.nx;
		uint32_t ny_old = kd.ny;
		//kd.nx = kd.nx_new;
		//kd.ny = kd.nx_new;
		
		FILE *fp_len = fopen("len.txt", "w");
		for (int32_t kk = 0; kk < (int32_t)CEILING_INT(kd.nz, (row_div * col_div)); kk++)
		{
			//k = reordered_vector[kk];
			k = kk;
			
			fprintf(stderr, "\nkk=%d, k=%d\n", kk, k);
		
			uint32_t k_delta = kk % kd.tau;
			printf("\nk = %d\tk_delta = %d\n", k, k_delta);
			
			read_input(&kd,
					   k,
					   command_line_file_name, 
					   vector_old[k_delta],
					   vector_new[k_delta],
					   nx_old,
					   ny_old,
					   row_div,
					   col_div);

			// for (int r = 0; r < 1; r++)
			// {
				// for (int c = 0; c < 20; c++)
				// {
					// printf("r = %d\tc = %d\tval = %d\n", r, c, vector_old[k_delta][r * nx_old + c]);
				// }
			// }

			// INSERT 1 HERE



			if (k_delta == 0) // snapshot raster
			{
				build_snapshot_raster(&kd, vector_new[k_delta], k_delta, k);
				kd.sB[k] = kd.sB_test[9];

// FOR TESTING: write lmax to file
if (k == 0 || k == 30 || k == 44)
{
	sprintf(buf, "lmax_%04d.txt", k);
	FILE *fp_test = fopen(buf, "w");
	for (i = 0; i < (int32_t)kd.lmax_len[k_delta]; ++i)
		fprintf(fp_test, "%d\t%d\n", i, kd.lmax[k_delta][i]);
	fclose(fp_test);					
}
				
				structure_add(&kd.sB_test[7], &kd.sB_test[9]);
				structure_add(&kd.sB_test[8], &kd.sB_test[9]);

				// fwrite(&kd.T_counter[0], sizeof(uint16_t), 1, fp2);
				// printf("kd.T_counter[0] = %d\n", kd.T_counter[0]);
				
				fwrite(&kd.t_size[0], sizeof(uint32_t), 1, fp2);
				printf("kd.t_size[0] = %d\n", kd.t_size[0]);
				fwrite(kd.T_packed[0], sizeof(uint32_t), kd.T_counter[0], fp2);
				
				// for (j = 0; j < kd.T_counter[0]; j++)
				// {
					// printf("%d\t%08X\n", j, kd.T_packed[0][j]);
				// }
			}	
			else // non-snapshot rasters
			{
				printf("Build the non-snapshot raster....\n");
      			build_non_snapshot_raster(&kd, vector_new[0], vector_new[k_delta], k_delta, k);
				kd.sB[k] = kd.sB_test[9];
				structure_add(&kd.sB_test[8], &kd.sB_test[9]);
				
// FOR TESTING: write lmax to file
if (k == 1 || k == 31 || k == 45)
{
	sprintf(buf, "lmax_%04d.txt", k);
	FILE *fp_test = fopen(buf, "w");
	for (i = 0; i < (int32_t)kd.lmax_len[k_delta]; ++i)
		fprintf(fp_test, "%d\t%d\n", i, kd.lmax[k_delta][i]);
	fclose(fp_test);					
}

				// fwrite(&kd.T_counter[k_delta], sizeof(uint32_t), 1, fp2);
				// fwrite(&kd.T_packed[k_delta], sizeof(uint32_t), kd.T_counter[k_delta], fp2);
				// fwrite(&kd.eqB_counter[k_delta], sizeof(uint32_t), 1, fp2);
				// fwrite(&kd.eqB_packed[k_delta], sizeof(uint32_t), kd.eqB_counter[k_delta], fp2);

				// Try new thing: for the current "k_delta" band,
				// which would be smaller: using k2raster or T-k2raster?
				printf("Now build the snapshot raster for the current \"k_delta\" band....\n");
				build_snapshot_raster(&kd, vector_new[k_delta], kd.tau, k);
				
				if (kd.sB_test[9].total_byte_size < kd.sB[k].total_byte_size)
				{
					structure_add(&kd.sB_test[7], &kd.sB_test[9]); // sB_test[7] for printing grand total
					kd.sB[k] = kd.sB_test[9];
				}
				else
				{
					structure_add(&kd.sB_test[7], &kd.sB[k]);
				}
			}
			fprintf(fp_len, "%d\t%d\n", k, kd.lmax_len[k_delta]);
			
			///////////////////////////////////////////
			// call get_cell() to check data
			// ///////////////////////////////////////////
			if (get_cell_checking)
			{
				printf("Inside get_cell_checking() - Regular groupings\n");
				
				
				int k_to_print = k == 30 || k == 31 || k == 44 || k == 45;
				
				FILE *fp = NULL;
				if (k_to_print)
				{
					sprintf(buf, "band_%d.txt", k);
					fp = fopen(buf, "w");
				}
				
				for (r = 0; r < (int32_t) kd.nx_new; r++) // row
				{
					for (c = 0; c < (int32_t) kd.nx_new; c++)     // column
					{
						int32_t val = access_data(&kd,
						                          r,
												  c,
												  k_delta,
												  k);

						//printf("row = %d\tcol = %d\tval = %d\torig_val = %d\n", r, c, val, vector_new[k_delta].s32[r * kd.nx_new + c]);
						
						if (k_to_print)
							fprintf(fp, "%d\t", vector_new[k_delta][r * kd.nx_new + c]);
						
						if (vector_new[k_delta][r * kd.nx_new + c] != val)
						{
							printf("Something is wrong. (1) \tk= %d\tr = %d\tc= %d\tk_delta = %d\torig_val = %d\tget_cell_val = %d\n",
								   k, r, c, k_delta, vector_new[k_delta][r * kd.nx_new + c], val);
						}
					}
					if (k_to_print)
						fprintf(fp, "\n");
				}
				
				if (k_to_print)
				{
					fclose(fp);
				}
					
/*
				if (k_delta != 0) // this is for snapshot
				{
					printf("Inside\tk_delta = %d\n", k_delta);
					for (r = 0; r < (int32_t) kd.nx_new; r++) // row
					{
						for (c = 0; c < (int32_t) kd.nx_new; c++)     // column
						{
							int32_t val = access_data(&kd,
												 r,
												 c,
												 kd.tau,
												 k);
							// if (k < 10)
								// printf("row = %d\tcol = %d\tval = %d\torig_val = %d\n", r, c, val, vector_new[k_delta][r * kd.nx_new + c]);
							
							if (vector_new[k_delta][r * kd.nx_new + c] != val)
							{
								printf("Something is wrong. (2) \tr = %d\tc= %d\torig_val = %d\tget_cell_val = %d\n",
								       r, c, vector_new[k_delta][r * kd.nx_new + c], val);
							}
						}
					}	
				}
*/
			} // finish get_cell_checking
		}

	
		fclose(fp2);
		fclose(fp_len);
		
		sprintf(buf, "./data5/k%d_data.dat", kd.k_val);
		//uint64_t file_size_data = get_file_size(buf);
		printf("TOTAL:\n");
		//uint32_t grand_nz = (uint32_t)ceil((double)kd.nz / (double)WORD_SIZE) * sizeof(uint32_t);
		printf("file_size_data, file_size_max, file_size_min, Total\t%ld\t%ld\t%ld\t%ld\n",
			        kd.sB_test[8].t_byte_size + 2 * sizeof(uint32_t),
					kd.sB_test[8].lmax_byte_size,
					kd.sB_test[8].lmin_byte_size,
					kd.sB_test[8].total_byte_size + 2 * sizeof(uint32_t));
		
		if (kd.tau != 1)
		{
			printf("HEURISTIC TOTAL:\n");
			uint32_t nz_byte_size = (uint32_t) CEILING_INT(kd.nz, WORD_SIZE) * sizeof(uint32_t); // account for sB array
			//uint32_t nz_byte_size = (uint32_t)ceil((double)kd.nz / (double)WORD_SIZE) * sizeof(uint32_t); 
			printf("file_size_data, file_size_max, file_size_min, Total\t%ld\t%ld\t%ld\t%ld\n",
			        kd.sB_test[7].t_byte_size + nz_byte_size + 2 * sizeof(uint32_t),
					kd.sB_test[7].lmax_byte_size,
					kd.sB_test[7].lmin_byte_size,
					kd.sB_test[7].total_byte_size + nz_byte_size + 2 * sizeof(uint32_t));
		}

		// clean up
		for (j = 0; j < (int32_t) kd.tau; j++)
		{		
			free(vector_new[j]);
			free(vector_old[j]);
		}
		
		free(vector_new);
		free(vector_old);

		//free_memory(&kd);
	}
/**************** if (tk2r_build_type == 1) *******************/
	else if (tk2r_build_type == 1) // Irregular groupings
	{
		// sprintf(buf, "./data5/k%d_data.dat", kd.k_val);
		// FILE *fp2 = fopen(buf, "wb");
		// fwrite(&kd.nz, sizeof(uint32_t), 1, fp2);
		// fwrite(&kd.tau, sizeof(uint32_t), 1, fp2);

		uint32_t nx_old = kd.nx;
		uint32_t ny_old = kd.ny;
		//kd.nx = kd.nx_new;
		//kd.ny = kd.nx_new;
		
		//size_t	orig_raster_size = ny_old * nx_old;	
		
		//for (int32_t kk = 0; kk < (int32_t)kd.nz; kk++)
		for (int32_t kk = 0; kk < (int32_t)CEILING_INT(kd.nz, (row_div * col_div)); kk++)
		{
			k = reordered_vector[kk];

			printf("\n");
			fprintf(stderr, "kk=%d\tk=%d\n", kk, k);
			
			read_input(&kd,
					   k,
					   command_line_file_name,
					   vector_old[0],
					   vector_new[0],
					   nx_old,
					   ny_old,
					   row_div,
					   col_div);				
		
			// build_s....
			build_snapshot_raster(&kd, vector_new[0], 0, k);
			kd.sB[k] = kd.sB_test[9];
			fill_final(&kd, k, 0, 0);
					
			//////if (k == 0)
			if (kk == 0)
				continue;
			
			int32_t prev_index = get_prev_snapshot_random(kd.sB, reordered_vector, kd.nz, k, 1);
			
			// if (kd.sB[k - 1].snapshot == 1)
			if (kd.sB[prev_index].snapshot == 1)
			{
				//compare both snapshot and non-snapshot builds
				
				// now let's build non-snapshot T-k2raster. 
				// test new build

printf("A===>\n");

				read_input(&kd,
					   k - 1,
					   command_line_file_name,
					   vector_old[1],
					   vector_new[1],
					   nx_old,
					   ny_old,
					   row_div,
					   col_div);	

				build_non_snapshot_raster(&kd, vector_new[1], vector_new[0], 1, k);
				
				if (kd.sB_test[9].total_byte_size < kd.sB[k].total_byte_size)
				{
printf("B===>\n");
					kd.sB[k] = kd.sB_test[9];
					fill_final(&kd, k, 1, 1);
				}
			}
			else // if (kd.sB[k - 1].snapshot == 0)
			{
				// (2) Represent t using a k2raster' with respect to the snapshot at s.
				//int32_t prev = get_prev_snapshot(kd.sB, k);
				int32_t prev = get_prev_snapshot_random(kd.sB, reordered_vector, kd.nz, k, 0);
				printf("k = %d\tprev = %d\n", k, prev);
				if (prev == -1)
					continue; // use the normal snapshot

printf("C===>\n");

				read_input(&kd,
					   prev,
					   command_line_file_name,
					   vector_old[1],
					   vector_new[1],
					   nx_old,
					   ny_old,
					   row_div,
					   col_div);

				build_non_snapshot_raster(&kd, vector_new[1], vector_new[0], 1, k); // [1] is the previous snapshot, [0] is the current raster
				
				if (kd.sB_test[9].total_byte_size < kd.sB[k].total_byte_size)
				{
printf("D===>\n");
					kd.sB[k] = kd.sB_test[9];
					fill_final(&kd, k, 1, 1);					
				}					
				
				//// (3) Change the representation of t-1 to use a snapshot
				// build_s for k - 1	
				read_input(&kd,
					   prev_index,
					   command_line_file_name,
					   vector_old[1],
					   vector_new[1],
					   nx_old,
					   ny_old,
					   row_div,
					   col_div);				

				build_snapshot_raster(&kd, vector_new[1], 2, k);
				kd.sB_test[2] = kd.sB_test[9];

				//// build_t for k		
				read_input(&kd,
					   k,
					   command_line_file_name,
					   vector_old[2],
					   vector_new[2],
					   nx_old,
					   ny_old,
					   row_div,
					   col_div);

				build_non_snapshot_raster(&kd, vector_new[1], vector_new[2], 3, k);
				kd.sB_test[3] = kd.sB_test[9];
				
				// printf("%ld %ld %ld\n", kd.sB_test[2].total_byte_size, kd.sB_test[3].total_byte_size,
				                        // kd.sB_test[2].total_byte_size + kd.sB_test[3].total_byte_size);
				// printf("%ld %ld %ld\n", kd.sB[k].total_byte_size, kd.sB[k - 1].total_byte_size,
				                        // kd.sB[k].total_byte_size + kd.sB[k - 1].total_byte_size);
				
				uint64_t test_result = kd.sB_test[2].total_byte_size + kd.sB_test[3].total_byte_size;
				//uint64_t old_values = kd.sB[k].total_byte_size + kd.sB[k - 1].total_byte_size;
				uint64_t old_values = kd.sB[k].total_byte_size + kd.sB[prev_index].total_byte_size;
				if (test_result < old_values)
				{
printf("E===>\n");
					//kd.sB[k - 1] = kd.sB_test[2]; // snapshot
					kd.sB[prev_index] = kd.sB_test[2]; // snapshot
					kd.sB[k] = kd.sB_test[3];   // non-snashot
					//fill_final(&kd, k - 1, 0, 2);
					fill_final(&kd, prev_index, 0, 2);
					fill_final(&kd, k, 1, 3);
				}
			}
			printf("Irregular intervals: k = %d\tsnapshot = %d\tfile_size_data1 = %ld\tfile_size_max = %ld\tfile_size_min = %ld\tTotal = %ld\n",
						k, kd.sB[k].snapshot, kd.sB[k].t_byte_size, kd.sB[k].lmax_byte_size, kd.sB[k].lmin_byte_size, kd.sB[k].total_byte_size);
		}

// begin saving to files
		FTRep 	    *dacMax = NULL;
		FTRep 	    *dacMin = NULL;
		sprintf(buf, "./data5/k%d_data.dat", kd.k_val);
		FILE *fp_irreg_write = fopen(buf, "w");
		if (fp_irreg_write == NULL)
		{
			fprintf(stderr, "Error: Cannot open file.\n");
			return (EXIT_FAILURE);		
		}	
		
		// save nz
		fwrite(&kd.nz, sizeof(uint32_t), 1, fp_irreg_write);
		
		// save tau
		fwrite(&kd.tau, sizeof(uint32_t), 1, fp_irreg_write);
		
		// save sB
		uint32_t sB_packed_count = (uint32_t)CEILING_INT(kd.nz, WORD_SIZE);
		//uint32_t sB[sB_packed_count];
		uint32_t *sB = (uint32_t *)calloc(sB_packed_count, sizeof(uint32_t));
		for (int32_t kk = 0; kk < (int32_t)CEILING_INT(kd.nz, (row_div * col_div)); kk++)
		{	
			k = reordered_vector[kk];
			sB[k / WORD_SIZE] |= kd.sB[k].snapshot << (k % WORD_SIZE);
		}
		fwrite(sB, sizeof(uint32_t), sB_packed_count, fp_irreg_write);
		free(sB);

		uint64_t lmax_size = 0;
		uint64_t lmin_size = 0;
		uint64_t total_t_size = 0;
		uint64_t total_lmax_size = 0;
		uint64_t total_lmin_size = 0;
		
		for (int32_t kk = 0; kk < (int32_t)CEILING_INT(kd.nz, (row_div * col_div)); kk++)
		{	
			k = reordered_vector[kk];
			
			// save T
			fwrite(&kd.t_size_final[k], sizeof(uint32_t), 1, fp_irreg_write);
			fwrite(kd.T_packed_final[k], sizeof(uint32_t), CEILING_INT(kd.t_size_final[k], WORD_SIZE), fp_irreg_write);

			// save eqB
			//printf("===> eqB_counter_final[%d] = %d\n", k, kd.eqB_counter_final[k]);
			fwrite(&kd.eqB_counter_final[k], sizeof(uint32_t), 1, fp_irreg_write);
			fwrite(kd.eqB_packed_final[k], sizeof(uint32_t), kd.eqB_counter_final[k], fp_irreg_write);
			// uint32_t eqB_packed_count = CEILING_INT(kd.t_size_final[k], WORD_SIZE);
			// fwrite(&eqB_packed_count, sizeof(uint32_t), 1, fp_irreg_write);
			// fwrite(kd.eqB_packed_final[k], sizeof(uint32_t), eqB_packed_count, fp_irreg_write);


			printf("%d\tsB =\t%d\tFirst =\t%d\tSecond =\t%d\n", k, kd.sB[k].snapshot, kd.t_size_final[k], kd.eqB_counter_final[k]);
// if (k == 0)
// {
	// // FOR TESTING: write lmax to file
	// FILE *fp_test = fopen("v5_000.txt", "w");
	// for (i = 0; i < (int32_t)kd.lmax_len_final[k]; ++i)
		// fprintf(fp_test, "%d\t%d\n", i, kd.lmax_final[k][i]);
	// fclose(fp_test);
// }
// if (k == 244)
// {
	// // FOR TESTING: write lmax to file
	// FILE *fp_test = fopen("v5_244.txt", "w");
	// for (i = 0; i < (int32_t)kd.lmax_len_final[k]; ++i)
		// fprintf(fp_test, "%d\t%d\n", i, kd.lmax_final[k][i]);
	// fclose(fp_test);
// }
// if (k == 245)
// {
	// // FOR TESTING: write lmax to file
	// FILE *fp_test = fopen("v5_245.txt", "w");
	// for (i = 0; i < (int32_t)kd.lmax_len_final[k]; ++i)
		// fprintf(fp_test, "%d\t%d\n", i, kd.lmax_final[k][i]);
	// fclose(fp_test);
// }
// if (k == 246)
// {
	// // FOR TESTING: write lmax to file
	// FILE *fp_test = fopen("v5_246.txt", "w");
	// for (i = 0; i < (int32_t)kd.lmax_len_final[k]; ++i)
		// fprintf(fp_test, "%d\t%d\n", i, kd.lmax_final[k][i]);
	// fclose(fp_test);
// }
			// save lmax
			sprintf(buf, "./data5/k%d_Max_%04d.dat", kd.k_val, k);
			if (all_zeros(kd.lmax_final[k], kd.lmax_len_final[k]))
			{
				printf("ALL ZEROS MAX: k = %d\n", k);
				FILE * fp3 = fopen(buf, "w");
				fclose(fp3);
			}
			else
			{
				dacMax = createFT((uint32_t *)kd.lmax_final[k], kd.lmax_len_final[k]);
				saveFT(dacMax, buf);
				lmax_size = get_file_size(buf);
				destroyFT(dacMax);
			}
			
			// save lmin
			sprintf(buf, "./data5/k%d_Min_%04d.dat", kd.k_val, k);
			if (all_zeros(kd.lmin_final[k], kd.lmin_len_final[k]))
			{
				printf("ALL ZEROS MIN: k = %d\n", k);
				FILE * fp3 = fopen(buf, "w");
				fclose(fp3);
			}
			else
			{
				dacMin = createFT((uint32_t *)kd.lmin_final[k], kd.lmin_len_final[k]);
				saveFT(dacMin, buf);
				lmin_size = get_file_size(buf);
				destroyFT(dacMin);
			}
			
			total_lmax_size += lmax_size;
			total_lmin_size += lmin_size;
		}
		fclose(fp_irreg_write);

		sprintf(buf, "./data5/k%d_data.dat", kd.k_val);
		total_t_size = get_file_size(buf);
		printf("**** First Size Print:\tk_val = %d\t%ld\t%ld\t%ld\t%ld\n", kd.k_val,   
		                                    total_t_size, total_lmax_size, total_lmin_size,
											total_t_size + total_lmax_size + total_lmin_size);
// end saving to files
		
// begin reloading from files
		uint32_t nz_local;
		uint32_t tau_local; 
		
		sprintf(buf, "./data5/k%d_data.dat", kd.k_val);
printf("**A\n");
		FILE *fp_irreg_read = fopen(buf, "r");		
		if (fp_irreg_read == NULL)
		{
			fprintf(stderr, "Error: Cannot open file.\n");
			return (EXIT_FAILURE);		
		}	
printf("**B\n");		
		fread_result = fread(&nz_local, sizeof(uint32_t), 1, fp_irreg_read);  if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		
printf("**C\n");
		fread_result = fread(&tau_local, sizeof(uint32_t), 1, fp_irreg_read); if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
printf("**D\n");

		// uint32_t sB_packed_count_local = (uint32_t)CEILING_INT(nz_local, WORD_SIZE);
		// uint32_t *sB_packed_local             = (uint32_t *)calloc(sB_packed_count_local, sizeof(uint32_t));
		// fread_result = fread(sB_packed_local, sizeof(uint32_t), sB_packed_count_local, fp_irreg_read); if (fread_result != sB_packed_count_local) {fputs ("Reading error", stderr); return (EXIT_FAILURE);} 

		uint32_t sB_packed_count_local = (uint32_t)CEILING_INT(nz_local, WORD_SIZE);
		sB_data  *sB_unpacked_local    = (sB_data *)calloc(nz_local, sizeof(sB_data));
		uint32_t *sB_packed_local      = (uint32_t *)calloc(sB_packed_count_local, sizeof(uint32_t));
		fread_result = fread(sB_packed_local, sizeof(uint32_t), sB_packed_count_local, fp_irreg_read);
		if (fread_result != sB_packed_count_local) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		for (int32_t k = 0; k < (int32_t)nz_local; k++)
			sB_unpacked_local[k].snapshot = bitget(sB_packed_local, k);

		uint32_t *T_unpacked_count_local = (uint32_t *)calloc(nz_local, sizeof(uint32_t));
		uint32_t **T_packed_local        = (uint32_t **)calloc(nz_local, sizeof(uint32_t *));
		uint32_t *eqB_packed_count_local = (uint32_t *)calloc(nz_local, sizeof(uint32_t));
		uint32_t **eqB_packed_local      = (uint32_t **)calloc(nz_local, sizeof(uint32_t *));
		FTRep **ftrep_local              = (FTRep **)calloc(nz_local, sizeof(FTRep *));
		
		for (int32_t kk = 0; kk < (int32_t)CEILING_INT(nz_local, (row_div * col_div)); kk++)
		{
			k = reordered_vector[kk];
			fread_result = fread(&T_unpacked_count_local[k], sizeof(uint32_t), 1, fp_irreg_read);
			if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
			
			uint32_t T_packed_count_local = CEILING_INT(T_unpacked_count_local[k], WORD_SIZE);
			T_packed_local[k] = (uint32_t *)calloc(T_packed_count_local, sizeof(uint32_t));
			
			fread_result = fread(T_packed_local[k], sizeof(uint32_t), T_packed_count_local, fp_irreg_read);
			if (fread_result != T_packed_count_local) {fputs ("Reading error", stderr); return (EXIT_FAILURE);} 
			
			fread_result = fread(&eqB_packed_count_local[k], sizeof(uint32_t), 1, fp_irreg_read);
			if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
			
			eqB_packed_local[k] = (uint32_t *)calloc(eqB_packed_count_local[k], sizeof(uint32_t));
			fread_result = fread(eqB_packed_local[k], sizeof(uint32_t), eqB_packed_count_local[k], fp_irreg_read);
			if (fread_result != eqB_packed_count_local[k]) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
			
			sprintf(buf, "./data5/k%d_Max_%04d.dat", kd.k_val, k);		
			
			if (get_file_size(buf) != 0L)
				ftrep_local[k] = loadFT(buf);
			else
				ftrep_local[k] = NULL;
		}
		
		fclose(fp_irreg_read);
// end reloading from files

		int32_t sB_snapshot_count = 0;

		memset(&kd.sB_test[7], 0, sizeof(sB_data));
		//for (int32_t kk = 0; kk < (int32_t)kd.nz; kk++)
		for (int32_t kk = 0; kk < (int32_t)CEILING_INT(kd.nz, (row_div * col_div)); kk++)
		{
			
			if (kd.sB[k].snapshot == 1)
				sB_snapshot_count++;
			
			k = reordered_vector[kk];
			
			structure_add(&kd.sB_test[7], &kd.sB[k]);
			
			printf("Band=%d\tsnapshot=%d\tt_size_final=%d\tlmax[0]=%d\tlmax[1]=%d\teqB_p[0]=%d\teqB_p[1] = %d\teqB_p[2] = %d\teqB_p[2] = %d\t%d\n",
			                              k,
										  kd.sB[k].snapshot,
			                              kd.t_size_final[k],
										  kd.lmax_final[k][0],
										  kd.lmax_final[k][1],
										  kd.eqB_packed_final[k][0],
										  kd.eqB_packed_final[k][1],
										  kd.eqB_packed_final[k][2],
										  kd.eqB_packed_final[k][3],
										  kd.eqB_counter_final[k]);
									
			if (get_cell_checking)
			{
				printf("Inside get_cell_checking() - Irregular groupings\n");
				
				FILE *fp1 = fopen(command_line_file_name, "rb");
				
				fseek(fp1, orig_raster_size * sizeof(uint16_t) * k, SEEK_SET);
				if (orig_raster_size != fread(vector_old[3], sizeof(uint16_t), orig_raster_size, fp1))
				{
					fprintf(stderr, "\nError reading data file.\n");
					return (EXIT_FAILURE);
				}			
						
				fclose(fp1);
				
				for (j = 0; j < (int32_t) ny_old; ++j)
				{
					for (i = 0; i < (int32_t) nx_old; ++i)
					{
						if (kd.data_type == 2)
						{
							vector_new[3][j * kd.nx_new + i] = ntohs(vector_old[3][j * nx_old + i]);							
						}							
						else if (kd.data_type == 3)
						{
							vector_new[3][j * kd.nx_new + i] = (int32_t)ntohs(vector_old[3][j * nx_old + i]);
							//vector_new[3][j * kd.nx_new + i] = (int16_t)ntohs(vector_old[3][j * nx_old + i]);
						}							
					}
				}
				
				//if (bitget(sB_packed_local, k) == 1)
				//if (kd.sB[k].snapshot == 1)
				if (sB_unpacked_local[k].snapshot == 1)
				{
					int32_t found = 0;
					
					for (r = 0; r < (int32_t) kd.nx_new; r++) // row
					{
						for (c = 0; c < (int32_t) kd.nx_new; c++)     // column
						{
							int32_t max_val = accessFT(ftrep_local[k], 0);
							int32_t val = get_cell_s(&kd,
									 DACS,
									 kd.T_packed_final[k],
									 ftrep_local[k],
									 kd.t_size_final[k],
									 kd.nx_new,
									 r,
									 c,
									 0,
									 max_val,
									 0,
									 k);
							// int32_t val = get_cell_s(&kd,
									 // DACS,
									 // T_packed_local[k], // kd.T_packed_final[k], // T_packed_local[k],
									 // ftrep_local[k],
									 // T_unpacked_count_local[k],
									 // kd.nx_new,
									 // r,
									 // c,
									 // 0,
									 // max_val,
									 // 0,
									 // k);
							// int32_t val = get_cell_s(&kd,
									 // UNCOMPRESSED,
									 // kd.T_packed_final[k],
									 // kd.lmax_final[k],
									 // kd.t_size_final[k],
									 // kd.nx_new,
									 // r,
									 // c,
									 // 0,
									 // kd.lmax_final[k][0]);
							
							if (vector_new[3][r * kd.nx_new + c] != val)
							{
								fprintf(stderr, "Something went wrong. (3a)\tk=%d (s)\tr=%d\tc=%d\torig_val = %d\tget_cell_val = %d\n", k, r, c,
												vector_new[3][r * kd.nx_new + c], val);
								printf("Something went wrong. (3b)\tk=%d (s)\tr=%d\tc=%d\torig_val = %d\tget_cell_val = %d\n", k, r, c,
												vector_new[3][r * kd.nx_new + c], val);
								found = 1;
								break;
							}
						}
						
						if (found)
							break;
					}
				}
				else // (kd.sB[k].snapshot == 0)
				{
					//int32_t prev = get_prev_snapshot(kd.sB, k);
					//int32_t prev = get_prev_snapshot_random(bitget(sB_packed_local, k), reordered_vector, kd.nz, k, 0);
					//int32_t prev = get_prev_snapshot_random(kd.sB, reordered_vector, kd.nz, k, 0);
					int32_t prev = get_prev_snapshot_random(sB_unpacked_local, reordered_vector, kd.nz, k, 0);
					
					//int32_t found = 0;
					
					for (r = 0; r < (int32_t) kd.nx_new; r++) // row
					{
						for (c = 0; c < (int32_t) kd.nx_new; c++)     // column
						{
							int32_t max_val_s = accessFT(ftrep_local[prev], 0);
							int32_t max_val_t = accessFT(ftrep_local[k], 0);
							int32_t val = get_cell_t(
									&kd,
									DACS,
									kd.k_val,
									kd.T_packed_final[prev],
									kd.T_packed_final[k],
									kd.eqB_packed_final[k], //eqB_packed_local[k],
									ftrep_local[prev], // kd.lmax_final[prev], // ftrep_local[prev],
									ftrep_local[k],    // kd.lmax_final[k],    // ftrep_local[k],
									kd.t_size_final[prev],
									kd.t_size_final[k],
									kd.nx_new,
									r,
									c,
									0,
									0,
									max_val_s,
									max_val_t,
									0,
									k);
							// int32_t val = get_cell_t(
									// &kd,
									// DACS,
									// kd.k_val,
									// T_packed_local[prev],
									// T_packed_local[k],
									// eqB_packed_local[k], // kd.eqB_packed_final[k], //eqB_packed_local[k],
									// ftrep_local[prev], // kd.lmax_final[prev], // ftrep_local[prev],
									// ftrep_local[k],    // kd.lmax_final[k],    // ftrep_local[k],
									// T_unpacked_count_local[prev],
									// T_unpacked_count_local[k],
									// kd.nx_new,
									// r,
									// c,
									// 0,
									// 0,
									// max_val_s,
									// max_val_t,
									// 0,
									// k);
							// int32_t val = get_cell_t(
									// &kd,
									// DACS,
									// kd.k_val,
									// kd.T_packed_final[prev],
									// kd.T_packed_final[k],
									// kd.eqB_packed_final[k],
									// kd.lmax_final[prev],
									// kd.lmax_final[k],
									// kd.t_size_final[prev],
									// kd.t_size_final[k],
									// kd.nx_new,
									// r,
									// c,
									// 0,
									// 0,
									// kd.lmax_final[prev][0],
									// kd.lmax_final[k][0],
									// 0,
									// k);

							if (vector_new[3][r * kd.nx_new + c] != val)
							{
								// fprintf(stderr, "Something went wrong. (4a)\tk=%d (ns)\tr=%d\tc=%d\torig_val = %d\tget_cell_val = %d\n", k, r, c,
												// vector_new[3][r * kd.nx_new + c], val);
								printf("Something went wrong. (4b)\tk=%d (ns)\tr=%d\tc=%d\torig_val = %d\tget_cell_val = %d\n", k, r, c,
												vector_new[3][r * kd.nx_new + c], val);
								// found = 1;
								// break;
							}
						}
						
						// if (found)
							// break;
					}
				}
			}
		}	

		if (tk2r_build_type == 1)
			printf("Snapshot_percent:\t%d / %d = %0.2f%%\n", sB_snapshot_count, kd.nz, (double) sB_snapshot_count / (double)kd.nz * 100);
		
		// print grand total sizes
		uint32_t nz_byte_size = (uint32_t) CEILING_INT(kd.nz, WORD_SIZE) * sizeof(uint32_t); // account for sB array
		printf("****\tk_val = %d\t%ld\t%ld\t%ld\t%ld\n", kd.k_val,   
		                                    kd.sB_test[7].t_byte_size + nz_byte_size + 2 * sizeof(uint32_t), 
											kd.sB_test[7].lmax_byte_size, 
											kd.sB_test[7].lmin_byte_size, 
											kd.sB_test[7].total_byte_size + nz_byte_size + 2 * sizeof(uint32_t));


		// clean up 1
		for (k = 0; k < (int32_t)nz_local; ++k)
		{
			if (T_packed_local[k] != NULL)
				free(T_packed_local[k]);
			
			if (eqB_packed_local[k] != NULL)
				free(eqB_packed_local[k]);
			
			if (ftrep_local[k] != NULL)
				destroyFT(ftrep_local[k]);
		}
		if (sB_unpacked_local != NULL)
			free(sB_unpacked_local);
		if (sB_packed_local != NULL)
			free(sB_packed_local);
		if (T_unpacked_count_local != NULL)
			free(T_unpacked_count_local);
		if (eqB_packed_count_local != NULL)
			free(eqB_packed_count_local);
		if (T_packed_local != NULL)
			free(T_packed_local);
		if (eqB_packed_local != NULL)
			free(eqB_packed_local);	

		// clean up
		for (j = 0; j < (int32_t) kd.tau; j++)
		{		
			free(vector_new[j]);
			free(vector_old[j]);
		}
		
		free(vector_new);
		free(vector_old);
		//free_memory(&kd);
	}

	printf("Getting random numbers from files....\n");

	uint32_t *nx_random = (uint32_t *)calloc(NUM_OF_ITERATIONS, sizeof(uint32_t));
	uint32_t *ny_random = (uint32_t *)calloc(NUM_OF_ITERATIONS, sizeof(uint32_t));
	uint32_t *nz_random = (uint32_t *)calloc(NUM_OF_ITERATIONS, sizeof(uint32_t));

	// create_random_locations(nx_random,
							// ny_random,
							// nz_random,
							// kd.nx,
							// kd.ny,
							// kd.nz,
							// prog_file_name);

	sprintf(buf, "random_%s.dat", prog_file_name);
	FILE *fp_rand = fopen(buf, "r");
	for (i = 0; i < NUM_OF_ITERATIONS; i++)
	{
		fread_result = fread(&nx_random[i], sizeof(uint32_t), 1, fp_rand); if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		fread_result = fread(&ny_random[i], sizeof(uint32_t), 1, fp_rand); if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		fread_result = fread(&nz_random[i], sizeof(uint32_t), 1, fp_rand); if (fread_result != 1) {fputs ("Reading error", stderr); return (EXIT_FAILURE);}
		// if (i < 5)
			// printf("RANDOM OUTSIDE%d:\t%d\t%d\t%d\n", i, nx_random[i], ny_random[i], nz_random[i]);
	}
	fclose(fp_rand);

	if (tk2r_build_type == 0 && kd.tau == 1 && kd.encoder_type == DACS)
	{
		printf("Running test_get_cell...\n");
		test_get_cell(DACS,
					  &kd,
					  command_line_file_name,
					  prog_dir,
					  prog_file_name,
					  kd.k_val,
					  kd.data_type,
					  kd.nx,
					  kd.ny,
					  kd.nx_new,
					  nx_random,
					  ny_random,
					  nz_random);
	}
	
	if (tk2r_build_type == 1 && kd.encoder_type == DACS)
	{
		printf("Running test_get_cell_v2()...\n");
		test_get_cell_v2(DACS,
					 &kd,
					 reordered_vector,
					 command_line_file_name,
					 prog_dir,
					 prog_file_name,
					 kd.k_val,
					 kd.data_type,
					 kd.nx,
					 kd.ny,
					 kd.nx_new,
					 nx_random,
					 ny_random,
					 nz_random);
	}
	free(nx_random);
	free(ny_random);
	free(nz_random);

	free_memory(&kd);

    return (EXIT_SUCCESS); 
} 

