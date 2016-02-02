#include "thread_pool.h"

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

static ThreadPool *pool = NULL;

static void* thread_routine(void *arg)
{
		ThreadWork *work;
		while(1)
		{
				pthread_mutex_lock(&pool->mutex);
				while(!pool->head && !pool->shutdown)
				{
						pthread_cond_wait(&pool->cond, &pool->mutex);
				}
				if(pool->shutdown)
				{
						pthread_mutex_unlock(&pool->mutex);
				}
				work = pool->head;
				pool->head = pool->head->next;
				pthread_mutex_unlock(&pool->mutex);
				work->routine(arg);
				free(work->arg);
				free(work);
		}
		return NULL;
}
