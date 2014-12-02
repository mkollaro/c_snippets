# Copyright 2009 Martina Kollarova
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Isrc -pedantic -g -O2 -fPIC
CFLAGS += -DDEBUG

EXE = tail wordcount wordcount-static
OBJ_TAIL = src/tail.o src/debug.o
OBJ_HTABLE = src/htable.o src/htable_iterator.o
OBJ_WORDCOUNT = src/wordcount.o src/io.o

SOURCES=$(wildcard src/**/*.c src/*.c)


# universal rules for generating all object files
%.o : src/%.c
	$(CC) $(CFLAGS) -c $<

# starting rule
all: dep tail src/htable.a wordcount

# build the static version of wordcount
static: wordcount-static src/htable.so

.PHONY: all clear dynamic tests


################# DEP ########################
# generating dependencies for modules, (run make dep when changing dep.)
dep:
	$(CC) -MM src/*.c > src/dep.list
-include src/dep.list


################# MAIN #######################
tail: $(OBJ_TAIL)
	$(CC) $(CFLAGS) $(OBJ_TAIL) -o $@

wordcount: $(OBJ_WORDCOUNT) src/htable.so
	$(CC) $(CFLAGS) $(OBJ_WORDCOUNT) src/htable.so -o $@

wordcount-static: $(OBJ_WORDCOUNT) src/htable.a
	$(CC) $(CFLAGS) $(OBJ_WORDCOUNT) -Bstatic src/htable.a -o $@


# static library
src/htable.a: $(OBJ_HTABLE)
	ar crs $@ $(OBJ_HTABLE)

# dynamic library
src/htable.so: $(OBJ_HTABLE)
	$(CC) $(CFLAGS) -shared -fPIC $(OBJ_HTABLE) -o $@


tests:
	make
	# TAIL
	bats tests/tail.bats
	# WORD COUNT
	bats tests/wordcount.bats

clean:
	rm -f $(EXE)
	cd src && rm -f *.o *.a *.so dep.list
