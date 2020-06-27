
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#include "vector_int32.h"
#include <stdlib.h>	// for malloc(),free()

void vector_int32__construct(struct vector_int32* v, int* buf, size_t capacity)
{
	v->buf_ = buf;
	v->size_ = 0;
	v->capacity_ = capacity;
	v->stack_flg_ = true;
}

int* vector_int32__begin(struct vector_int32* v)
{
	return v->buf_;
}

size_t vector_int32__size(struct vector_int32* v)
{
	return v->size_;
}

int vector_int32__at(struct vector_int32* v, size_t idx)
{
	return v->buf_[idx];
}

void vector_int32__free_buffer(struct vector_int32* v)
{
	if (v->stack_flg_ != true) free(v->buf_);
}

int* vector_int32__copy_n(int* first, size_t n, int* result)
{
	for (size_t i = 0; i < n; ++i) {
		*result++ = *first++;
	}
	return result;
}

void vector_int32__reserve(struct vector_int32* v, size_t n)
{
	int* tmp_buf = (int*)malloc(sizeof(int) * n);
	vector_int32__copy_n(vector_int32__begin(v), vector_int32__size(v), tmp_buf);
	vector_int32__free_buffer(v);
	v->buf_ = tmp_buf;
	v->capacity_ = n;
	v->stack_flg_ = false;
}

void vector_int32__push(struct vector_int32* v, int val)
{
	if (v->size_+1 >= v->capacity_) {
		vector_int32__reserve(v, v->capacity_ * 2);
	}
	v->buf_[v->size_++] = val;
}

void vector_int32__destruct(struct vector_int32* v)
{
	vector_int32__free_buffer(v);
}

