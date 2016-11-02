#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

struct dt_list;
struct dt_list_iterator;


/** A List Interface.
 */

struct dt_list {

	/** Gets the element at the given index.
	 *
	 *  Arguments:
	 *    this_: This list.
	 *    index: The index to get [0, length).
	 *  Returns:
	 *    The pointer if such an element exists NULL otherwise.
	 */
	void * (* get)(const struct dt_list * this_, size_t index);

	/** Inserts the item at the given index.
	 *  
	 *  Arguments:
	 *    this_: This list.
	 *    index: The index to insert at [0, length].
	 *    item: The item to insert.
	 *
	 *  Returns:
	 *    Zero on success. A negative number otherwise.
	 *
	 *  Notes:
	 *    Invalidates all iterators.
	 */
	int (* insert)(struct dt_list * this_, size_t index, void * item);

	/** Removes an item at the given index.
	 *
	 *  Arguments:
	 *    this_: This list.
	 *    index: The index to remove at [0, length).
	 *
	 *  Returns:
	 *    Zero on success. A negative number otherwise.
	 *
	 *  Notes:
	 *    Invalidates all iterators.
	 */
	int (* remove)(struct dt_list * this_, size_t index);

	/** Gets the length of the list.
	 *
	 *  Arguments:
	 *    this_: This list.
	 *
	 *  Returns:
	 *    The length of the list.
	 */
	size_t (* length)(const struct dt_list * this_);

	/** Creates a new list iterator.
	 *
	 *  Arguments:
	 *    this_: This list.
	 *
	 *  Returns:
	 *    A new list iterator which must be deleted when done.
	 *
	 *  Notes:
	 *    Iterators may be invalidated by other calls made on
	 *    other iterators or the list itself.
	 */
	struct dt_list_iterator * (* iterator)
		(struct dt_list * this_);
	
	/** Deletes this list.
	 *
	 *  Arguments:
	 *    this_: This list.
	 *
	 *  Notes:
	 *    Invalidates all references to this list and
	 *    all iterators produced.
	 */
	void (* del)(struct dt_list * this_);

	/** Internal state.
	 */
	void * _data;
};

/** A List Iterator Interface.
 */
struct dt_list_iterator {
	
	/** Gets the current item.
	 *  
	 *  Arguments:
	 *    this_: This iterator.
	 *
	 *  Returns:
	 *    The item at the current position.
	 *
	 *  Notes:
	 *    Invalid if valid returns zero.
	 */
	void * (* get)(const struct dt_list_iterator * this_);

	/** Checks if there is a current item.
	 *  
	 *  Arguments:
	 *    this_: This iterator.
	 *
	 *  Returns:
	 *  One if there is an item to get. Zero otherwise.
	 */
	int (* valid)(const struct dt_list_iterator * this_);

	/** Moves the iterator to the next item.
	 *
	 *  Arguments:
	 *    this_: This iterator.
	 *
	 *  Returns:
	 *    Zero on success. A negative number otherwise.
	 */
	int (* next)(struct dt_list_iterator * this_);

	/** Moves the iterator to the previous item.
	 *
	 *  Arguments:
	 *    this_: This iterator.
	 *
	 *  Returns:
	 *    Zero on success. A negative number otherwise.
	 */
	int (* previous)(struct dt_list_iterator * this_);

	/** Inserts an item at the current position
	 *    and moves all the items starting from
	 *    here back one. e.g. [i] -> [i+1] for i in [position, size)
	 *
	 *  Arguments:
	 *    this_: This iterator.
	 *    item: The item to insert.
	 *
	 *  Returns:
	 *    Zero on sucess. A negative number otherwise.
	 *
	 *  Notes:
	 *    Invalidates all other iterators.
	 *
	 *    This call is valid when the iterator is past the
	 *    end of the list and will add to the end. This iterator
	 *    will then be at that item.
	 */
	int (* insert)(struct dt_list_iterator * this_, void * item);

	/** Removes the item at the current position
	 *    and moves any subsequent items back one.
	 *    e.g. [i] -> [i -1] for i in [position, size - 1).
	 *
	 *  Arguments:
	 *    this_: This iterator.
	 *
	 *  Returns:
	 *    Zero on success. A negative number otherwise.
	 *
	 *  Notes:
	 *    Invalidates all other iterators.
	 */
	int (* remove)(struct dt_list_iterator * this_);

	/** Deletes this iterator.
	 *
	 *  Arguments:
	 *    this_: This iterator.
	 */
	void (* del)(struct dt_list_iterator * this_);

	/** The position of the iterator in the list.
	 *
	 *  Updated by next and previous. 
	 */
	size_t position;

	/** The referenced list.
	 */
	struct dt_list * list;


	/** Internal state.
	 */
	void * _data;
};


/** Creates a new list.
 *
 *  This should create the list you will
 *  most likely want to use.
 *
 *  Returns:
 *    A list.
 */
struct dt_list * dt_list_new(void);

#ifdef __cplusplus
}
#endif

#endif // __LIST_H_
