Stack
=====
Stacks, a precursor to lists. They are
a simple data structure which allows as
much storage as memory will allow. They
are simple enough to implement quickly
yet still quite useful in understanding
programming in general.

#### error
The errors that can be returned by the
interface.

#### linked
A really simple stack. A pointer to the
next and the current value. Most operations
on this stack can be implemented in a few lines.

Run times:
 - Push() -> O(1)
 - Pop() -> O(1)
 - Peek() -> O(1)

Notes:
 - These all depend on the memory allocator, if
   the memory allocator cannot give O(1) performance
   than the memory allocator will dominate and
   that will be the performance of the Push / Pop
   operations.

#### vector
A stack stored in a resizable array. Using a
doubling growing strategy when the buffer is
full. This makes the resizing effects less
significant.

Run times:
 - Push() -> O(sizeof(stack))
 - Pop() -> O(sizeof(stack))
 - Peek() -> O(1)

Notes
 - For Push / Pop you will see O(1) performance
   most of the time they only use a lot of time
   in a resize. And if you calculate the amortized
   time it is O(1).

