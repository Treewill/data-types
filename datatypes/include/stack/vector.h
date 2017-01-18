#ifndef __STACK_VECTOR_H__
#define __STACK_VECTOR_H__

#include "stack.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a new vector stack.
 *
 *  Returns:
 *    A new stack. Or NULL if there is not
 *    enough memory.
 */
struct dt_stack * dt_stack_vector_new(void);

#ifdef __cplusplus
}
#endif
#endif //__STACK_VECTOR_H__
