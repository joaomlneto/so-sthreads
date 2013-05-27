/* 
 * Testing scheduler
 *
 * Modifying nice
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

void *thread1(void *);

int a[2];
int b[2];

int main(int argc, char **argv)
{
  printf("Testing sthread_create, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();
    
  if (sthread_create(thread1, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  printf("in main\n");
  sthread_sleep(100000);
  printf("\ntarefa 2 com prioridade 13 (ou 10 outra vez)...\n");
  sthread_dump();
  printf("\nout main\n");

  return 0;
}


void *thread1(void *arg)
{
  sthread_nice(3);
  printf("\ntarefa 2 com prioridade 10...\n");
  sthread_dump();
  while(1);
}

