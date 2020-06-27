
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#include "vector_int64.h"
#include <stdlib.h>	// for malloc(),free()

void vector_int64__construct(struct vector_int64* v, char** buf, size_t capacity)
{
	v->buf_ = buf;
	v->size_ = 0;
	v->capacity_ = capacity;
	v->stack_flg_ = true;
}

char** vector_int64__begin(struct vector_int64* v)
{
	return v->buf_;
}

size_t vector_int64__size(struct vector_int64* v)
{
	return v->size_;
}

char* vector_int64__at(struct vector_int64* v, size_t idx)
{
	return v->buf_[idx];
}

void vector_int64__free_buffer(struct vector_int64* v)
{
	if (v->stack_flg_ != true) free(v->buf_);
}

char** vector_int64__copy_n(char** first, size_t n, char** result)
{
	for (size_t i = 0; i < n; ++i) {
		*result++ = *first++;
	}
	return result;
}

void vector_int64__reserve(struct vector_int64* v, size_t n)
{
	char** tmp_buf = (char**)malloc(sizeof(char*) * n);
	vector_int64__copy_n(vector_int64__begin(v), vector_int64__size(v), tmp_buf);
	vector_int64__free_buffer(v);
	v->buf_ = tmp_buf;
	v->capacity_ = n;
	v->stack_flg_ = false;
}

void vector_int64__push(struct vector_int64* v, char* val)
{
	if (v->size_+1 >= v->capacity_) {
		vector_int64__reserve(v, v->capacity_ * 2);
	}
	v->buf_[v->size_++] = val;
}

void vector_int64__destruct(struct vector_int64* v)
{
	vector_int64__free_buffer(v);
}

