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
    
	if ((thr = sthread_create(thread_start, (void*)1, 1)) == NULL) {
		exit(-1);
	}
    
	sthread_join(thr, &ret);
	printf("back in main\n");
	sthread_dump();
	return 0;
}


void *thread_start(void *arg)
{
  int i;
	printf("In thread_start, arg = %d\n", (int)arg);
	  for(i = 0; i < 300000000; i++)
	    if((i % 3000000) == 0) printf("%d\n", i/3000000);
	printf("100! In thread_start, ending\n");
	return 0;
}
