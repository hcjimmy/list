/* Generic-like list in c.
 * Copyright (C) 2023  hcjimmy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include <stdlib.h>
#include <stddef.h>

#define INIT_LEN 8
#define REALLOC_MULT 1.5

#define list_def_proto(type) named_list_def_proto(type, type)
#define list_def_funcs(type) named_list_def_funcs(type, type)

/* -- Define the structs and function protypes -- */
#define  named_list_def_proto(type, name)								\
													\
/* To contain the list. */										\
typedef struct name##_list {										\
	type *buf;											\
	type *p_buf;											\
	type *buf_end;											\
} name##_list;												\
													\
/* Type to go over the list. */										\
typedef struct name##_list_iterator {									\
	type *pval;											\
} name##_list_iterator;											\
													\
/* list_init - initialize list.
 *
 * Must be called on list before usage of the other functions.
 *
 * Return a non-zero value if memory allocation error occures, otherwise 0.
 *
 * Notes:
 * 	list may be a local varaible, called as `list_init(&list);`.
 *
 * 	You may call this function multiple times on the same varaible after calling list_close,
 * 	or after successful return of list_to_array.
 */													\
short name##_list_init(name##_list *list);								\
													\
													\
/* list_close - Free all data associated with the list.
 * free_value should free value of type, or be NULL if freeing it is not desired. */			\
void name##_list_close(name##_list *list, void(*free_value)(type));					\
     													\
													\
/* list_push - Adds value to the end of the list, returning 0 on success and non-zero
 * value on memory failure.
 *
 * If memory failure occures, the list is not freed and is unmodified. */				\
short name##_list_push(name##_list *list, type value);							\
													\
													\
/* list_remove - Remove value at index, and return it. 
 *
 * index is expected to be valid (the implementation doesn't neccessarily check). */			\
type name##_list_remove(name##_list *list, size_t index);						\
													\
													\
/* list_remove_no_preserve - Remove value from list, but doesn't guarantee
 * to preservation of the original order of the list.
 *
 * This allows the implementation under dynamic array to be O(1). */					\
type name##_list_remove_no_preserve(name##_list *list, size_t index);					\
													\
													\
/* Return the number of elements in the list. */							\
size_t name##_list_length(const name##_list *list);							\
													\
													\
/* list_to_array - Convert the list to array.
 *
 * On memory allocation failure:
 * 	NULL is returned and the list is unmodified.
 *
 * On success:
 * 	The list should must not be accessed or closed, and the returned array must be freed instead.
 */													\
type* name##_list_to_array(name##_list *list);								\
													\
													\
/* list_comp - Compare two lists.
 *
 * comp should return 0 if the values equal, otherwise a representing the calculation (like strcmp, for example).
 *
 * If both lists equal: 0 is returned.
 * If different values are found in the list: the result of comp on the first different values is returned.
 * If the lists are of differing length, but otherwise equal: if list1 is shorter -1, if list2 is shorter 1.
 */													\
int name##_list_comp(name##_list *list1, name##_list *list2,						\
		int(*comp)(type val1, type val2));							\
													\
													\
/* get_list_iterator - Return an iterator to the list "pointing" at start of list.
 *
 * The iterator may be used with function list_get to go over the list in sequential order.
 *
 * Note: "Pointing at start" means the next call to list_get will give the first value in the list.
 */													\
name##_list_iterator get_##name##_list_iterator(const name##_list *list);				\
													\
													\
/* list_get - Get the next value in the list, indicated by iterator.
 *
 * If reached the end of the list:
 * 	A non-zero value is returned, iterator is unmodified.
 *
 * Otherwise:
 * 	0 is returned, *next is set to the next value in the list, and iterator is set to
 * 	point at the value after next in the list.
 *
 * Note:
 * 	After the end is reached it is legal to call this function again, unless the list is modified
 * 	it should return the same value.
 */													\
short name##_list_get(name##_list_iterator *iterator, const name##_list *list, type *next);		\
													\
													\
