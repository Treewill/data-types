#ifndef __STACK_LINKED_H__
#define __STACK_LINKED_H__

#include "stack.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a new linked stack.
 *
 *  Returns:
 *    A new stack. Or null if there is not
 *    enough memory.
 */
struct dt_stack * dt_stack_linked_new(void);

#ifdef __cplusplus
}
#endif
#endif // __STACK_LINKED_H__
