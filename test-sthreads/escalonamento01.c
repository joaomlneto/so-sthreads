/* 
 * Testing scheduler
 *
 * Equal quantum and equal priority
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

void *thread1(void *);
void *thread2(void *);

int main(int argc, char **argv)
{
  printf("Testing sthread_create, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();
    
  if (sthread_create(thread1, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  if (sthread_create(thread2, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  printf("in main\n");
  sthread_sleep(10000);
  
  printf("\ntwo threads runqueue active in priority 10\n");
  sthread_dump();
  sthread_sleep(10000);

  printf("\ntwo threads runqueue active in priority 10\n");
  sthread_dump();

  printf("out main\n");

  return 0;
}


void *thread1(void *arg)
{
  while(1){
    ;
  }
}

void *thread2(void *arg)
{
  while(1){
    ;
  }
}
