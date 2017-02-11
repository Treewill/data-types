#include "set.h"
#include "set/hash.h"

struct dt_set * dt_set_new(
	int (* comparator)(void * a, void * b),
	unsigned int (* hash)(void * item))
{
	return dt_set_hash_new(comparator, hash);
}
