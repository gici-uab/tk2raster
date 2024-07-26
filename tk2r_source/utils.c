#include "utils.h"

// int32_t makehist(int32_t *S, int32_t *hist, uint32_t len)
// {
	// int32_t  wherechar[64*1024];
	// uint32_t i;
	// uint32_t histlen;
	// histlen = 0;
	// for (i = 0; i < (64*1024); i++)
		// wherechar[i] = -1;
	// for (i = 0; i < len; i++)
	// {
		// if (wherechar[(uint32_t)S[i]] == -1)
		// {
			// wherechar[(uint32_t)S[i]] = histlen;
			// histlen++;
		// }
		// hist[wherechar[(uint32_t)S[i]]]++;
	// }
	// return histlen;
// }

int32_t makehist(int32_t *S, int32_t *hist, uint32_t len)
{
	int32_t  wherechar[64*1024];
	uint32_t i;
	uint32_t histlen;
	histlen = 0;
	for (i = 0; i < (64*1024); i++)
		wherechar[i] = -1;
	for (i = 0; i < len; i++)
	{
		if (wherechar[(uint16_t)S[i]] == -1)
		{
			wherechar[(uint16_t)S[i]] = histlen;
			histlen++;
		}
		hist[wherechar[(uint16_t)S[i]]]++;
	}
	return histlen;
}

double entropy(int32_t *hist, uint32_t histlen, uint32_t len)
{
	uint32_t i = 0;
	double   H = 0;
	double   p = 0;
	for (i = 0; i < histlen; i++)
	{
		p = (double)hist[i] / len;
		H -= p * log2(p);
	}
	return H;
}
