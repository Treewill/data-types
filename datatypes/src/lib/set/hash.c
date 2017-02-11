
#include "set/hash.h"
#include "set/error.h"

#include <stdbool.h>
#include <stdlib.h>

#include "buffers.h"
#include "list.h"
#include "set/tree.h"

#define BUCKET_SET dt_set_tree_new

// This must be a power of two.
//
// It could be implemented differently
// but hardware can take advantage of powers
// of two.
#define DEFAULT_BUCKETS_COUNT 16

struct set_implementation;
struct set_implementation {
	int (* comparator)(void * a, void * b);
	unsigned int (* hash)(void * item);
	struct dt_set * * buckets;
	size_t buckets_size;
	size_t item_count;
};

static int set_insert(struct dt_set * this, void * item);
static void * set_has(const struct dt_set * this, void * item);
static void set_remove(struct dt_set * this, void * item);
static struct dt_list * set_items(const struct dt_set * this);
static void set_del(struct dt_set * this);

// Grow if needed.
static void grow(struct dt_set * this);
static bool should_grow(struct set_implementation * data);
static size_t grow_to(struct set_implementation * data);

/** Pulls up the appropriate bucket (creating if
 *  requested and required).
 *
 *  Arguments:
 *    data: The hash set implementation.
 *    item: The item to find a bucket for.
 *    create: If true a bucket will be created if needed.
 *
 *  Returns:
 *    The bucket if available, null otherwise.
 *    If create is true and null is returned
 *    then there's not enough memory available.
 */
static struct dt_set * get_bucket_set(
	struct set_implementation * data,
	void * item,
	bool create);

// Remaps the hash to another hash.
// Used to avoid poor hash results.
static long universal_hash_a = 1188764207l;
static long universal_hash_b = 3227431836l;
static long const universal_hash_prime = 4294967311l;
static unsigned int universal_hash(unsigned int hash);

struct dt_set * dt_set_hash_new(
	int (* comparator)(void * a, void * b),
	unsigned int (* hash)(void * item))
{
	struct dt_set * set;
	set = malloc(sizeof(*set));

	if (!set) return NULL;

	struct set_implementation * implementation;
	implementation = malloc(sizeof(*implementation));

	if (!implementation) {
		free(set);
		return NULL;
	}

	struct dt_set * * buckets;
	size_t buckets_size = ARRAY_SIZE(buckets, DEFAULT_BUCKETS_COUNT);
	buckets = malloc(buckets_size);
	if (!buckets) {
		free(implementation);
		free(set);
		return NULL;
	}
	for (size_t i = 0; i < ARRAY_LENGTH(buckets, buckets_size); i++) {
		buckets[i] = NULL;
	}

	set->insert = &set_insert;
	set->has = &set_has;
	set->remove = &set_remove;
	set->items = &set_items;
	set->del = &set_del;
	set->_data = implementation;

	implementation->comparator = comparator;
	implementation->hash = hash;
	implementation->buckets = buckets;
	implementation->buckets_size = buckets_size;
	implementation->item_count = 0;

	return set;
}


static int set_insert(struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;

	if (should_grow(data)) grow(this);

	struct dt_set * bucket_set;
	bucket_set = get_bucket_set(data, item, true);

	if (!bucket_set) return DT_SET_ENOMEM;

	int return_value = bucket_set->insert(bucket_set, item);

	if (return_value) return return_value;

	data->item_count++;
	return 0;

}

static void * set_has(const struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;

	struct dt_set * bucket_set;
	bucket_set = get_bucket_set(data, item, false);

	if (!bucket_set) return NULL;
	return bucket_set->has(bucket_set, item);
}

static void set_remove(struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;

	struct dt_set * bucket_set;
	bucket_set = get_bucket_set(data, item, false);

	if (bucket_set && bucket_set->has(bucket_set, item)) {
		data->item_count--;
		bucket_set->remove(bucket_set, item);
	}
}

