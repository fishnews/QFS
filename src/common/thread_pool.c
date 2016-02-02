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

int create_thread_pool(int num)
{
		int i;
		pool = (ThreadPool*)malloc(sizeof(ThreadPool));
		pool->shutdown = 0;
		pool->max_thread_num = num;
		pool->head = NULL;
		pool->tail = NULL;
		pthread_mutex_init(&pool->mutex, NULL);
		pthread_cond_init(&pool->cond, NULL);

		pool->thread_id = (pthread_t *)malloc(sizeof(pthread_t) * num);
		for(i=0; i<num; i++)
		{
				if(pthread_create(&pool->thread_id[i], NULL, thread_routine, NULL) != 0)
				{
						printf("create thread error\n");
						exit(1);
				}
		}
		return 0;
}

int destroy_thread_pool()
{
		int i;
		ThreadWork *member;
		if(pool->shutdown)
		{
				return 0;
		}
		pool->shutdown = 1;
		pthread_mutex_lock(&pool->mutex);
		pthread_cond_broadcast(&pool->cond);
		pthread_mutex_unlock(&pool->mutex);

		for(i=0; i<pool->max_thread_num; i++)
		{
				pthread_join(pool->thread_id[i], NULL);
		}
		free(pool->thread_id);

		while(pool->head)
		{
				member = pool->head;
				pool->head = pool->head->next;
				free(member->arg);
				free(member);
		}

		pthread_mutex_destroy(&pool->mutex);
		pthread_cond_destroy(&pool->cond);
		return 0;
}

int add_work_to_pool(void*(*routine)(void*), void *arg)
{
		ThreadWork *work;
		if(!routine)
		{
				printf("function is null\n");
				return -1;
		}
		work = (ThreadWork*)malloc(sizeof(ThreadWork));
		work->routine = routine;
		work->arg = arg;
		work->next = NULL;

		pthread_mutex_lock(&pool->mutex);
		if(!pool->head)
		{
				pool->head = work;
				pool->tail = work;
		}
		else
		{
				pool->tail->next = work;
				pool->tail = work;
		}
		pthread_cond_signal(&pool->cond);
		pthread_mutex_unlock(&pool->mutex);
		return 0;
}
