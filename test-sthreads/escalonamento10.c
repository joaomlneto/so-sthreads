/* 
 * Testing scheduler
 *
 * testing preemption
 * 
 */

#include <sys/time.h> // to debug timer
#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>

sthread_t thr[3];

void *thread0(void *);
void *thread1(void *);
void *thread2(void *);

static sthread_mon_t mon = NULL;

int main(int argc, char **argv)
{
  void *ret;

  printf("Testing sthreads, impl: %s\n",
	 (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
  sthread_init();

  mon = sthread_monitor_init();
    
  if ((thr[0] = sthread_create(thread0, (void*)0, 10)) == NULL) {
      printf("sthread_create failed\n");
      exit(-1);
  }    

  if ((thr[1] = sthread_create(thread1, (void*)0, 10)) == NULL) {
      printf("sthread_create failed\n");
      exit(-1);
  }    

  if ((thr[2] = sthread_create(thread2, (void*)0, 5)) == NULL) {
      printf("sthread_create failed\n");
      exit(-1);
  }    


  printf("in main\n");
  sthread_join(thr[1], &ret);
  sthread_join(thr[2], &ret);

  printf("\nif order was wrong preemption not working\n");
  printf("\n1st and 2nd: test preemption working\n");
  printf("3rd and 4th: test that the previous situation is not the default\n");
  printf("out main\n");

  return 0;
}


void *thread1(void *arg)
{
  sthread_monitor_enter(mon);
  sthread_monitor_signal(mon);
  sthread_monitor_exit(mon);
  printf("2nd\n");
  sthread_monitor_enter(mon);
  sthread_monitor_wait(mon);
  sthread_monitor_exit(mon);
  printf("4th\n");
  return 0;
}

void *thread2(void *arg)
{
  sthread_monitor_enter(mon);
  sthread_monitor_wait(mon);
  sthread_monitor_exit(mon);
  printf("1st\n");

  //sthread_sleep(100000);

  sthread_monitor_enter(mon);
  sthread_monitor_signal(mon);
  sthread_monitor_exit(mon);

  printf("3rd\n");
  return 0;
}
  

void *thread0(void *arg)
{
  while(1);
}
