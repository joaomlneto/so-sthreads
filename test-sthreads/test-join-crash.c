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
  sthread_t thr;
  void *ret;

	printf("Testing sthread_join, impl: %s\n",
		   (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
	sthread_init();

	sthread_yield();
    
	if ((thr = sthread_create(thread_start, (void*)1, 1)) == NULL) {
		exit(-1);
	}
    
	sthread_join(thr, &ret);
	printf("back in main\n");
	return 0;
}


void *thread_start(void *arg)
{
	printf("In thread_start, arg = %d\n", (int)arg);
	sthread_sleep(10000000);
	printf("In thread_start, ending\n");
	return 0;
}
