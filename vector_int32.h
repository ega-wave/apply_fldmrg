
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#include <stdio.h>	// for size_t
#include <stdbool.h>

struct vector_int32
{
	int* buf_;
	size_t size_;	// 0 <= size_ < capacity_
	size_t capacity_;
	bool stack_flg_;
};

void vector_int32__construct(struct vector_int32* v, int* buf, size_t capacity);
int* vector_int32__begin(struct vector_int32* v);
size_t vector_int32__size(struct vector_int32* v);
int vector_int32__at(struct vector_int32* v, size_t idx);
void vector_int32__free_buffer(struct vector_int32* v);
int* vector_int32__copy_n(int* first, size_t n, int* result);
void vector_int32__reserve(struct vector_int32* v, size_t n);
void vector_int32__push(struct vector_int32* v, int val);
void vector_int32__destruct(struct vector_int32* v);

