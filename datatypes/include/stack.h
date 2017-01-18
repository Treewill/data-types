#ifndef __STACK_H__
#define __STACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

struct dt_stack;


/** A stack interface.
 */

struct dt_stack {

	/** Puts an item on top of the stack.
	 *
	 *  Arguments:
	 *    this_: This stack.
	 *    item: The item to put on the stack.
	 *  Returns:
	 *    Zero on success a negative number otherwise.
	 */
	int (* push)(struct dt_stack * this_, void * item);

	/** Removes an item from the top of the stack.
	 *
	 *  Arguments:
	 *    this_: This stack.
	 *
	 *  Returns:
	 *    The item on top, if it exists. Otherwise NULL.
	 */
	void * (* pop)(struct dt_stack * this_);

	/** Shows the top item without removing it.
	 *
	 *  Arguments:
	 *    this_: This stack.
	 *
	 *  Returns:
	 *    The item on top, if it exists. Otherwise NULL.
	 */
	void * (* peek)(const struct dt_stack * this_);

	/** The length of the stack.
	 *
	 *  Arguments:
	 *    this_: This stack.
	 *
	 *  Returns:
	 *    The length of the stack.
	 */
	size_t (* length)(const struct dt_stack * this_);

	/** Deletes this stack.
	 *
	 *  Arguments:
	 *    this_: This stack.
	 */
	void (* del)(struct dt_stack * this_);

	/** Internal state.
	 */
	void * _data;
};

/** Creates a new stack.
 *
 *  This should create the stack you will
 *  most likely want to use.
 *
 *  Returns:
 *    A stack.
 */
struct dt_stack * dt_stack_new(void);

#ifdef __cplusplus
}
#endif

#endif // __STACK_H_

