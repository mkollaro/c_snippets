# C Code Fragments

This project contains my experiments and exercises in C, as well as various
snippets that I might re-user later. Some of them are back from the times when
I was just learning C, thus they might not be of the highest quality.


## Contents:

* hash table that compiles into both a static library (`htable.a`) or a shared
  one (`htable.so`)
* `wordcount` program that counts word frequency using the above hash table
* a very limited re-implementation of the UNIX program `tail` (has a fixed
  limit of how long an input line can be)


## Usage:

You will need the `gcc` compiler and build tools, which you can get by
running on Debian-based systems like Ubuntu:

    $ sudo apt-get install build-essential


Afterwards, just run:

    $ make
    $ make tests  # you will need `valgrind` and `cppcheck` for this

Program usage examples:

    $ cat tests/files/wordcount_simple2.txt| ./wordcount
    1 elephant
    2 giraffe
    5 cow
    3 dog

    $ ./tail -3 tests/files/book.txt
    including how to make donations to the Project Gutenberg Literary
    Archive Foundation, how to help produce our new eBooks, and how to
    subscribe to our email newsletter to hear about new eBooks.
