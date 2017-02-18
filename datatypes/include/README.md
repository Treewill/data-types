Include
=======

Here you'll find the definitions of the data types
in the sub-folders you'll find specific implementations
of the data type.

#### buffers.h

This is a simple macro library to do common buffer
manipulations i.e. computing size from length and
vice-versa

#### stack
A basic stack data type.
Things get pushed on to the top.
Then later popped off the top in a
last in first out order. Operations include:
 - push an item on top of the stack
 - pop an item off the top of the stack.
 - peek at the item on top (without removing it)
 - retrieve the length of the stack

#### list

This a basic unbounded list interface.
Supporting the following operations:
 - indexed insertions
 - indexed removals
 - indexed retrievals
 - retrieving the length
 - iteration

Iteration simply acts like the above but
saves the position as it goes along.

#### set
A simple set, much like a set in mathematics
it stores unique items and can quickly check
if the item is there and return it.
The set can also be used like a map by altering
input parameters.
Supports the following:
 - insertion
 - removal
 - membership check (returns the item too)
 - listing of the items

The hash function and comparison function are needed
to make the operations efficient otherwise the sets
cannot give any advantage over searching a list.
