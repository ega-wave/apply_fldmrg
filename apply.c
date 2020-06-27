
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>	// for atoi
#include <sys/wait.h> // for wait

#include "vector_int32.h"
#include "vector_int64.h"
#include "vector_int8.h"

int fork_with_cmd(int* pipe_fd, size_t size, int* pipe_fd_ptr, size_t in_fd_idx, size_t out_fd_idx, char* cmdline);
void close_not_used_fd(int pipe_fd[], size_t size, int used_fd[], size_t size_of_used_fd);
void ordering(int* fds, size_t n);
int getNF();

/*
 * usage: apply n 'command1 ...' m 'command2 ...' ...
 * 引数: argv[2k-1] ... 変換を加えるフィールド(1<=k)
 *       argv[2k]   ... そのフィールドに適用するコマンド(1<=k)
 * 引数で指定したフィールドに対しては、対応するコマンドを実行し、
 * 指定されてないフィールドに対しては、そのまま、
 * それらの出力をfldmrg(1)で集めて標準出力に出力する
 */
int main(int argc, char* argv[])
{
	enum { IDENTICAL = 0,	// 変換しない
			FORK = 1 		/* 変換する */ };

	int num_of_processes = 1;	// applyするプロセス数+1
	int NF = getNF();	// awkでいうところのNF
//fprintf(stderr, "getNF():<%d>\n", NF);
	if (NF == 0) return 0;

	int fork_type_array[16];	// sizeはNFだけ必要
	struct vector_int32 fork_type_array_v;
	vector_int32__construct(&fork_type_array_v, fork_type_array, ARRAY_LENGTH(fork_type_array));

	char** argv_tmp = argv;
	int j = 1;	// 1 <= j < argc
	int next_field = (argv_tmp[j] == NULL) ? -1 : atoi(argv_tmp[j]);	// 引数で指定された最初のフィールド
	for (int i = 1; i <= NF; ++i) {
		if (i == next_field) {
			vector_int32__push(&fork_type_array_v, FORK); ++num_of_processes;	// FORKならinclement
			j += 2;
			next_field = (argv_tmp[j] == NULL) ? -1 : atoi(argv_tmp[j]);	// 引数で指定された次のフィールド
		} else {
			vector_int32__push(&fork_type_array_v, IDENTICAL);
		}
	}

	int fds_to_write_by_apply[16];	// sizeはNFだけ必要
	struct vector_int32 fds_to_write_by_apply_v;
	vector_int32__construct(&fds_to_write_by_apply_v, fds_to_write_by_apply, ARRAY_LENGTH(fds_to_write_by_apply));

	int fds_to_read_by_fldmrg[16];	// sizeはNFだけ必要
	struct vector_int32 fds_to_read_by_fldmrg_v;
	vector_int32__construct(&fds_to_read_by_fldmrg_v, fds_to_read_by_fldmrg, ARRAY_LENGTH(fds_to_read_by_fldmrg));

	int pipe_fd[64];	// sizeは最大4*NF必要
	struct vector_int32 pipe_fd_v;
	vector_int32__construct(&pipe_fd_v, pipe_fd, ARRAY_LENGTH(pipe_fd));

	// fork()
	argv_tmp = argv;
	j = 2;	// 2 <= j < argc
	for (int i = 0; i < NF; ++i) {
		if (vector_int32__at(&fork_type_array_v, i) == FORK) {
			// FORKが指定されたフィールドの場合
			int pipe_fd_tmp[4];
			if ((pipe(&pipe_fd_tmp[0]) < 0) ||
				(pipe(&pipe_fd_tmp[2]) < 0)) {
				fputs("pipe() error1", stderr);
				return 1;
			}
			for (int k = 0; k < 4; ++k) {
				vector_int32__push(&pipe_fd_v, pipe_fd_tmp[k]);
			}
			vector_int32__push(&fds_to_write_by_apply_v, pipe_fd_tmp[1]);
			vector_int32__push(&fds_to_read_by_fldmrg_v, pipe_fd_tmp[2]);

			int c_pid = fork_with_cmd(vector_int32__begin(&pipe_fd_v),
									vector_int32__size(&pipe_fd_v),
									pipe_fd_tmp, 0, 3, argv_tmp[j]);
			if ( c_pid < 0 ) return 1;
			j += 2;
		} else {
			// IDENTICALが指定されたフィールドの場合
			int pipe_fd_tmp[2];
			if (pipe(&pipe_fd_tmp[0]) < 0) {
				fputs("pipe() error1", stderr);
				return 1;
			}
			for (int k = 0; k < 2; ++k) {
				vector_int32__push(&pipe_fd_v, pipe_fd_tmp[k]);
			}
			vector_int32__push(&fds_to_write_by_apply_v, pipe_fd_tmp[1]);
			vector_int32__push(&fds_to_read_by_fldmrg_v, pipe_fd_tmp[0]);
		}
	}

	int c_pid;
	if ((c_pid = fork()) < 0) {
		fputs("fork() error", stderr);
		return 1;
	} else if (c_pid == 0) {
		close(0);
		close_not_used_fd(vector_int32__begin(&pipe_fd_v),
							vector_int32__size(&pipe_fd_v),
							vector_int32__begin(&fds_to_read_by_fldmrg_v),
							vector_int32__size(&fds_to_read_by_fldmrg_v));
		ordering(vector_int32__begin(&fds_to_read_by_fldmrg_v),
					vector_int32__size(&fds_to_read_by_fldmrg_v));

		char* child_argv2[4];
		char buf1[16]; char buf2[16];
		child_argv2[0] = "fldmrg";
		sprintf(buf1,"%d",3);	// fldmrgの第１引数は3（fdの最小値）
		sprintf(buf2,"%d",vector_int32__size(&fds_to_read_by_fldmrg_v));
		child_argv2[1] = buf1;
		child_argv2[2] = buf2;
		child_argv2[3] = NULL;
		execvp("fldmrg", child_argv2);
		_exit(0);
	} else {
		close_not_used_fd(vector_int32__begin(&pipe_fd_v),
							vector_int32__size(&pipe_fd_v),
							vector_int32__begin(&fds_to_write_by_apply_v),
							vector_int32__size(&fds_to_write_by_apply_v));
	}

	unsigned char in[512];
	struct vector_int8 in_v;
	vector_int8__construct(&in_v, in, ARRAY_LENGTH(in));

	int c;
	size_t fldno = 0;
	bool isSpaceFlag = true;

	while (1)
	{
		if ( (c = fgetc(stdin)) == EOF ) {
			// breakするが、
			// NF-フィールドが'\n'で終わってない場合はflushする
			if (isSpaceFlag == false) {
				vector_int8__push(&in_v, (unsigned char)'\n');
				write(vector_int32__at(&fds_to_write_by_apply_v, fldno-1),
						vector_int8__begin(&in_v), vector_int8__size(&in_v));
				vector_int8__clear(&in_v);
			}
			break;
		}

		if (isspace(c)) {
			if (isSpaceFlag == true) { /* nop */ }
			else {
				isSpaceFlag = true;
				vector_int8__push(&in_v, (unsigned char)'\n');
				write(vector_int32__at(&fds_to_write_by_apply_v, fldno-1),
						vector_int8__begin(&in_v), vector_int8__size(&in_v));
				vector_int8__clear(&in_v);
				if (c == '\n') { fldno = 0; }
			}
		} else {
			if (isSpaceFlag == true) {
				isSpaceFlag = false;
				++fldno;
			} else {
			}
			vector_int8__push(&in_v, (unsigned char)c);
		}
	}

	size_t fds = vector_int32__size(&fds_to_write_by_apply_v);
	for (size_t i = 0; i < fds; ++i) {
		close(vector_int32__at(&fds_to_write_by_apply_v, i));
	}

	for (int i = 0; i < num_of_processes; ++i) {
		int status;
		wait(&status);
	}
	return 0;
}

