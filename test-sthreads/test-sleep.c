/* 
 * test-sleep.c
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
  int i;

  printf("Testing sthread_sleep, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
  
  sthread_init();
  
  if ((thr = sthread_create(thread_start, (void*)1, 1)) == NULL) {
    exit(-1);
  }
  
  for (i = 1; i <= 15; i++){
    sthread_sleep(1000000);
    printf("elapsed %i secs\n", i);
  }

  return 0;
}


void *thread_start(void *arg)
{
  for(;;);
}
