
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

#include "vector_int8.h"
#include <stdlib.h>	// for malloc(),free()

void vector_int8__construct(struct vector_int8* v, unsigned char* buf, size_t capacity)
{
	v->buf_ = buf;
	v->size_ = 0;
	v->capacity_ = capacity;
	v->stack_flg_ = true;
}

unsigned char* vector_int8__begin(struct vector_int8* v)
{
	return v->buf_;
}

unsigned char* vector_int8__end(struct vector_int8* v)
{
	return &v->buf_[vector_int8__size(v)];
}

void vector_int8__set_end(struct vector_int8* v, unsigned char* end)
{
	v->size_ = end - vector_int8__begin(v);
}

size_t vector_int8__size(struct vector_int8* v)
{
	return v->size_;
}

bool vector_int8__empty(struct vector_int8* v)
{
	return v->size_ == 0;
}

unsigned char* vector_int8__find(unsigned char* first, unsigned char* last, unsigned char c)
{
	for ( ; first != last; ++first) {
		if (*first == c) return first;
	}
	return last;
}

unsigned char vector_int8__at(struct vector_int8* v, size_t idx)
{
	return v->buf_[idx];
}

void vector_int8__free_buffer(struct vector_int8* v)
{
	if (v->stack_flg_ != true) free(v->buf_);
}

unsigned char* vector_int8__copy_n(const unsigned char* first, size_t n, unsigned char* result)
{
	for (size_t i = 0; i < n; ++i) {
		*result++ = *first++;
	}
	return result;
}

void vector_int8__reserve(struct vector_int8* v, size_t n)
{
	if (v->capacity_ >= n) return;

	unsigned char* tmp_buf = (unsigned char*)malloc(sizeof(unsigned char) * n);
	vector_int8__copy_n(vector_int8__begin(v), vector_int8__size(v), tmp_buf);
	vector_int8__free_buffer(v);
	v->buf_ = tmp_buf;
	v->capacity_ = n;
	v->stack_flg_ = false;
}

void vector_int8__push(struct vector_int8* v, unsigned char val)
{
	if (v->size_+1 >= v->capacity_) {
		vector_int8__reserve(v, v->capacity_ * 2);
	}
	v->buf_[v->size_++] = val;
}

void vector_int8__append(struct vector_int8* v, unsigned char* first, unsigned char* last)
{
	size_t n = last - first;
	vector_int8__reserve(v, vector_int8__size(v) + n);
	vector_int8__copy_n(first, n, vector_int8__end(v));
	v->size_ += n;
}

void vector_int8__clear(struct vector_int8* v)
{
	v->size_ = 0;
}

void vector_int8__destruct(struct vector_int8* v)
{
	vector_int8__free_buffer(v);
}

