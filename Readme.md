# List - Generic-like list in c.

This project provides macros for generating a struct to contain a list of a given type and functions to work with it dynamically. Currently there's only an implementation of a dynamic array, but other implementations may be added later.

## Usage

The basic usage is to call `list_def_proto` in a header file to define the type-specific struct
and prototypes, and then call `list_def_funcs` in the code file to create the functions.

An example could be

```
	list_def_proto(int)	// (in a header)
	list_def_funcs(int)	// (in .c file)
```

which would create the struct `int_list`, and the functions `int_list_init`, `int_list_append`,
`int_list_close`, etc.

Unless using `typedef`, these macros should not be called with pointers as, for example,
calling `list_def_proto(int*)` would attempt to erroneously create struct `int*_list`.

Alternatively, `named_list_def_proto` and `named_list_def_funcs` can be used to allow the name and
type to be different, allowing us to work with pointers, or otherwise making our life more
convenient:

E.g. `named_list_def_proto(char*, string)` would define a list of type char pointers named
`string_list`, and the appropriate prototypes.

Note: it is unrecommended to access the struct directly, as the implementation may change, but to
use the functions provided instead.

The functions (with `<name>_list` changed to `list`):

- `list_init`				        Initialize a list.
- `list_append`				        Add value to the end of a list.
- `list_get_index`                  Get value at specified index.
- `list_pop_index`				    Remove value at specific index and return it.
- `list_pop_index_no_preserve`      Same as `list_pop_index`, but don't guarantee preserving order (faster with dynamic-array).
- `list_length`				        Get the number of elements in the list.
- `list_to_array`			        Convert the list to an array.
- `list_comp`				        Compare two lists.
- `get_list_iterator`		        Get value to iterate over the list.
- `list_get`                        Iterate over a list with iterator.
- `list_close`                      Free allocated memory associated with the list.

See further documentation of each function in the header file.

## License

This project is licensed under GPLv3.