/* list_get_index - Get value at specified index (assumed to be a valid index).
 *
 * Note: under a linked list implementation (if it's implemented in the future),
 * this would be quite slow. If working sequentially, it might be preferable to use list_get,
 * or convert the list to an array.
 */										\
type name##_list_get_index(name##_list *list, size_t index);


/* -- Define the implementation -- */
#define named_list_def_funcs(type, name)								\
													\
short name##_list_init(struct name##_list *list)							\
{													\
	list->buf = malloc(sizeof(*list->buf) * INIT_LEN);						\
	if(!list->buf)											\
		return -1;										\
													\
	list->p_buf = list->buf;									\
	list->buf_end = &list->buf[INIT_LEN];								\
	return 0;											\
}													\
													\
short name##_list_push(struct name##_list *list, type value) 						\
{													\
	type *tmp;											\
													\
	if(list->p_buf == list->buf_end) {	/* If reached end of allocated buffer. */		\
		tmp = realloc(list->buf,								\
				sizeof(*list->buf) * 							\
				((list->buf_end - list->buf) * REALLOC_MULT));				\
		if(!tmp)										\
			return -1;									\
													\
		/* Move pointers to point at tmp. */							\
		list->p_buf = &tmp[list->p_buf - list->buf];						\
		list->buf_end = &tmp[(int)((list->buf_end - list->buf) * REALLOC_MULT)];		\
		list->buf = tmp;									\
	}												\
													\
	*list->p_buf++ = value;										\
	return 0;											\
}													\
													\
void name##_list_close(struct name##_list *list, void(*free_value)(type))				\
{													\
	if(free_value != NULL)										\
		while(--list->p_buf >= list->buf)							\
			free_value(*list->p_buf);							\
	free(list->buf);										\
}													\
													\
size_t name##_list_length(const struct name##_list *list)						\
{													\
	return list->p_buf - list->buf;									\
}													\
													\
type* name##_list_to_array(struct name##_list *list)							\
{													\
	type *tmp;											\
													\
	if(name##_list_length(list) == 0)								\
		return list->buf;									\
													\
	tmp = realloc(list->buf, sizeof(*list->buf) * (list->p_buf - list->buf));			\
	return (!tmp) ? list->buf : tmp;								\
}													\
													\
struct name##_list_iterator get_##name##_list_iterator(const struct name##_list *list)			\
{													\
	struct name##_list_iterator iterator;								\
													\
	iterator.pval =	list->buf;									\
	return iterator;										\
}													\
													\
short name##_list_get(struct name##_list_iterator *iterator, const struct name##_list *list, type *next)\
{													\
	if(iterator->pval >= list->p_buf)	/* If reached the end. */				\
		return -1;										\
													\
	*next = *(iterator->pval++);									\
	return 0;											\
}													\
													\
type name##_list_get_index(name##_list *list, size_t index)						\
{													\
	return list->buf[index];									\
}													\
													\
int name##_list_comp(struct name##_list *list1, struct name##_list *list2,				\
		int(*comp)(type val1, type val2))							\
{													\
	type *pval1, *pval2;										\
													\
	int comparison;											\
													\
	pval1 = list1->buf;										\
	pval2 = list2->buf;										\
													\
	while(pval1 != list1->p_buf) {									\
													\
		if(pval2 == list2->p_buf)								\
			return 1;									\
		if((comparison = comp(*pval1, *pval2)) != 0)						\
			return comparison;								\
		pval1++, pval2++;									\
	}												\
													\
	return (pval2 == list2->p_buf) ? 0 : -1;							\
}													\
													\
type name##_list_remove(name##_list *list, size_t index)						\
{													\
	type *p = list->buf + index;									\
	type removed = *p;										\
													\
	list->p_buf--;											\
													\
	for (;p < list->p_buf; p++)									\
		p[0] = p[1];										\
													\
	return removed;											\
}													\
													\
type name##_list_remove_no_preserve(name##_list *list, size_t index)					\
{													\
	type removed = list->buf[index];								\
	list->buf[index] = *--list->p_buf;								\
	return removed;											\
}

