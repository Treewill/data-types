#ifndef __LIST_LINKED_H__
#define __LIST_LINKED_H__

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a new linked list.
 *
 *  Returns:
 *    A new list. Or NULL if there's not
 *    enough memory.
 */
struct dt_list * dt_list_linked_new(void);

#ifdef __cplusplus
}
#endif
#endif //__LIST_LINKED_H__