/* fork() & exec()する
 * さらに、pipe_fd_ptr[in_fd_idx]を0に、pipe_fd_ptr[out_fd_idx]を1につなぎかえる
 * さらに、in_fd,out_fd以外のすべてのfdをclose()する
 */
int fork_with_cmd(int* pipe_fd, size_t size, int* pipe_fd_ptr, size_t in_fd_idx, size_t out_fd_idx, char* cmdline)
{
	int c_pid;
	if ((c_pid = fork()) < 0) {
		fputs("fork() error", stderr);
		return -1;
	} else if (c_pid == 0) {
		dup2(pipe_fd_ptr[in_fd_idx],0);
		dup2(pipe_fd_ptr[out_fd_idx],1);
		for (int i=0; i < size; ++i) {
			close(pipe_fd[i]);
		}

		char* child_argv[4];
		struct vector_int64 child_argv_v;
		vector_int64__construct(&child_argv_v, child_argv, ARRAY_LENGTH(child_argv));

		char* begin = cmdline;
		bool single_quote_flag = false;
		for (; *cmdline != '\0'; ++cmdline) {
			if (*begin == ' ') {
				++begin;
				continue;
			} else if (single_quote_flag == false && *begin == '\'') {
				single_quote_flag = true;
				++begin;
				continue;
			}

			if (single_quote_flag == false && *cmdline == ' ') {
				*cmdline = '\0';
				vector_int64__push(&child_argv_v, begin);
				begin = cmdline +1;
			} else if (single_quote_flag == true && *cmdline == '\'') {
				single_quote_flag = false;
				*cmdline = '\0';
			}
		}
		vector_int64__push(&child_argv_v, begin);
		vector_int64__push(&child_argv_v, NULL);
		execvp(*vector_int64__begin(&child_argv_v), vector_int64__begin(&child_argv_v));
		_exit(0);
	} else {
		/* nop */
	}

	return c_pid;
}

