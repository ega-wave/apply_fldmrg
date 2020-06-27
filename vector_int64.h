
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#include <stdio.h>	// for size_t
#include <stdbool.h>

struct vector_int64
{
	char** buf_;
	size_t size_;	// 0 <= size_ < capacity_
	size_t capacity_;
	bool stack_flg_;
};

void vector_int64__construct(struct vector_int64* v, char** buf, size_t capacity);
char** vector_int64__begin(struct vector_int64* v);
size_t vector_int64__size(struct vector_int64* v);
char* vector_int64__at(struct vector_int64* v, size_t idx);
void vector_int64__free_buffer(struct vector_int64* v);
char** vector_int64__copy_n(char** first, size_t n, char** result);
void vector_int64__reserve(struct vector_int64* v, size_t n);
void vector_int64__push(struct vector_int64* v, char* val);
void vector_int64__destruct(struct vector_int64* v);

