#ifndef __SET_H__
#define __SET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

#include "list.h"

struct dt_set;

/** A Set Interface.
 */

struct dt_set {

	/** Inserts the item into the set.
	 *
	 *  Arguments:
	 *    this_: This set.
	 *    item: The item to insert.
	 *
	 *  Returns:
	 *    Zero on success. A negative number otherwise.
	 */
	int (* insert)(struct dt_set * this_, void * item);

	/** Removes the item from the set.
	 *
	 *  Arguments:
	 *    this_: This set.
	 *    item: The item to remove from the set.
	 */
	void (* remove)(struct dt_set * this_, void * item);

	/** Checks if the set has the item.
	 *
	 *  Arguments:
	 *    this_: This set.
	 *    item: The item to check.
	 *
	 *  Returns:
	 *    The item that matched, if found, null otherwise.
	 */
	void * (* has)(const struct dt_set * this_, void * item);

	/** Produces an immutable list for viewing the set.
	 *
	 *  Arguments:
	 *    this_: This set.
	 *
	 *  Returns:
	 *    An immutable list containing the elements of
	 *    the set.
	 *
	 *  Note:
	 *    Any modification to the set invalidates this list.
	 */
	struct dt_list * (* items)(const struct dt_set * this_);

	/** Deletes this set.
	 *
	 *  Arguments:
	 *    this_: This set.
	 */
	void (* del)(struct dt_set * this_);

	/** Internal state.
	 */
	void * _data;
};



/** Creates a new Set.
 *
 *  This should create the set you will
 *  most likely want to use.
 *
 * Arguments:
 *   comparator: A function which orders inputs.
 *     Arguments:
 *       a: The first item.
 *       b: The second item.
 *
 *     Returns:
 *       0 if a is logically equal to b.
 *       -1 if a comes before b.
 *       1 if a comes after b.
 *   hash: A function which maps
 *         inputs down to a number.
 *     Arguments:
 *       item: The item to hash.
 *     Returns:
 *       A number.
 *
 *  Returns:
 *    A set.
 */
struct dt_set * dt_set_new(
	int (* comparator)(void * a, void * b),
	unsigned int (* hash)(void * item));

#ifdef __cplusplus
}
#endif

#endif // __SET_H_
