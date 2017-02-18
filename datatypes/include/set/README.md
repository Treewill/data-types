Set
===
Sets are much like mathematical sets
however since the comparator and hash
functions are not known to the interface
it is possible to store extra data along
side the things being compared. Sets
can be faster than searching through a
list because they do not preserve order.

Note: A list implementation must be
available for the sets. It is usually
used to list out the elements of the set.

#### error
The errors sets can return.

#### hash
This is actually not simple set
and it requires some other set
available to fall into as a base case.
It is possible to have a 'pure' hash
set but this requires that the hash
function is one to one. Since we
cannot guarantee that or likely even
depend on it (consider strings of
arbitrary length), we fall down
to a simple set, the tree set in this case.

Run times:
  Identical to the base set.

Notes:
  - You can actually expect much better
    performance all the way up to O(1)
    depending on the specifics of the
    hash set. The larger the table and
    the better distributed the hashes
    the closer it gets. A really poor
    hashing algorithm will produce the
    same worst case.
  - In this case it is about sqrt(sizeof(set))
    in place of sizeof(set).

#### list
A list backed set. The list is
kept sorted for quick retrieval.

Run times:
 - Insert -> O(sizeof(set))
 - Remove -> O(sizeof(set))
 - Has -> O(log(n))

Notes:
 - All operations could be worse
   if the wrong type of list is used.

#### tree
Using binary search tree we can get
good times for all operations. An
AVL tree is used here for best worst
case.

Run times:
 - All: O(log(n))

Notes:
 - Varies with the memory allocator
   if allocation takes too long
   insertion operations may be much
   slower.
