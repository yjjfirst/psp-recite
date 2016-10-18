#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BOUNDARY_LENGTH 26
#define WORDLIST_LENGTH 120

typedef unsigned char uchar;
typedef unsigned int uint;

typedef struct _WORD_STRUCTURE {
	char  spell[32];
	char  symbol[32];
	uchar ch[64];
	char  content[1200];
}WORD_STRUCTURE, *WORD_STRUCTURE_P;

int words_count;
WORD_STRUCTURE words_list[WORDLIST_LENGTH];

uchar word_boundary[BOUNDARY_LENGTH];

void get_boundary(FILE *fp)
{
	fread(word_boundary, BOUNDARY_LENGTH, 1, fp);
	rewind(fp);
}

uchar *ustrstr (uchar* buf, uint buf_len, uchar *sub, uint sub_len)
{
	uchar *bp;
	uchar *sp;
	int i, j;

	if (buf == NULL || buf == NULL) {
		return NULL;
	}

	for (i = 0; i < buf_len ; i++) {
		bp = buf;
		sp = sub;
		j = 0;

		while(*bp++ == *sp++) {
			j++;
		} ;

		if (j == sub_len) {
			return buf;
		}
		buf += 1;
	}
	return NULL;
}

int max_content;

void parse_previous_word(uchar *word)
{ 
	uchar *pos;

//	printf("\n**************************************************\n");
//	pos = word + 3;
//	printf("spell: %s\n", pos);
//	pos = strlen(pos) + pos + 1;
//	printf("symbol: %s\n", pos);
//	pos = strlen(pos) + pos + 1;
//	printf("ch: %s\n", pos);
//	pos = strlen(pos) + pos +1;
//	printf("content: %s\n", pos);
//	printf("\n**************************************************\n");

	if (strlen(pos) > max_content) {
		max_content = strlen(pos);
	}
}

int main ()
{
	FILE *fp = NULL;
	uchar *line_buffer = NULL;
	uchar *begin;
	size_t len = 0;
	ssize_t read;
	uchar word_buffer[1200];
	int word_len = 0;
	uchar *pos;
	int previous = 0;

	if ( (fp = fopen("../words/gre_test", "r")) == NULL) {
		printf("can not open word files\n");
		return;
	}

	get_boundary(fp);
 
	while ( (read = getline(&line_buffer, &len, fp)) != -1) {
		if ( ( pos = ustrstr(line_buffer, read, word_boundary, BOUNDARY_LENGTH)) != NULL) {
			if (previous > 0) {
				memcpy(word_buffer+ word_len, line_buffer, pos - line_buffer);
				parse_previous_word(word_buffer);
			}

			previous ++;
			pos += BOUNDARY_LENGTH;
			word_len = line_buffer + read - pos;
			memcpy(word_buffer, pos, word_len);
		} else {
			memcpy(word_buffer + word_len, line_buffer, read);
			word_len += read;
		}
	}

	if (previous > 0) {
		parse_previous_word(word_buffer);
	}

	if (line_buffer) {
		free(line_buffer);
	}

	return 0;
}
