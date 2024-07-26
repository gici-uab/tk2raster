#define UNCOMPRESSED			0
#define DACS					1
#define SIMPLE9_SAMPLING    	2
#define SIMPLE16_SAMPLING   	3
#define PFD32			        4
#define PFD64			        5
#define PFD128			        6
#define PFD256			        7
#define SIMPLE9             	8
#define SIMPLE16            	9
#define SIMPLE16_PARTIAL_SUMS	10

#define REORDER_UNCHANGED		0
#define REORDER_RANDOM			1
#define REORDER_AVERAGE			2
#define REORDER_ENTROPY			3
#define REORDER_MAX				4
#define REORDER_MIN				5
#define REORDER_DIFFERENCE		6
#define REORDER_NON_ZERO_DATA	7
#define REORDER_ZERO_DATA		8

#define MASK_31 				0x0000001F

typedef struct _child
{
	int32_t maxval_s;
	int32_t maxval_t;
	int32_t minval_s;
	int32_t minval_t;
	int32_t diff;
	
} child;

typedef struct _maxmin
{
	int32_t	max;
	int32_t min;
} maxmin;

typedef struct _sB_data
{
	uint8_t 	snapshot;
	uint64_t	t_byte_size;
	uint64_t	lmax_byte_size;
	uint64_t	lmin_byte_size;
	uint64_t	total_byte_size;
} sB_data;

typedef struct _reorder
{
	uint32_t	index;
	double		data;
} reorder;

typedef struct _kdata 
{
	int32_t     td; // Tree Depth = Tree Height + 1
	uint32_t    nx;
	uint32_t    ny;
	uint32_t 	nz;
	uint8_t     data_type;
	int32_t		encoder_type;
	uint32_t	num_elements;
	uint32_t    t_size_packed;
	uint8_t		reorder_type;
	int32_t 	**lmax;  // ok
	int32_t 	**lmin;  // ok
	int32_t 	***vmax; // ok
	int32_t 	***vmin; // ok
	uint8_t     ***t;    // ok
	uint8_t		***eqB;  // ok
	uint32_t 	**vmax_count; // ok
	uint32_t 	**vmin_count; // ok
	uint32_t    **t_count;    // ok
	uint32_t	**eqB_count;  // ok
	maxmin 		*rMaxMin;     // ok
	uint32_t	**T_packed; // ok
	uint32_t    **eqB_packed; // ok
	uint32_t    *t_size; // ok
	uint32_t	nx_new; //
	uint16_t    k_val; //
	uint32_t	tau; //
	sB_data     *sB;
	sB_data     sB_test[10];
	uint32_t	*lmax_len;
	uint32_t	*lmin_len;
	uint32_t	*T_counter;
	uint32_t    *eqB_counter;
	
	int32_t     **lmax_final;       // for get_cell_checking
	int32_t     **lmin_final;       // for get_cell_checking
	uint32_t    *lmax_len_final;    // for get_cell_checking
	uint32_t    *lmin_len_final;    // for get_cell_checking
	uint32_t    **T_packed_final;   // for get_cell_checking // packed size
	uint32_t    **eqB_packed_final; // for get_cell_checking
	uint32_t    *t_size_final;      // for get_cell_checking // unpacked size
	uint32_t    *eqB_counter_final; // for get_cell_checking // packed size
	
	int32_t     *lmax_temp[2];     // for get_cell_checking
	int32_t     *lmin_temp[2];     // for get_cell_checking
	int32_t     lmax_len_temp[2];  // for get_cell_checking
	int32_t     lmin_len_temp[2];  // for get_cell_checking
	uint32_t    *T_temp[2];        // for get_cell_checking
	uint32_t    t_size_temp[2];    // for get_cell_checking
	reorder		*reordered_bands;
	
	int32_t     **vector_new_2;
	int32_t		*average_array;
} kdata;

typedef struct _build_ret
{
	int32_t		s_max; // snapshot
	int32_t		t_max; // relative to snapshot
	int32_t		s_min; // snapshot
	int32_t		t_min; // relative to snapshot
	int32_t		diff;
} build_ret;

maxmin build_s16(uint32_t k_val,
                 int16_t *vec,
                 uint32_t nx_new,
                 uint32_t n,
                 uint32_t l,
                 uint32_t r,
                 uint32_t c,
				 kdata  *pkdata,
				 uint32_t k_interval);
				 
int32_t initialize(uint32_t k_val,
               uint32_t   td,
			   uint32_t *t_size,
			   int32_t  *T_size,
			   uint32_t *num_elements,
			   uint32_t nz,
			   kdata  *pkdata,
			   uint16_t interval);
			   
void free_memory(int32_t   td, 
				 uint32_t  nz,
			     kdata   *pkdata,
			     uint16_t  interval);


int32_t M[3][64] =
{
	{
	9, 8, 7, 7, 6, 6, 3, 2,
	7, 7, 7, 7, 6, 6, 3, 3,
	6, 6, 6, 6, 3, 3, 3, 3,
	5, 5, 6, 6, 3, 3, 3, 3,
	4, 5, 5, 5, 4, 4, 4, 4,
	3, 3, 5, 5, 4, 4, 4, 4,
	3, 3, 3, 5, 4, 4, 4, 4,
	4, 4, 3, 4, 4, 4, 4, 4
	},
	{
	9, 8, 7, 7, 7, 7, 4, 4,
	7, 7, 7, 7, 7, 7, 4, 4,
	6, 6, 6, 6, 4, 3, 3, 3,
	5, 5, 6, 6, 3, 3, 3, 3,
	4, 5, 5, 5, 4, 4, 4, 4,
	3, 3, 5, 5, 4, 4, 4, 4,
	3, 3, 4, 5, 5, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,		
	},
	{
	9, 8, 7, 7, 8, 7, 5, 5,
	7, 7, 7, 7, 7, 7, 5, 5,
	7, 7, 6, 6, 4, 3, 4, 4,
	6, 6, 6, 6, 4, 4, 4, 4,
	4, 5, 5, 5, 4, 4, 4, 4,
	3, 3, 5, 5, 4, 4, 4, 4,
	3, 3, 4, 5, 6, 4, 4, 4,
	4, 4, 4, 4, 5, 4, 4, 4,		
	}
};