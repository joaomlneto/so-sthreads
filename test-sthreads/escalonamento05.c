/* 
 * Testing scheduler
 *
 * Diferent quantum and diferent priority after blocking
 * check to see that dynamic thread priority does not go below 5
 */

#include <sys/time.h> // to debug timer
#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

void *thread0(void *);
void *thread1(void *);
void *thread2(void *);


static sthread_mon_t mon = NULL;

int main(int argc, char **argv)
{
  printf("Testing sthreads, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();

  mon = sthread_monitor_init();
    
  if (sthread_create(thread0, (void*)1, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  if (sthread_create(thread1, (void*)1, 6) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  if (sthread_create(thread2, (void*)1, 0) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }

  printf("in main\n");
  
  sthread_sleep(100000);
  
  printf("out main\n");

  return 0;
}


void *thread0(void *arg)
{
  while(1);
}

void *thread1(void *arg)
{
  printf("\nactive thread with priorities 6 and quantum 5\n");
  sthread_dump();
  sthread_monitor_enter(mon);
  sthread_monitor_signal(mon);
  sthread_monitor_wait(mon);
  sthread_monitor_exit(mon);
  printf("\nactive thread with priorities 5 and quantum 7\n");
  sthread_dump();

  return 0;
}

void *thread2(void *arg)
{
  sthread_monitor_enter(mon);
  sthread_monitor_wait(mon);
  sthread_monitor_exit(mon);
  
  sthread_sleep(10000);

  sthread_monitor_enter(mon);
  sthread_monitor_signal(mon);
  sthread_monitor_exit(mon);

  return 0;
}
