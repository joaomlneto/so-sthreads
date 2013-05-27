/* 
 * escalonamneto-08.c
 *
 * sthread_sleep test
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <sthread.h>
#include <time.h>


void *thread0(void *);
void *thread1(void *);

int passed0;
int passed1;



int main(int argc, char **argv)
{
  int t;

  sthread_init();
    
  if (sthread_create(thread0, (void*)1, 2) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
  if (sthread_create(thread1, (void*)1, 5) == NULL) {
    printf("sthread_create failed\n");
    exit(-1);
  }
    
  t = time(NULL);
    
  while(!passed0 || !passed1);

  t = time(NULL) - t;

  printf("time: %i sec\n", t);
  if (t <= 32 && t >= 28){
    printf("PASSED\n");
  }

  return 0;
}


void *thread0(void *arg)
{
  int i;

  for(i = 1; i < 10; i++){
    sthread_sleep(1000000);
    printf("%i sec\n", i);
  }

  passed0 = 1;

  return 0;
}

void *thread1(void *arg)
{
  int i;

  for(i = 1; i <= 3; i++){
    sthread_sleep(10000000);
    printf("%i0 sec\n", i);
  }

  passed1= 1;

  return 0;
}

