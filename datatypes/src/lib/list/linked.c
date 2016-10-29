#include "list/linked.h"

#include "list/error.h"

#include <stdlib.h>

struct list_implementation;
struct iterator_implementation;

struct list_node;

struct list_implementation {
	struct list_node * head;
	size_t length;
};

struct iterator_implementation {
	struct list_node ** last_node_pointer;
	// Messy, but required for iterator_insert and
	// iterator_previous to work efficiently and correctly.
	struct list_node * last_node;
};

struct list_node {
	void * data;
	struct list_node * next;
	struct list_node * previous;
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
/** Finds the node that comes in a later position.
 *
 *  Arguments:
 *    node: The node to start from.
 *    distance: The number of nodes to traverse.
 *
 *
 *  Returns:
 *    The node that is `distance` away.
 */
static struct list_node * node_at(struct list_node * node, size_t distance);

/** Gets the current node for the iterator.
 *
 *  Arguments:
 *    iterator: The iterator.
 *
 *  Returns:
 *    The node that is referenced.
 */
static struct list_node * iterator_node
	(const struct dt_list_iterator * iterator);

// List functions
struct dt_list * dt_list_linked_new(void)
{
	struct dt_list * list = NULL;
	list = malloc(sizeof(*list));
	if (!list) return NULL;
	struct list_implementation * implementation;
	implementation = malloc(sizeof(*implementation));
	if (!implementation){
		free(list);
		return NULL;
	}
	list->get = &list_get;
	list->insert = &list_insert;
	list->remove = &list_remove;
	list->length = &list_length;
	list->iterator = &list_iterator;
	list->del = &list_del;

	list->_data = implementation;

	implementation->head = NULL;
	implementation->length = 0;

	return list;
}

static void * list_get(const struct dt_list * this, size_t index)
{
	struct list_implementation * data = this->_data;
	if (this->length(this) <= index) return NULL;
	return node_at(data->head, index)->data;
}


static int list_insert(struct dt_list * this, size_t index, void * item)
{
	if (index > this->length(this)) return DT_LIST_EINDEX;

	struct list_node * node = NULL;
	node = malloc(sizeof(*node));
	if (!node) return DT_LIST_ENOMEM;

	node->data = item;

	struct list_implementation * data = this->_data;
	if (index > 0) {
		struct list_node * node_before = node_at(data->head, index - 1);
		struct list_node * node_after = node_before->next;
		
		node->next = node_after;
		node->previous = node_before;

		node_before->next = node;
		if (node_after) node_after->previous = node;
	} else {
		node->next = data->head;
		node->previous = NULL;
		if (data->head) data->head->previous = node;

		data->head = node;
	}

	data->length += 1;

	return 0;
}


static int list_remove(struct dt_list * this, size_t index)
{
	if (index > this->length(this)) return DT_LIST_EINDEX;

	struct list_node * node = NULL;

	struct list_implementation * data = this->_data;
	if (index > 0) {
		struct list_node * node_before = node_at(data->head, index - 1);
		node = node_before->next;
		struct list_node * node_after = node->next;
		
		node_before->next = node->next;
		
		if (node_after) node_after->previous = node->previous;
	} else {
		node = data->head;
		data->head = node->next;
		if(node->next) node->next->previous = NULL;
	}

	free(node);

	data->length -= 1;

	return 0;
}


static size_t list_length(const struct dt_list * this)
{
	struct list_implementation * data = this->_data;
	return data->length;
}


static struct dt_list_iterator * list_iterator(struct dt_list * this)
{
	struct list_implementation * data = this->_data;

	struct dt_list_iterator * iterator = NULL;
	iterator = malloc(sizeof(*iterator));
	if (!iterator) return NULL;
	struct iterator_implementation * implementation;
	implementation = malloc(sizeof(*implementation));
	if (!implementation){
		free(iterator);
		return NULL;
	}

