
#include "stack/vector.h"
#include "stack/error.h"
#include <stdlib.h>

#define ARRAY_LENGTH(array, size) (size / sizeof(*array))
#define ARRAY_SIZE(array, length) (length * sizeof(*array))

struct stack_implementation;
struct stack_implementation {
	void ** buffer;
	size_t buffer_size;
	size_t length;
};

static int stack_push(struct dt_stack * this, void * item);
static void * stack_pop(struct dt_stack * this);
static void * stack_peek(const struct dt_stack * this);
static size_t stack_length(const struct dt_stack * this);
static void stack_del(struct dt_stack * this);

struct dt_stack * dt_stack_vector_new(void)
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

	implementation->buffer_size = ARRAY_SIZE(implementation->buffer, 8);
	implementation->length = 0;
	implementation->buffer = malloc(implementation->buffer_size);

	if (!implementation->buffer) {
		free(implementation);
		free(stack);
		return NULL;
	}

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
	size_t buffer_length = 
		ARRAY_LENGTH(data->buffer, data->buffer_size);

	if (buffer_length <= this->length(this)) {
		size_t new_size = data->buffer_size * 2;
		
		if (new_size < data->buffer_size) {
			//Overflow
			return DT_STACK_ENOMEM;
		}

                void ** new_buf = realloc(data->buffer, new_size);
                if (!new_buf) return DT_STACK_ENOMEM;

                data->buffer = new_buf;
                data->buffer_size = new_size;
	}
	
	data->buffer[this->length(this)] = item;
	data->length++;
	return 0;
}

static void * stack_pop(struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;
	if (this->length(this) == 0) return NULL;

	data->length--;
	void * return_value = data->buffer[this->length(this)];

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
		
	return return_value;
}

static void * stack_peek(const struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;
	if (this->length(this) == 0) return NULL;
	return data->buffer[this->length(this) - 1];
}

static size_t stack_length(const struct dt_stack * this)
{
	const struct stack_implementation * data = this->_data;
	return data->length;
}

static void stack_del(struct dt_stack * this)
{
	struct stack_implementation * data = this->_data;
	free(data->buffer);
	free(data);
	free(this);
}

