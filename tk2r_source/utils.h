#include <stdint.h>
#include <math.h>

int32_t makehist(int32_t *S, int32_t *hist, uint32_t len);
double  entropy(int32_t *hist, uint32_t histlen, uint32_t len);
