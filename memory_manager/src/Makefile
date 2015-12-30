CFLAGS := -fPIC -O3 -g -Wall -Werror
CC := gcc
MAJOR := 0
MINOR := 1
NAME := memory_manager
VERSION := $(MAJOR).$(MINOR)

lib: lib$(NAME).so.$(VERSION)

test: $(NAME)_test
	LD_LIBRARY_PATH=. ./$(NAME)_test

$(NAME)_test: lib$(NAME).so
	$(CC) $(NAME)_test.c -o $@ -L. -l$(NAME)

lib$(NAME).so: lib$(NAME).so.$(VERSION)
	ldconfig -v -n .
	ln -s lib$(NAME).so.$(MAJOR) lib$(NAME).so

lib$(NAME).so.$(VERSION): $(NAME).o
	$(CC) -shared -Wl,-soname,lib$(NAME).so.$(MAJOR) $^ -o $@

clean:
	$(RM) $(NAME)_test *.o *.so*
