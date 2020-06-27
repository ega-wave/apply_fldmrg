
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#include <stdio.h>	// for size_t
#include <stdbool.h>

struct vector_int8
{
	unsigned char* buf_;
	size_t size_;	// 0 <= size_ < capacity_
	size_t capacity_;
	bool stack_flg_;
};

void vector_int8__construct(struct vector_int8* v, unsigned char* buf, size_t capacity);
unsigned char* vector_int8__begin(struct vector_int8* v);
unsigned char* vector_int8__end(struct vector_int8* v);
void vector_int8__set_end(struct vector_int8* v, unsigned char* end);
size_t vector_int8__size(struct vector_int8* v);
bool vector_int8__empty(struct vector_int8* v);
unsigned char* vector_int8__find(unsigned char* first, unsigned char* last, unsigned char c);
unsigned char vector_int8__at(struct vector_int8* v, size_t idx);
void vector_int8__free_buffer(struct vector_int8* v);
unsigned char* vector_int8__copy_n(const unsigned char* first, size_t n, unsigned char* result);
void vector_int8__reserve(struct vector_int8* v, size_t n);
void vector_int8__push(struct vector_int8* v, unsigned char val);
void vector_int8__append(struct vector_int8* v, unsigned char* first, unsigned char* last);
void vector_int8__clear(struct vector_int8* v);
void vector_int8__destruct(struct vector_int8* v);