void close_not_used_fd(int pipe_fd[], size_t size, int used_fd[], size_t size_of_used_fd)
{
	int i = 0;	// 0 <= i < size
	int j = 0;	// 0 <= j < size_of_used_fd
	for (; i < size; ++i) {
		int used_fd_j = (j < size_of_used_fd) ? used_fd[j]: -1;
		if (pipe_fd[i] != used_fd_j) {
			close(pipe_fd[i]);
		} else {
			++j;
		}
	}
}

/*
 * fds[0] .. fds[n-1] を、3 .. n+2につなぎなおす
 */
void ordering(int* fds, size_t n)
{
	for (int i = 0; i < n; ++i) {
		if (fds[i] != i+3) {
			dup2(fds[i], i+3);
			close(fds[i]);
		}
	}
}

/*
 * stdinから1行読み込み、NFを返す(0 <= NF)
 */
int getNF()
{
	unsigned char in[512];
	struct vector_int8 in_v;
	vector_int8__construct(&in_v, in, ARRAY_LENGTH(in));

	int c;
	size_t fldno = 0;
	bool isSpaceFlag = true;

	while (1)
	{
		if ( (c = fgetc(stdin)) == EOF ) break;
		vector_int8__push(&in_v, (unsigned char)c);

		if (isspace(c)) {
			isSpaceFlag = true;
			if (c == '\n') break;
		} else {
			if (isSpaceFlag == true) ++fldno;
			isSpaceFlag = false;
		}
	}

	// stdinから読み込んだ文字列をstdinに戻す
	unsigned char* begin = vector_int8__begin(&in_v);
	unsigned char* end = vector_int8__end(&in_v);
	for (--end; begin <= end; --end) {
		ungetc(*end, stdin);
	}

	return fldno;
}

