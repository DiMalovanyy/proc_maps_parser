mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

binary_name=proc_maps_parser
binary_include_dir=$(mkfile_dir)include

all: lib$(binary_name).a $(binary_name)_test

lib$(binary_name).a: $(binary_name).o
	ar rcs $@ $?

$(binary_name)_test: $(binary_name).o test.o
	gcc $? -o $@

$(binary_name).o: src/proc_maps.c
	gcc -c $? -I$(binary_include_dir) -o $@

test.o: test/test.c
	gcc -c $? -I$(binary_include_dir) -o $@

clean:
	rm -rf $(binary_name)
	rm -rf $(binary_name)_test
	rm -rf *.o
	rm -rf *.a


.PHONY: clean test
