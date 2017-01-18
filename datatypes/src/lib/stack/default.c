#include "stack.h"
#include "stack/vector.h"


struct dt_stack * dt_stack_new(void)
{
	return dt_stack_vector_new();
}

