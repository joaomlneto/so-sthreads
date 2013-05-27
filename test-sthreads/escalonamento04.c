/* 
 * Testing scheduler
 *
 * Equal quantum and different priority
 * 
 */

#include <sys/time.h> // to debug timer
#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

void *thread0(void *);
void *thread1(void *);
void *thread2(void *);
void *thread3(void *);
void *thread4(void *);
void *thread5(void *);


int main(int argc, char **argv)
{
  printf("Testing sthreads, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();

  if (sthread_create(thread0, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  if (sthread_create(thread4, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  if (sthread_create(thread2, (void*)1, 3) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  if (sthread_create(thread1, (void*)1, 2) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  if (sthread_create(thread5, (void*)1, 13) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  if (sthread_create(thread3, (void*)1, 5) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  printf("in main\n");
  printf("\norder should be correct\n");
  sthread_sleep(100000);
  printf("out main\n");

  return 0;
}


void *thread0(void *arg)
{
  while (1){
    ;
  }
  return 0;
}

void *thread1(void *arg)
{
  printf("1th\n");
  return 0;
}

void *thread2(void *arg)
{
  printf("2nd\n");
  return 0;
}

void *thread3(void *arg)
{
  printf("3rd\n");
  return 0;
}

void *thread4(void *arg)
{
  printf("4th\n");
  return 0;
}

void *thread5(void *arg)
{
  printf("5th\n");
  return 0;
}


