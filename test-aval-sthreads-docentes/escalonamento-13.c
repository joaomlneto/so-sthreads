/*
 * Threads with different priorities in sleep to see dump
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>

void *thread_0(void *);
void *thread_1(void *);
void *thread_2(void *);
void *thread_3(void *);

int main(int argc, char **argv)
{
  sthread_init();

  if (sthread_create(thread_0, (void*)0, 2) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_1, (void*)0, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_2, (void*)0, 5) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_3, (void*)0, 4) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  printf("created threads\n");

  sthread_sleep(1000000);
  
  sthread_dump();

  printf("dump should be:\n1 active thread with priority 1\n1 executable thread with priority 2 and 3 threads in sleep 1000000, 1500000, 2000000\n");

  return 0;
}


void *thread_0(void *arg)
{
  for (;;);

  return 0;
}

void *thread_1(void *arg)
{
  sthread_sleep(2000000);

  return 0;
}

void *thread_2(void *arg)
{
  sthread_sleep(2500000);

  return 0;
}

void *thread_3(void *arg)
{
  sthread_sleep(3000000);

  return 0;
}


