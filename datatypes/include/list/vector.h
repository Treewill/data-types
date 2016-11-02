#ifndef __LIST_VECTOR_H__
#define __LIST_VECTOR_H__

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a new vector list.
 *
 *  Returns:
 *    A new list. Or NULL if there is not
 *    enough memory.
 */
struct dt_list * dt_list_vector_new(void);

#ifdef __cplusplus
}
#endif
#endif //__LIST_VECTOR_H__
