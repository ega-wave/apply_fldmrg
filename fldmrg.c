
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
 * argv[1]: min(fd) fdの最小値
 * argv[2]: fdの数
 * fd=n, n+1, ..., n+m-1 (argv[2]個)のfdをreadし、paste(1)のように出力する
 */
int main(int argc, char* argv[])
{
	int start_fd = atoi(argv[1]);
	int n = atoi(argv[2]);
	bool is_all_eof_flag = true;	// 全fieldがEOFかを表すflag

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

			if ( i == n-1 ) {	// i == NFだったら
				if ( is_all_eof(vector_int32__begin(&results_v), n) == true ) {
					goto done;
				} else {
					putchar('\n');
					is_all_eof_flag = true;	// 再度初期化
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
	// 全てのfdをclose()
	for (int i=0; i<n; ++i) {
		close(start_fd + i);
	}

	return 0;
}

/*
 * res[0] ... res[n-1]がすべてEOFならtrue、
 * 1つでもEOFでない値があればfalseを返す
 */
bool is_all_eof(int res[], int n)
{
	for (int i = 0; i < n; ++i) {
		if (res[i] != EOF) return false;
	}
	return true;
}

/*
 * fdから1行読み込み、stdoutに書く
 * 戻り値：0   fdから読み込んでいって、'\n'に当たった時
 *       : EOF EOFに当たった時
 * 注意：'\n'で止まっていないと、無限ループ
 */
int read_1line_and_write(int fd, struct vector_int8* v)
{
	int c;

	while (1)
	{
		if ((c = getc_fd(fd)) == EOF) return EOF;
		if (c == '\n') return 0;

		// もしvにwhite_spacesがあれば、
		// fputc(c, stdout)の前にそのwhite_spacesを全部出力する
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
 * fdから1文字readしてそれを返す
 * fgetc(fp)のfd版
 */
int getc_fd(int fd)
{
	char buf[4];
	if (read(fd, buf, 1) == 0) return EOF;
	return buf[0];
}

