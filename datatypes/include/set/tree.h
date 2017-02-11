#ifndef __SET_TREE_H__
#define __SET_TREE_H__

#include "set.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a new tree set.
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
 *    A new set. Or null if there is not
 *    enough memory.
 */

struct dt_set * dt_set_tree_new(
	int (* comparator)(void * a, void * b),
	unsigned int (* hash)(void * item));


#ifdef __cplusplus
}
#endif

#endif // __SET_TREE_H__
