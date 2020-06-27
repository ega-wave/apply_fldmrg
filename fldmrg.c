
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>	// for isspace()
#include <unistd.h> // for close()
#include <stdbool.h>

#include "vector_int32.h"
#include "vector_int8.h"

bool is_all_eof(int res[], int n);
int read_1line_and_write(int fd, struct vector_int8* v);
int getc_fd(int fd);
int __attribute__((__cdecl__)) fileno (FILE *);

/*
 * usage: fldmrg n m
 * argv[1]: min(fd) fd�̍ŏ��l
 * argv[2]: fd�̐�
 * fd=n, n+1, ..., n+m-1 (argv[2]��)��fd��read���Apaste(1)�̂悤�ɏo�͂���
 */
int main(int argc, char* argv[])
{
	int start_fd = atoi(argv[1]);
	int n = atoi(argv[2]);
	bool is_all_eof_flag = true;	// �Sfield��EOF����\��flag

	int results[16];	// results[0] .. results[n-1]
	struct vector_int32 results_v;
	vector_int32__construct(&results_v, results, ARRAY_LENGTH(results));
	vector_int32__reserve(&results_v, n);

	unsigned char white_spaces[16];
	struct vector_int8 white_spaces_v;
	vector_int8__construct(&white_spaces_v, white_spaces, ARRAY_LENGTH(white_spaces));
	vector_int8__reserve(&white_spaces_v, n);

	while (1) {
		for (int i=0; i < n; ++i) {
			*(vector_int32__begin(&results_v) + i) = read_1line_and_write(start_fd + i, &white_spaces_v);
			if (vector_int32__at(&results_v, i) != EOF) is_all_eof_flag = false;

			if ( i == n-1 ) {	// i == NF��������
				if ( is_all_eof(vector_int32__begin(&results_v), n) == true ) {
					goto done;
				} else {
					putchar('\n');
					is_all_eof_flag = true;	// �ēx������
				}
			} else {
				if (is_all_eof_flag == true &&
						vector_int32__at(&results_v, i) == EOF) {
//fprintf(stderr, "results[%d]==EOF\n", i);
					vector_int8__push(&white_spaces_v, ' ');
				} else {
					putchar(' ');
				}
			}
		}
	}

done:
	// �S�Ă�fd��close()
	for (int i=0; i<n; ++i) {
		close(start_fd + i);
	}

	return 0;
}

/*
 * res[0] ... res[n-1]�����ׂ�EOF�Ȃ�true�A
 * 1�ł�EOF�łȂ��l�������false��Ԃ�
 */
bool is_all_eof(int res[], int n)
{
	for (int i = 0; i < n; ++i) {
		if (res[i] != EOF) return false;
	}
	return true;
}

/*
 * fd����1�s�ǂݍ��݁Astdout�ɏ���
 * �߂�l�F0   fd����ǂݍ���ł����āA'\n'�ɓ���������
 *       : EOF EOF�ɓ���������
 * ���ӁF'\n'�Ŏ~�܂��Ă��Ȃ��ƁA�������[�v
 */
int read_1line_and_write(int fd, struct vector_int8* v)
{
	int c;

	while (1)
	{
		if ((c = getc_fd(fd)) == EOF) return EOF;
		if (c == '\n') return 0;

		// ����v��white_spaces������΁A
		// fputc(c, stdout)�̑O�ɂ���white_spaces��S���o�͂���
		if (vector_int8__empty(v) == false) {
			size_t n = vector_int8__size(v);
			for (int i = 0; i < n; ++i) {
				int ch = vector_int8__at(v, i);
				putchar(ch);
			}
			vector_int8__clear(v);
		}

		putchar(c);
	}
	
	return 0;	// never reach
}

/*
 * fd����1����read���Ă����Ԃ�
 * fgetc(fp)��fd��
 */
int getc_fd(int fd)
{
	char buf[4];
	if (read(fd, buf, 1) == 0) return EOF;
	return buf[0];
}

