
#include "stack/linked.h"
#include "stack/error.h"
#include <stdlib.h>

struct stack_implementation;
struct stack_node;

struct stack_implementation {
	struct stack_node * nodes;
	size_t length;
};

struct stack_node {
	void * value;
	struct stack_node * next;
};


static int stack_push(struct dt_stack * this, void * item);
static void * stack_pop(struct dt_stack * this);
static void * stack_peek(const struct dt_stack * this);
static size_t stack_length(const struct dt_stack * this);
static void stack_del(struct dt_stack * this);

struct dt_stack * dt_stack_linked_new(void)
{
	struct dt_stack * stack;
	stack = malloc(sizeof(*stack));

	if (!stack) return NULL;

	struct stack_implementation * implementation;
	implementation = malloc(sizeof(*implementation));

	if (!implementation) {
		free(stack);
		return NULL;
	}
	
	implementation->length = 0;
	implementation->nodes = NULL;

	stack->push = stack_push;
	stack->pop = stack_pop;
	stack->peek = stack_peek;
	stack->length = stack_length;
	stack->del = stack_del;
	stack->_data = implementation;

	return stack;
}


static int stack_push(struct dt_stack * this, void * item)
{
	struct stack_implementation * data = this->_data;
	struct stack_node * new_node;

	new_node = malloc(sizeof(*new_node));

	if (!new_node) return DT_STACK_ENOMEM;

	new_node->value = item;
	new_node->next = data->nodes;

	data->nodes = new_node;
	data->length++;

	return 0;
}

static void * stack_pop(struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;

	if (!data->nodes) return NULL;

	void * return_value = data->nodes->value;

	struct stack_node * del_node = data->nodes;

	data->nodes = del_node->next;
	free(del_node);

	data->length--;
	return return_value;

}

static void * stack_peek(const struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;
	if (!data->nodes) return NULL;

	return data->nodes->value;

}

static size_t stack_length(const struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;
	return data->length;
}

static void stack_del(struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;
	struct stack_node * last_node = data->nodes;

	while (last_node) {
		struct stack_node * del_node = last_node;
		last_node = last_node->next;

		free(del_node);
	}

	free(data);
	free(this);
}

