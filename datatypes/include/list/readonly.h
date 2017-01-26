#ifndef __LIST_READONLY_H__
#define __LIST_READONLY_H__

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a new read only list.
 *
 *  Arguments:
 *    list: The list to make an image of.
 *
 *  Notes: 
 *    This list is invalid if the other list
 *    is changed.
 *
 *  Returns:
 *    A new list. Or NULL if there is not
 *    enough memory.
 */
struct dt_list * dt_list_readonly_new(struct dt_list * list);

#ifdef __cplusplus
}
#endif
#endif //__LIST_READONLY_H__