static struct dt_list * set_items(const struct dt_set * this)
{
	struct set_implementation * data = this->_data;

	struct dt_list * list = dt_list_new();
	if (!list) return NULL;

	for (size_t i = 0; i < ARRAY_LENGTH(data->buckets, data->buckets_size); i++) {
		struct dt_set * bucket_set;
		bucket_set = data->buckets[i];
		if (!bucket_set) continue;

		struct dt_list * bucket_list;
		bucket_list = bucket_set->items(bucket_set);

		if (!bucket_list) {
			list->del(list);
			return NULL;
		}

		struct dt_list_iterator * iter;
		iter = bucket_list->iterator(bucket_list);

		if (!iter) {
			bucket_list->del(bucket_list);
			list->del(list);
			return NULL;
		}

		for (; iter->valid(iter); iter->next(iter)) {
			list->insert(list, list->length(list), iter->get(iter));
		}
		iter->del(iter);
		bucket_list->del(bucket_list);
	}

	return list;
}

static void set_del(struct dt_set * this)
{
	struct set_implementation * data = this->_data;
	for (size_t i = 0; i < ARRAY_LENGTH(data->buckets, data->buckets_size); i++) {
		struct dt_set * bucket_set;
		bucket_set = data->buckets[i];
		if (!bucket_set) continue;
		bucket_set->del(bucket_set);
	}
	free(data->buckets);
	free(data);
	free(this);
}

static void grow(struct dt_set * this)
{
	struct set_implementation * data = this->_data;

	// Grow buckets if a set is too large.

	size_t new_size = grow_to(data);
	if (new_size <= data->buckets_size) return;

	struct dt_list * items = this->items(this);
	if (!items) return;

	struct dt_list_iterator * iter;
	iter = items->iterator(items);
	if (!iter) {
		items->del(items);
		return;
	}

	struct dt_set * * new_buckets;
	new_buckets = realloc(data->buckets, new_size);
	if (!new_buckets) {
		iter->del(iter);
		items->del(items);
		return;
	}
	data->buckets = new_buckets;

	for (size_t i = 0; i < ARRAY_LENGTH(data->buckets, data->buckets_size); i++) {
		struct dt_set * bucket = data->buckets[i];
		if (bucket) bucket->del(bucket);
		data->buckets[i] = NULL;
	}
	data->buckets_size = new_size;

	for (; iter->valid(iter); iter->next(iter)) {
		// If we run out of memory now ...
		// There's no real way to recover.
		this->insert(this, iter->get(iter));
	}
}

static bool should_grow(struct set_implementation * data)
{
	size_t length = ARRAY_LENGTH(data->buckets, data->buckets_size);

	if (length * length < length) return false;
	if (data->item_count < length * length) return false;
	return true;
}

static size_t grow_to(struct set_implementation * data)
{
	return data->buckets_size * 2;
}

static struct dt_set * get_bucket_set(
	struct set_implementation * data,
	void * item,
	bool create)
{
	unsigned int hash = data->hash(item);
	hash = universal_hash(hash);
	hash = hash & (ARRAY_LENGTH(data->buckets, data->buckets_size) - 1);

	struct dt_set * bucket_set;
	bucket_set = data->buckets[hash];

	if (!bucket_set && create) {
		bucket_set = BUCKET_SET(data->comparator, data->hash);
		data->buckets[hash] = bucket_set;
	}

	return bucket_set;
}

static unsigned int universal_hash(unsigned int hash)
{
	unsigned long a = universal_hash_a;
	unsigned long b = universal_hash_b;
	unsigned long p = universal_hash_prime;

	unsigned long bits = ((unsigned int) ~0);
	unsigned int shift = sizeof(unsigned int) * 8;

	// Computes (a * hash + b) % p without ever overflowing.

	return
		((((a >> shift) * ((1 << shift) * hash % p)) % p +
		(a & bits) % p) + b) % p;
}
