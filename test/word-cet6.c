#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_words(char *words)
{
	char *ptr = words;
	char *pos_start;
	char *pos_end;
	char buf[1024];
	int i;
	int len;	

	while ( (pos_start = strstr(ptr, "<<<")) != NULL) {		
		pos_start += 3;
		ptr = pos_start;
		memset(buf, 0, sizeof(buf));
		if ( (pos_end = strstr(ptr, "<<<")) == NULL) {
			pos_end = ptr + strlen(ptr);	
		}
		
		i = 0;
		while (*(pos_start) != '/') {
			buf[i] = *pos_start;
			i ++;
			pos_start ++;	
		}
		pos_start ++;
		printf("%s\n", buf);
		i = 0;
		while (*(pos_start) != '/') {
			buf[i] = *pos_start;
			i ++;
			pos_start ++;
		}
		pos_start ++;
		
		printf("%s\n", buf);
		while ( *(pos_start) == 0x0d || *(pos_start) == 0x0a) pos_start ++;	

		len = (sizeof(buf) - 1 > (pos_end - pos_start)) ? (pos_end - pos_start) : (sizeof(buf) - 1);
		strncpy(buf, pos_start, len); 

		ptr = pos_end;
		printf("%s\n", buf);
	}					
}

int main()
{
	FILE *fp = NULL;
	int size;
	char *words;

	if ( (fp = fopen("../words/cet6", "r")) == NULL) {
		printf("can not open word lib.\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if ( (words =(char *) calloc(1, size + 1)) == NULL ) {
		printf("not enough memory.\n");
		goto fail;
	}

	words[size] = '\0';
		
	fread(words, size, 1, fp);

	parse_words(words);
	
	free(words);
fail:
	fclose(fp);
}
