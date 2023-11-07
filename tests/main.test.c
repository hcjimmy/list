#include "list.test.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef int (*tester_func)(void);

void announce_fails_or_die(int fails)
{
	if(fails < 0) {
		fputs("Exiting.\n", stderr);	// The error resulting this should've already been reported.
		exit(fails);
	} else if(fails == 0) {
		fputs("All passed.\n", stderr);
	} else {
		fprintf(stderr, "%d tests failed :(\n", fails);
	}
}

/* Run a tester  */
int run_tester(tester_func tester, char *test_name)
{
	int fails;

	fprintf(stderr, "--- Testing %s ---\n", test_name);
	fails = tester();
	announce_fails_or_die(fails);
	fprintf(stderr, "--- %s test end ---\n", test_name);


	return fails;
}

/* Run all testers given (functions defined above as tester_func, returning the number of failed tests).
 *
 * prefix	String to be printed before all tests.
 * suffix	String to be printed after all tests.
 * ...		Each parameter should be a test function pointer followed by the test's name,
 * 			the last parameter should be NULL. */
int run_testers(char *prefix, char *suffix, ...)
{
	va_list ap;
	int (*tester)(void);
	char *test_name;

	int ret = 0;

	fputs(prefix, stderr);
	fputc('\n', stderr), fputc('\n', stderr);

	va_start(ap, suffix);

	while((tester = va_arg(ap, tester_func))) {
		if(!(test_name = va_arg(ap, char*))) {
			fprintf(stderr, "Error in run_testers: missing tester function name.\n");
			va_end(ap);
			return -1;
		}

		ret += run_tester(tester, test_name);
	}

	va_end(ap);

	fputc('\n', stderr);
	fputs(suffix, stderr);
	fputc('\n', stderr);

	return ret;
}

int main()
{
	int fails = run_testers("======= BEGINNING TESTS ======",
				"========= TESTS END =========",
				int_list_to_array_tester, "int_list_to_array",
				int_list_pop_index_tester, "int_list_pop_index",
				int_list_pop_index_no_preserve_tester, "int_list_pop_index_no_preserve",
				NULL);
	announce_fails_or_die(fails);
	return fails;
}
