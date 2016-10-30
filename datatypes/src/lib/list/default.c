#include "list.h"
#include "list/vector.h"


struct dt_list * dt_list_new(void)
{
	return dt_list_vector_new();
}

