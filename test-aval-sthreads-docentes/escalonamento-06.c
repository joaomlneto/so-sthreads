/*
 * Each thread fills certain positions of the array with its id
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>

static int counter = 0;
static int complete = 0;

void *thread(void *);

#define MAX 100
#define NUMTHREADS 50

int a[MAX];

int main(int argc, char **argv)
{
  int i;
  int passed;

  sthread_init();

  for (i = 0; i < NUMTHREADS; i++){
    if (sthread_create(thread, (void*)i, 1) == NULL) {
      printf("sthread_create failed\n");
      exit(1);
    }
  }

  printf("created threads\n");

  while(!complete);

  printf("completed threads\n");

  passed = 1;
  for (i = 0; i < MAX; i++){
    if (a[i] != i % NUMTHREADS)
      passed = 0;
  }

  if (passed)
    printf("PASSED\n");
  return 0;
}


void *thread(void *arg)
{
  while(counter<MAX) {
     
    if (counter % NUMTHREADS == (int)arg){
      if (counter < MAX){
	a[counter] = (int)arg;
	counter++;
	printf("%i\n", counter);
      }
    }
  }

   complete = 1;

   return 0;
}



