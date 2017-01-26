#include "list/readonly.h"

#include <stdlib.h>

// List functions
static void * list_get(const struct dt_list * this, size_t index);
static size_t list_length(const struct dt_list * this);
static struct dt_list_iterator * list_iterator(struct dt_list * this);
static void list_del(struct dt_list * this);

// Iterator functions
static void * iterator_get(const struct dt_list_iterator * this);
static int iterator_valid(const struct dt_list_iterator * this);
static int iterator_next(struct dt_list_iterator * this);
static int iterator_previous(struct dt_list_iterator * this);
static void iterator_del(struct dt_list_iterator * this);

struct dt_list * dt_list_readonly_new(struct dt_list * list) {
	struct dt_list * read_list;
	read_list = malloc(sizeof(*read_list));

	if (!read_list) return NULL;

	read_list->get = &list_get;
	read_list->insert = NULL;
	read_list->remove = NULL;
	read_list->length = &list_length;
	read_list->iterator = &list_iterator;
	read_list->del = &list_del;
	read_list->_data = list;

	return read_list;
}

static void * list_get(const struct dt_list * this, size_t index)
{
	const struct dt_list * data = this->_data;
	return data->get(data, index);
}

static size_t list_length(const struct dt_list * this)
{
	const struct dt_list * data = this->_data;
	return data->length(data);
}

static struct dt_list_iterator * list_iterator(struct dt_list * this)
{
	struct dt_list * data = this->_data;

	struct dt_list_iterator * iterator;
	iterator = malloc(sizeof(*iterator));

	if (!iterator) return NULL;

	iterator->get = &iterator_get;
	iterator->valid = &iterator_valid;
	iterator->next = &iterator_next;
	iterator->previous = &iterator_previous;
	iterator->insert = NULL;
	iterator->remove = NULL;
	iterator->del = &iterator_del;

	iterator->position = 0;
	iterator->list = this;
	iterator->_data = data->iterator(data);

	if (!iterator->_data) {
		free(iterator);
		return NULL;
	}

	return iterator;
}

static void list_del(struct dt_list * this)
{
	free(this);
}


static void * iterator_get(const struct dt_list_iterator * this)
{
	const struct dt_list_iterator * data = this->_data;
	return data->get(data);
}

static int iterator_valid(const struct dt_list_iterator * this)
{
	const struct dt_list_iterator * data = this->_data;
	return data->valid(data);
}

static int iterator_next(struct dt_list_iterator * this)
{
	struct dt_list_iterator * data = this->_data;
	int next = data->next(data);
	this->position = data->position;
	return next;
}

static int iterator_previous(struct dt_list_iterator * this)
{
	struct dt_list_iterator * data = this->_data;
	int previous = data->previous(data);
	this->position = data->position;
	return previous;
}

static void iterator_del(struct dt_list_iterator * this)
{
	struct dt_list_iterator * data = this->_data;
	data->del(data);
	free(this);
}
