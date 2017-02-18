Lists
=====

Lists, basically the extension to an array.
They're variable size and are only really
limited by the system itself.

#### error
The errors that can be returned by the
interface.


#### linked
A simple linked list. The implemented version
is doubly linked just to fill out all the
interface. Normally you only need a singly
linked list as you can always save the amount
you need to go back across or just iterate over
the list again. As such the provided code is
designed to be efficient if the backward pointers
and related code is removed.

Run times:
 - Get(index) -> O(index)
 - Insert(index) -> O(index)
 - Remove(index) -> O(index)
 - Length(list) -> O(1)
 - Iterator.Valid()
 - Iterator.Get() -> O(1)
 - Iterator.Next() -> O(1)
 - Iterator.Previous() -> O(1)
 - Iterator.Insert() -> O(1)
 - Iterator.Remove() -> O(1)

Note:
 - In many cases the length operation may take
   up to the length of the list itself or
   O(sizeof(list)).
 - It is usually not common to traverse the list
   backwards and it may take up to O(index - 1)
   to get the previous on an iterator or even
   be impossible to do directly.
 - It is also worth noting that we assume that the
   memory allocator can operate in O(1) time
   because otherwise all insertion and removing
   operation depend on the speed of that. This
   is more important for this data structure
   than the other lists as they do not make
   many calls to the memory allocator.

#### vector
A resizing array. It uses less memory than a linked
list and keep the elements close in memory but it may
need to do a lot of maintenance to maintain order and
expand. The code provided uses a doubling strategy to
make resizing the buffer rather insignificant.

Run times:
 - Get() -> O(1)
 - Insert(index) -> O(sizeof(list))
 - Remove(index) -> O(sizeof(list))
 - Length(list) -> O(1)
 - Iterator.Valid()
 - Iterator.Get() -> Same as Get()
 - Iterator.Next() -> O(1)
 - Iterator.Previous() -> O(1)
 - Iterator.Insert(index) -> Same as Insert(index)
 - Iterator.Remove(index) -> Same as Remove(index)

Notes:
 - The index for the iterator is the stored index.
 - Insert / Remove are actually more likely to take
   O(sizeof(list) - index). And if you calculate it
   This is also the amortized time. So if you ran
   many tests this is the performance you could
   expect.

#### read-only
This is just a simple wrapper to prevent modification
of the underlying list. It may actually be better
to just create iterators in most cases but this gives
a quick and dirty way to guarantee no modifications
take place.


