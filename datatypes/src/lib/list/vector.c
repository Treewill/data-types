#include "list/vector.h"

#include "list/error.h"

#include <stdlib.h>

#define ARRAY_LENGTH(array, size) (size / sizeof(*array))
#define ARRAY_SIZE(array, length) (length * sizeof(*array))

struct list_implementation;

struct list_implementation {
	void ** buffer;
	size_t buffer_size;
	size_t length;
};



// List functions
static void * list_get(const struct dt_list * this, size_t index);
static int list_insert(struct dt_list * this, size_t index, void * item);
static int list_remove(struct dt_list * this, size_t index);
static size_t list_length(const struct dt_list * this);
static struct dt_list_iterator * list_iterator(struct dt_list * this);
static void list_del(struct dt_list * this);

// Iterator functions
static void * iterator_get(const struct dt_list_iterator * this);
static int iterator_valid(const struct dt_list_iterator * this);
static int iterator_next(struct dt_list_iterator * this);
static int iterator_previous(struct dt_list_iterator * this);
static int iterator_insert(struct dt_list_iterator * this, void * item);
static int iterator_remove(struct dt_list_iterator * this);
static void iterator_del(struct dt_list_iterator * this);

// Internal functions
/** Shifts elements right starting at start.
 *
 *  Arguments:
 *    list: The list to shift the elements of.
 *    start: The index to start at.
 */
static void shift_right(struct dt_list * list, size_t start);

/** Shifts elements left starting at start.
 *
 *  Arguments:
 *    list: The list to shift the elements of.
 *    start: The index to start at.
 */
static void shift_left(struct dt_list * list, size_t start);

struct dt_list * dt_list_vector_new(void) {
	struct dt_list * list;
	list = malloc(sizeof(*list));

	if (!list) return NULL;

	struct list_implementation * implementation;
	implementation = malloc(sizeof(*implementation));

	if (!implementation) {
		free(list);
		return NULL;
	}
	
	implementation->buffer_size = 
		ARRAY_SIZE(implementation->buffer, 8);
	implementation->length = 0;
	implementation->buffer = malloc(implementation->buffer_size);

	if (!implementation->buffer) {
		free(implementation);
		free(list);
		return NULL;
	}

	list->_data = implementation;
	
	list->get = &list_get;
	list->insert = &list_insert;
	list->remove = &list_remove;
	list->length = &list_length;
	list->iterator = &list_iterator;
	list->del = &list_del;

	return list;
}

static void * list_get(const struct dt_list * this, size_t index)
{
	struct list_implementation * data = this->_data;
	if (index < this->length(this)) return data->buffer[index];
	return NULL;
}

static int list_insert(struct dt_list * this, size_t index, void * item)
{
	struct list_implementation * data = this->_data;
	if (index > this->length(this)) return DT_LIST_EINDEX;

	size_t buffer_length =
		ARRAY_LENGTH(data->buffer, data->buffer_size);
	
	if (buffer_length <= this->length(this)) {

		size_t new_size = data->buffer_size * 2;

		if (new_size < data->buffer_size) {
			// Overflow
			return DT_LIST_ENOMEM;
		}

		void ** new_buf = realloc(data->buffer, new_size);
		if (!new_buf) return DT_LIST_ENOMEM;

		data->buffer = new_buf;
		data->buffer_size = new_size;

	}

	shift_right(this, index);
	data->buffer[index] = item;

	data->length++;
	return 0;
}

static int list_remove(struct dt_list * this, size_t index)
{
	struct list_implementation * data = this->_data;
	if (index >= this->length(this)) return DT_LIST_EINDEX;

	shift_left(this, index);
	data->length--;

	size_t buffer_length =
		ARRAY_LENGTH(data->buffer, data->buffer_size);
	
	if (buffer_length / 4 > this->length(this)) {

		size_t new_size = data->buffer_size / 2;
		void ** new_buf = realloc(data->buffer, new_size);

		if (new_buf) {
			data->buffer = new_buf;
			data->buffer_size = new_size;
		}
	}

	return 0;
}

static size_t list_length(const struct dt_list * this)
{
	struct list_implementation * data = this->_data;
	return data->length;
}

static struct dt_list_iterator * list_iterator(struct dt_list * this)
{
	struct dt_list_iterator * iterator;
	iterator = malloc(sizeof(*iterator));

	if (!iterator) return NULL;

	iterator->get = &iterator_get;
	iterator->valid = &iterator_valid;
	iterator->next = &iterator_next;
	iterator->previous = &iterator_previous;
	iterator->insert = &iterator_insert;
	iterator->remove = &iterator_remove;
	iterator->del = &iterator_del;

	iterator->position = 0;
	iterator->list = this;
	iterator->_data = NULL;
	return iterator;
}

static void list_del(struct dt_list * this)
{
	
	struct list_implementation * data = this->_data;
	free(data->buffer);
	free(data);
	free(this);
}


static void * iterator_get(const struct dt_list_iterator * this)
{
	return this->list->get(this->list, this->position);
}

static int iterator_valid(const struct dt_list_iterator * this)
{
	return this->position < this->list->length(this->list);
}

static int iterator_next(struct dt_list_iterator * this)
{
	if (this->position >= this->list->length(this->list))
		return DT_LIST_EINDEX;
	
	this->position++;

	if (this->position == this->list->length(this->list))
		return DT_LIST_EINDEX;
	return 0;
}

static int iterator_previous(struct dt_list_iterator * this)
{
	if (this->position <= 0) return DT_LIST_EINDEX;
	this->position--;
	return 0;
}

static int iterator_insert(struct dt_list_iterator * this, void * item)
{
	return this->list->insert(this->list, this->position, item);
}

static int iterator_remove(struct dt_list_iterator * this)
{
	return this->list->remove(this->list, this->position);
}

static void iterator_del(struct dt_list_iterator * this)
{
	free(this);
}

static void shift_right(struct dt_list * list, size_t start)
{
	struct list_implementation * list_data = list->_data;
	size_t length = list->length(list);
	void ** buffer = list_data->buffer + start;
	void ** end = list_data->buffer + length;
	for (; buffer < end; end--) {
		end[0] = end[-1];
	}
}

static void shift_left(struct dt_list * list, size_t start)
{
	struct list_implementation * list_data = list->_data;
	size_t length = list->length(list);
	void ** buffer = list_data->buffer + start;
	void ** end = list_data->buffer + length - 1;
	for (; buffer < end; buffer++) {
		buffer[0] = buffer[1];
	}
	
}