	iterator->get = &iterator_get;
	iterator->valid = &iterator_valid;
	iterator->next = &iterator_next;
	iterator->previous = &iterator_previous;
	iterator->insert = &iterator_insert;
	iterator->remove = &iterator_remove;
	iterator->del = &iterator_del;
	
	iterator->list = this;
	iterator->position = 0;

	iterator->_data = implementation;

	implementation->last_node_pointer = &(data->head);
	implementation->last_node = NULL;

	return iterator;
}

static void list_del(struct dt_list * this)
{
	struct list_implementation * data = this->_data;

	struct list_node * node = data->head;

	while(node){
		void * del_me = node;
		node = node->next;
		free(del_me);
	}
	free(this->_data);
	free(this);
}

// Iterator functions
static void * iterator_get(const struct dt_list_iterator * this)
{
	struct list_node * current = iterator_node(this);
	if (current) return current->data;
	return NULL;
}

static int iterator_valid(const struct dt_list_iterator * this)
{
	struct list_node * current = iterator_node(this);
	if (current) return 1;
	return 0;
}

static int iterator_next(struct dt_list_iterator * this)
{
	struct iterator_implementation * data = this->_data;
	struct list_node * current = iterator_node(this);

	if (!current) { 
		return DT_LIST_EINDEX;
	}

	data->last_node_pointer = &(current->next);
	data->last_node = current;
	this->position += 1;

	if (iterator_node(this)) return 0;
	return DT_LIST_EINDEX;
}

static int iterator_previous(struct dt_list_iterator * this)
{
	// This function is weird to implement by design.
	// This linked list is primarily designed to make
	// it easy to become a singlely linked list.
	
	struct iterator_implementation * data = this->_data;
	struct list_implementation * list_data = this->list->_data;
	struct list_node * current = iterator_node(this);

	if (!current) {
		if (!data->last_node) return DT_LIST_EINDEX;
		data->last_node = data->last_node->previous;

		if (data->last_node) {
			data->last_node_pointer = &(data->last_node->next);
		} else {
			data->last_node_pointer = &(list_data->head);
		}
	} else if (!current->previous) {
		return DT_LIST_EINDEX;
	} else if (current->previous->previous) {
		data->last_node_pointer = &(current->previous->previous->next);
		data->last_node = current->previous->previous;
	} else {
		data->last_node_pointer = &(list_data->head);
		data->last_node = NULL;
	}
	
	this->position--;
	return 0;
}

static int iterator_insert(struct dt_list_iterator * this, void * item)
{
	struct iterator_implementation * data = this->_data;
	struct list_node * current = iterator_node(this);
	
	struct list_node * node = NULL;
	node = malloc(sizeof(*node));
	if (!node) return DT_LIST_ENOMEM;

	node->data = item;

	*(data->last_node_pointer) = node;
	node->next = current;
	node->previous = data->last_node;

	if(node->next) node->next->previous = node;


	struct list_implementation * list_data = this->list->_data;
	list_data->length += 1;

	return 0;
}

static int iterator_remove(struct dt_list_iterator * this)
{
	struct iterator_implementation * data = this->_data;
	struct list_node * current = iterator_node(this);
	
	if (!current) return DT_LIST_EINDEX;

	if (current->next) current->next->previous = current->previous;
	*(data->last_node_pointer) = current->next;

	struct list_implementation * list_data = this->list->_data;
	list_data->length -= 1;

	free(current);

	return 0;
}

static void iterator_del(struct dt_list_iterator * this)
{
	free(this->_data);
	free(this);
}

// Internal functions
static struct list_node * node_at(struct list_node * node, size_t distance)
{
	for (size_t i = distance; i > 0; i--) {
		node = node->next;
	}
	return node;
}

static struct list_node * iterator_node
	(const struct dt_list_iterator * iterator)
{
	struct iterator_implementation * data = iterator->_data;
	return *(data->last_node_pointer);
}

