#include "../thread_pool.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

void *fun(void *s);

void *fun(void *s)
{
		printf("run function\n");
}
int main()
{
		create_thread_pool(10);
		int i;
		for(i=0; i<10; i++)
		{
				add_work_to_pool(fun, NULL);
		}
		while(1);
}
