#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__

#include <pthread.h>

typedef struct threadWork
{
		void*				(*routine)(void *arg);
		void				*arg;
		struct threadWork	*next;
}ThreadWork;

typedef struct threadPool
{
		int					shutdown;
		int					max_thread_num;
		pthread_t			*thread_id;
		ThreadWork			*head;
		ThreadWork			*tail;
		pthread_mutex_t		mutex;
		pthread_cond_t		cond;;
}ThreadPool;

int	create_thread_pool(int num);
int destroy_thread_pool();
int add_work_to_pool(void*(*routine)(void*), void *arg);
#endif
