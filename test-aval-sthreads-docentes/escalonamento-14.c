/*
 * Threads with diffferent priorities in sleep, mutex and monitor to see dump
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
void *thread_4(void *);
void *thread_5(void *);
void *thread_6(void *);


static sthread_mutex_t mutex;
static sthread_mon_t mon = NULL;

int main(int argc, char **argv)
{
  sthread_init();
  mutex = sthread_mutex_init();
  mon = sthread_monitor_init();

  if (sthread_create(thread_0, (void*)0, 2) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_1, (void*)0, 3) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_2, (void*)0, 4) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_3, (void*)0, 5) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_4, (void*)0, 6) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_5, (void*)0, 7) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_6, (void*)0, 8) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  printf("created threads\n");

  sthread_sleep(2000000);
  
  sthread_dump();

  printf("dump should be:\n1 active thread with priority 1\n1 executable thread with priority 2\n3 threads in sleep\n1 thread in mutex\n1 thread in wait of monitor\nand 1 thread waiting to enter monitor\n");

  return 0;
}


void *thread_0(void *arg)
{
  // will be in rb tree
  for (;;);

  return 0;
}

void *thread_1(void *arg)
{
  // will be waiting in sleep
  printf("ok1\n");
  sthread_sleep(3000000);
  printf("ok1\n");

  return 0;
}

void *thread_2(void *arg)
{
  sthread_monitor_enter(mon);

  // will be waiting in wait
  printf("ok2\n");
  sthread_monitor_wait(mon);
  printf("ok2\n");

  sthread_monitor_exit(mon);

  return 0;
}

void *thread_3(void *arg)
{
  sthread_monitor_enter(mon);

  // will be waiting in sleep
  printf("ok3\n");
  sthread_sleep(3000000);
  printf("ok3\n");
  sthread_monitor_signal(mon);

  sthread_monitor_exit(mon);

  return 0;
}

void *thread_4(void *arg)
{
  sthread_sleep(1000000);

  // will be waiting in here
  printf("ok4\n");
  sthread_monitor_enter(mon);
  printf("ok4\n");

  sthread_monitor_exit(mon);

  return 0;
}

void *thread_5(void *arg)
{
  sthread_mutex_lock(mutex);

  // will be waiting in sleep
  printf("ok5\n");
  sthread_sleep(3000000);
  printf("ok5\n");

  sthread_mutex_unlock(mutex);

  return 0;
}


void *thread_6(void *arg)
{
  sthread_sleep(1000000);

  // will be waiting in here
  printf("ok6\n");
  sthread_mutex_lock(mutex);
  printf("ok6\n");

  sthread_mutex_unlock(mutex);

  return 0;
}


