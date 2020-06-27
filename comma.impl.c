
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>	// for strncmp()

#include "vector_int8.h"

unsigned char* comma2(const unsigned char* begin, const unsigned char* end, unsigned char* pout_begin)
{
	size_t len = (size_t)(end-begin);
	size_t num_of_commas = (len-1)/3;
	const unsigned char* pin = end -1;
	unsigned char* pout_end = pout_begin + len + num_of_commas;
	unsigned char* pout = pout_end -1;

	while (num_of_commas-- > 0) {
		*pout-- = *pin--;
		*pout-- = *pin--;
		*pout-- = *pin--;
		*pout-- = ',';
	}
	for (; begin <= pin;) {
		*pout-- = *pin--;
	}

	return pout_end;
}

unsigned char* comma4(const unsigned char* begin, const unsigned char* end, unsigned char* pout_begin)
{
	size_t len = (size_t)(end-begin);
	size_t num_of_commas = (len-1)/4;
	const unsigned char* pin = end -1;
	unsigned char* pout_end = pout_begin + len + num_of_commas;
	unsigned char* pout = pout_end -1;

	while (num_of_commas-- > 0) {
		*pout-- = *pin--;
		*pout-- = *pin--;
		*pout-- = *pin--;
		*pout-- = *pin--;
		*pout-- = ',';
	}
	for (; begin <= pin;) {
		*pout-- = *pin--;
	}

	return pout_end;
}

void comma(struct vector_int8* in_v, struct vector_int8* out_v, bool comma4flag)
{
	unsigned char* begin = vector_int8__begin(in_v);
	size_t len = vector_int8__size(in_v);
	int minus_flag = 0;
	if (vector_int8__at(in_v, 0) == '-') {
		++begin;
		--len;
		minus_flag = 1;
		vector_int8__push(out_v, '-');
	}

	unsigned char* period = 
		vector_int8__find(begin, vector_int8__end(in_v), '.');
	size_t seisuu_bu_len = period - begin;

	size_t num_of_commas;
	if (comma4flag == true) {
		num_of_commas = (seisuu_bu_len-1)/4;
	} else {
		num_of_commas = (seisuu_bu_len-1)/3;
	}
	
	vector_int8__reserve(out_v, len + num_of_commas + minus_flag);

	if (comma4flag == true) {
		unsigned char* out_end =
			comma4(begin
				, period
				, vector_int8__end(out_v));
		vector_int8__set_end(out_v, out_end);
	} else {
		unsigned char* out_end =
			comma2(begin
				, period
				, vector_int8__end(out_v));
		vector_int8__set_end(out_v, out_end);
	}

	vector_int8__append(out_v, period, vector_int8__end(in_v));
}

int main(int argc, char* argv[])
{
	bool comma4flag = false;
	if (argv[1] != NULL && strncmp(argv[1], "-4", 2) == 0) {
		comma4flag = true;
	}

	unsigned char out[512];
	struct vector_int8 out_v;
	vector_int8__construct(&out_v, out, ARRAY_LENGTH(out));

	unsigned char in[256];
	struct vector_int8 in_v;
	vector_int8__construct(&in_v, in, ARRAY_LENGTH(in));

	int c;
	bool isSpaceFlag = true;

	while ( (c = fgetc(stdin)) != EOF )
	{
		if (isspace(c)) {
			if (isSpaceFlag == true) { /* nop */ }
			else {
				isSpaceFlag = true;
				comma(&in_v, &out_v, comma4flag);

				vector_int8__push(&out_v, (unsigned char)'\n');
				vector_int8__push(&out_v, (unsigned char)'\0');
				fputs(vector_int8__begin(&out_v), stdout);

				vector_int8__clear(&in_v);
				vector_int8__clear(&out_v);
			}
		} else {
			isSpaceFlag = false;
			vector_int8__push(&in_v, (unsigned char)c);
		}
	}
	if (! vector_int8__empty(&in_v)) {
		comma(&in_v, &out_v, comma4flag);

		vector_int8__push(&out_v, (unsigned char)'\n');
		vector_int8__push(&out_v, (unsigned char)'\0');
		fputs(vector_int8__begin(&out_v), stdout);
	}

	return 0;
}

