/* 
 * escalonamneto-07.c
 *
 * sthread_nice test
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>


void *thread0(void *);
void *thread1(void *);
void *thread2(void *);
void *thread3(void *);

int prio0;
int prio1;
int prio2;
int prio3;

int main(int argc, char **argv)
{
  int i;

  sthread_init();
    
  if (sthread_create(thread0, (void*)1, 2) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
  if (sthread_create(thread1, (void*)1, 5) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  if (sthread_create(thread2, (void*)1, 5) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  if (sthread_create(thread3, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
    
  for(i=0;i<300000000;i++);


  printf("prio0: %i\n", prio0);
  printf("prio1: %i\n", prio1);
  printf("prio2: %i\n", prio2);
  printf("prio3: %i\n", prio3);

  if (prio0 == 7 && prio1 == 10 && prio2 == 7 && prio3 == 13)
    printf("PASSED\n");

  return 0;
}


void *thread0(void *arg)
{
  prio0 = sthread_nice(5);

  printf("thread 0 done\n");
  return 0;
}

void *thread1(void *arg)
{
  prio1 = sthread_nice(5);

  printf("thread 1 done\n");
  return 0;
}

void *thread2(void *arg)
{
  prio2 = sthread_nice(2);

  printf("thread 2 done\n");
  return 0;
}

void *thread3(void *arg)
{
  prio3 = sthread_nice(2);
  prio3 = sthread_nice(3);

  printf("thread 3 done\n");
  return 0;
}
