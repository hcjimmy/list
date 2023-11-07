#include "../list.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

list_def_proto(int);
list_def_funcs(int);

short int_list_init_with(struct int_list *list, unsigned amount, ...)
{
	va_list ap;

	if(int_list_init(list))
		return -1;

	va_start(ap, amount);

	for(; amount != 0; amount--) {
		if(int_list_append(list, va_arg(ap, int))) {
			int_list_close(list, NULL);
			va_end(ap);
			return -1;
		}
	}

	va_end(ap);
	return 0;
}

short copy_array_to_int_list(int *array, size_t array_length, int_list *new_list)
{
	int_list list;
	int *end;

	if(int_list_init(new_list))
		return -1;

	end = array + array_length;
	while(array != end) {
		if(int_list_append(new_list, *array)) {
			int_list_close(new_list, NULL);
			return -1;
		}
		++array;
	}

	return 0;
}

int int_list_comp_array(int_list *list, int array[], size_t array_length)
{
	int_iterator iterator = get_int_list_iterator(list);
	size_t i;
	int next;

	for(i = 0; !int_list_get(&iterator, list, &next); i++) {
		if(i == array_length)
			return -1;
		if(next != array[i]) {
			return next - array[i];
		}
	}
	if(i != array_length)
		return +1;

	return 0;
}

void int_list_print(int_list *list, FILE *fp, char *seperator, void (*fprint_value)(int value, FILE* fp))
{
	int next;
	int_iterator it;
	it = get_int_list_iterator(list);

	if(int_list_get(&it, list, &next))
		return;
	fprint_value(next, fp);

	while(!int_list_get(&it, list, &next)) {
		fputs(seperator, fp);
		fprint_value(next, fp);
	}
}

void print_int_array(int *array, size_t array_length, char *seperator, FILE* fp)
{
	if(array_length == 0)
		return;

	fprintf(fp, "%d", *array);
	int *array_end = array + array_length;

	while(++array < array_end) {
		fputs(seperator, fp);
		fprintf(fp, "%d", *array);
	}
}

void fprint_int(int value, FILE*fp)
{
	fprintf(fp, "%d", value);
}

bool int_list_contains(int_list *list, int value)
{
	int next;
	int_iterator it = get_int_list_iterator(list);

	while(!int_list_get(&it, list, &next)) {
		if(next == value)
			return true;
	}
	return false;
}

bool test_int_list_to_array(int_list *list, int expected_array[], size_t array_length)
{
	int next;

	if(int_list_comp_array(list, expected_array, array_length)) {
		fprintf(stderr, "List is different from expected.\n"
				"\tExpected list: ");
		print_int_array(expected_array, array_length, ", ", stderr);
		fputs("\n\tList: ", stderr);
		int_list_print(list, stderr, ", ", fprint_int);
		fputc('\n', stderr);

		return true;
	}
	return false;
}


bool test_int_list_pop_index(int array[], size_t array_length, size_t removed_index)
{
	int_list list;
	int popped;
	bool failed;

	copy_array_to_int_list(array, array_length, &list);

	popped = int_list_pop_index(&list, removed_index);

	if((failed = (popped != array[removed_index]))) {
		fprintf(stderr, "Expected popped value at index %ld to be %d, but was %d.\n",
				removed_index, array[removed_index], popped);
	}

	for(size_t i = removed_index + 1; i < array_length; ++i) {
		array[i-1] = array[i];
	}
	array_length--;

	if(int_list_comp_array(&list, array, array_length)) {
		fprintf(stderr, "List is different from expected.\n"
				"\tExpected list: ");
		print_int_array(array, array_length, ", ", stderr);
		fputs("\n\tList: ", stderr);
		int_list_print(&list, stderr, ", ", fprint_int);
		fputc('\n', stderr);

		failed = true;
	}

	int_list_close(&list, NULL);
	return false;
}


bool test_int_list_pop_index_no_preserve(int array[], size_t array_length, size_t removed_index)
{
	int_list list;
	int popped;
	bool failed;

	copy_array_to_int_list(array, array_length, &list);

	popped = int_list_pop_index_no_preserve(&list, removed_index);

	if((failed = (popped != array[removed_index]))) {
		fprintf(stderr, "Expected popped value at index %ld to be %d, but was %d.\n",
				removed_index, array[removed_index], popped);
	}

	if(int_list_contains(&list, popped)) {
		fprintf(stderr, "Expecting popped value %d to have been removed, but wasn't.\n", popped);
		failed = true;
	}

	return failed;
}

int int_list_to_array_tester()
{
	int fails = 0;
	int expected_array[] = { 0,1,2,3,4 };
	int_list list[5];

	if(
			int_list_init_with(list+0, 0)
			|| int_list_init_with(list+1, 1, 	0,1)
			|| int_list_init_with(list+2, 2, 	0,1,2)
			|| int_list_init_with(list+3, 3, 	0,1,2,3)
			|| int_list_init_with(list+4, 4, 	0,1,2,3,4)
	) {
		fprintf(stderr, "Memory allocation failed.\n");
		return -1;
	}

	for(size_t i = 0; i < 5; ++i) {
		fails += test_int_list_to_array(list+i, expected_array, i);
	}

	return fails;
}

int int_list_pop_index_tester()
{
#define ARR_LEN 10

	int fails = 0;

	int array[ARR_LEN] = { 0,1,2,3,4,5,6,7,8,9 };
	int tested_array[ARR_LEN];

	for(ssize_t i = 0; i < ARR_LEN; i++) {
		memcpy(tested_array, array, sizeof(array));
		fails += test_int_list_pop_index(tested_array, 10, i);
	}

	return fails;
}

int int_list_pop_index_no_preserve_tester()
{
	int fails = 0;

	int array[ARR_LEN] = { 0,1,2,3,4,5,6,7,8,9 };

	for(ssize_t i = 0; i < ARR_LEN; i++) {
		fails += test_int_list_pop_index_no_preserve(array, 10, i);
	}

	return fails;
}

