/* 
 * Testing scheduler
 *
 * No thread in runqueues
 * 
 */

#include <sys/time.h> // to debug timer
#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

void *thread1(void *);


int main(int argc, char **argv)
{
  printf("Testing sthreads, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();

  if (sthread_create(thread1, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  printf("in main\n");
  
  sthread_sleep(10000);
  
  printf("SUCCESS: no thread in runqueue works\n");
  printf("out main\n");

  return 0;
}


void *thread1(void *arg)
{
  printf("in thread\n");
  sthread_sleep(10000);
  printf("out thread\n");

  return 0;
}


