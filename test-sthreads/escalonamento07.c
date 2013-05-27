/* 
 * Testing scheduler
 *
 * 50 threads launched that wait for signalall
 * 
 */

#include <sys/time.h> // to debug timer
#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

void *thread0(void *);
void *thread1(void *);

#define NUMBER 10

sthread_t thr[NUMBER];

static sthread_mon_t mon1 = NULL;
static sthread_mon_t mon2 = NULL;

int main(int argc, char **argv)
{
  void *ret;
  int i;

  printf("Testing sthreads, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();

  mon1 = sthread_monitor_init();
  mon2 = sthread_monitor_init();

    
  if (sthread_create(thread0, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  sthread_monitor_enter(mon1);
  for (i = 0; i < NUMBER; i++){
    if ((thr[i] = sthread_create(thread1, (void*)i, 10)) == NULL) {
      printf("sthread_create failed\n");
      exit(-1);
    }
    sthread_yield();
  }
  for (i = 0; i < NUMBER; i++){
    sthread_monitor_wait(mon1);
  }
    
  printf("in main\n");
  
  sthread_monitor_exit(mon1);
  
  sthread_sleep(10000);

  sthread_monitor_enter(mon2);
  sthread_monitor_signalall(mon2);
  sthread_monitor_exit(mon2);
  
  for (i = 0; i < NUMBER; i++){
    sthread_join(thr[i], &ret);
  }
  printf("\nSUCCESS in creating %i threads\n", NUMBER);
  printf("out main\n");

  return 0;
}


void *thread0(void *arg)
{
  printf("thread %d started\n", *(int*)arg);
  while(1);
}

void *thread1(void *arg)
{
  int i = *(int*)arg;
  printf("thread %d started\n", i);
  sthread_monitor_enter(mon1);
  printf("thread %d enter mon1\n", i);
  sthread_monitor_signal(mon1);
  printf("thread %d signal mon1\n", i);
  sthread_monitor_exit(mon1);
  printf("thread %d exit mon1\n", i);
  sthread_monitor_enter(mon2);
  printf("thread %d enter mon2\n", i);
  sthread_monitor_wait(mon2);
  printf("thread %d wait mon2\n", i);
  sthread_monitor_exit(mon2);
  printf("end %i\n", *(int*)arg);
  return 0;
}

