Data Types
==========
Some sample code for learning about abstract data types
and high level functions.

You can use this learn how to create data types and
understand them too.

This project is primarily for linux but if you can get
it working on other systems and keep it working on linux
I'll be happy to add your fix.

#### Getting Started

1. Getting the source, it is as easy as.
   ```
   git clone --recurse-submodules git@github.com:Treewill/data-types.git
   ```
2. Building. To do this you will need to
   make directory to build in (say `build`)
   and run `cmake` there with the path to this repo.
   ```
   cd path/to/repo
   mkdir build
   cd build
   cmake ..
   ```
   Add `-DCMAKE_BUILD_TYPE=DEBUG` to `cmake` if you want
   debugging symbols for your debugger.
   ```
   cmake -DCMAKE_BUILD_TYPE=DEBUG ..
   ```
3. Now you are ready to build and run the tests.
   ```
   make all test
   ```
4. Explore the source and tests.

#### C Primer

  If you are unfamiliar with C you should find a
  tutorial before proceeding. If on the other hand
  you are very fluent in C you should skip this
  section. Ensure that you are familiar with
  `struct`s and pointers before proceeding.

  There are two concepts which I have noticed some
  difficulty around and I will try my best to explain
  them here in hope that they won't trip you up too
  much when using this code.

  The first being `void *` it really just means a
  pointer to anything. You will always need to cast
  it to the correct type. For example you could
  store a string in it and then print it.
  ```
  char * really_obscure_thing = "The text";
  void * str = really_obscure_thing;
  printf("%s\n", (char *)str);
  ```
  The reason for `void *` is mostly when your
  code does not care what is being pointed at.

  The second being function pointers. These are
  used when you don't really care what code you
  are going to enter but you know *how* to enter
  the code and *what* will come of it. It is hard
  to demonstrate exactly without getting in to
  something complicated but here's the gist of it.
  ```
  // declare function pointer.
  void (* print_it)(int * it);

  // declare functions with the same signature
  void print_value(int * it) { printf("%d\n", *it); }
  void print_place_and_value(int * it)
  {
          printf("[%p] = %d\n", it, *it);
  }

  void main(void)
  {
          // Assign the function pointer.
          print_it = &print_value;

          int integers[20];
          for(int i = 0; i < 20; i++) {
                  integers[i] = i * i;
          }

          // Use the chosen print function to
          // print the value.
          *print_it(integers + 4);
          *print_it(integers + 13);
  }
  ```
  It becomes a lot more useful when the pointer
  is set very far away from where it is used much
  like how the library implements a list data type.
  (See list/linked.c the new\_\* functions). All
  of the interfaces will use these because C does
  not have another way to do interfaces after it
  is linked. Before it is linked you need only to
  supply a different implementation.

#### Implementing for yourself

  It is a wonderful exercise. All you need to do is
  clear out the `src/lib` folder. Then you can work
  on your own. Feel free to just delete the one you
  want to work on and leave the rest intact that way
  most of the programs will still work.
  
  To get your hands dirty quick just modify the hello
  program and rebuild. Then you are ready to go further.

  To build all you need to do is switch to your `build`
  directory and run `make` again. You do not really
  need to be familiar with this process to implement
  stuff here. If you are having trouble try rerunning
  `cmake` as described earlier.

  Once you are started you can use the
  `build/datatypes/bin/*_cli` programs to test early
  versions.

  If you get stuck I suggest looking into `gdb` and
  `valgrind` both of the programs were designed to
  help you write programs. Just google "gdb" and 
  "gdb cheat sheet". They are quite useful in tracking
  down bugs. 
  
  And when you think your code is ready you can
  run it against the tests in `build/datatypes/test/*'
  programs.

#### More documentation

  You should be able to find more specific documentation
  in the `README.md` along side the file. All of the
  interactive programs should spit out some help if you
  type help while they are running.

#### Contributing

  It is great that you want to help out, however, I
  am implementing this mostly for myself. Anything
  in `src/lib` I will not accept modifications for
  but other parts I might.

  Test cases are great and testing programs too but
  I do not want to clutter it with much more than the
  bare minimum to play around with the API. If you
  have ideas for improving it great but do not go
  too overboard. It is just there to help out an
  implmentor. If you think there is a particullary
  tricky corner case I have left out let me know in
  the patch.

#### LICENSE

  The licenses are in LICENSE files which apply to
  that directory and any directories below.

  I doubt anything in the `datatypes/include` or
  `datatypes/src/lib` merits any kind of copyright
  but to be sure just follow the license.

#### Parting Words

  Happy coding everyone.
