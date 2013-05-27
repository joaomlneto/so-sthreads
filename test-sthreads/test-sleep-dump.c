/* 
 * test-create.c
 *
 * Simple test of thread create
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>


void *thread_start(void *);


int success = 0;


int main(int argc, char **argv)
{
	printf("Testing sthread_create, impl: %s\n",
		   (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
	sthread_init();
    
	if (sthread_create(thread_start, (void*)1, 1) == NULL) {
		printf("sthread_create failed\n");
		exit(-1);
	}
    
	sthread_yield();
	sthread_sleep(2000000);
	sthread_dump();
	printf("back in main\n");
	return 0;
}


void *thread_start(void *arg)
{
	printf("In thread_start, arg = %d\n", (int)arg);
	sthread_sleep(10000000);
	sthread_dump();
	return 0;
}