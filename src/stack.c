/* stack.c: stack implem */
#include "stack.h"

#define MAXSTACK 128
#define EMPTYSTACK -1
static int top = EMPTYSTACK;
static char *items[MAXSTACK];

void stack_push(char *c) {
	items[++top] = c;
}

char *stack_pop() {
	return items[top--];
}

int stack_full()  {
	return top+1 == MAXSTACK;
}

int stack_empty()  {
	return top == EMPTYSTACK;
}

#if 0
int main()
{
	int i;
	char str[] = "1234567890";
	
	i = 0;
	while(str[i] != '\0') {
		push(str + i);
		i ++;
	}

	while(!empty()) {
		printf("%s\n", pop());
	}	

}
#endif
