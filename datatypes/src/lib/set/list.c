
#include "set/list.h"
#include "set/error.h"

#include <stdlib.h>

#include "list.h"
#include "list/error.h"
#include "list/readonly.h"

struct set_implementation;
struct set_implementation {
	int (* comparator)(void * a, void * b);
	struct dt_list * list;
};

static int set_insert(struct dt_set * this, void * item);
static void * set_has(const struct dt_set * this, void * item);
static void set_remove(struct dt_set * this, void * item);
static struct dt_list * set_items(const struct dt_set * this);
static void set_del(struct dt_set * this);

/** Finds the index to insert the item at
 *  in the list.
 *
 *  Arguments:
 *    list: The list to look through.
 *    item: The item to find the index for.
 *    comparator: An ordering function for the items
 *                in the list.
 *      Arguments:
 *        a: The first item.
 *        b: The second item.
 *
 *      Returns:
 *        0 if a is logically equal to b.
 *        -1 if a comes before b.
 *        1 if a comes after b.
 *
 *    found: A result variable. True if the item was actually found.
 *
 *    Returns:
 *      The index to insert the item at.
 */
static size_t find_index(
	const struct dt_list * list,
	void * item,
	int (* comparator)(void * a, void * b),
	bool * found);

struct dt_set * dt_set_list_new(
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

	struct dt_list * list = dt_list_new();
	if (!list) {
		free(implementation);
		free(set);
		return NULL;
	}

	set->insert = &set_insert;
	set->has = &set_has;
	set->remove = &set_remove;
	set->items = &set_items;
	set->del = &set_del;
	set->_data = implementation;

	implementation->comparator = comparator;
	implementation->list = list;
	return set;
}

static int set_insert(struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;

	size_t index;
	bool already_have;
	index = find_index(data->list, item, data->comparator, &already_have);

	if (already_have) return 0;

	int result = data->list->insert(data->list, index, item);
	if (result == DT_LIST_ENOMEM) {
		return DT_SET_ENOMEM;
	} else if (result) {
		return DT_SET_ERROR;
	}
	return 0;
}

static void * set_has(const struct dt_set * this, void * item)
{
	const struct set_implementation * data = this->_data;
	size_t index;
	bool has;
	index = find_index(data->list, item, data->comparator, &has);

	if (has) return data->list->get(data->list, index);
	return NULL;
}

static void set_remove(struct dt_set * this, void * item)
{
	struct set_implementation * data = this->_data;

	size_t index;
	bool has;
	index = find_index(data->list, item, data->comparator, &has);

	if (has) data->list->remove(data->list, index);
}

static struct dt_list * set_items(const struct dt_set * this)
{
	const struct set_implementation * data = this->_data;
	return dt_list_readonly_new(data->list);

}

static void set_del(struct dt_set * this)
{
	struct set_implementation * data = this->_data;
	data->list->del(data->list);
	free(data);
	free(this);
}

static size_t find_index(
	const struct dt_list * list,
	void * item,
	int (* comparator)(void * a, void * b),
	bool * found)
{
	*found = false;

	size_t begin, end;
	begin = 0;
	end = list->length(list);

	while  (begin != end) {
		size_t middle = (begin + end) / 2;
		void * other = list->get(list, middle);
		int compare = comparator(item, other);
		if (compare == 0) {
			* found = true;
			return middle;
		} else if (compare > 0) {
			begin = middle + 1;
		} else {
			// Could be more precise with a check and
			// a -1 but there's really no need since
			// middle != end.
			end = middle;
		}
	}
	return begin;
}
