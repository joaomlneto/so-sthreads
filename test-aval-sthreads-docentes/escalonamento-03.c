/*
 * Threads with different priorities
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>

static int counter1 = 0;
static int counter2 = 0;

void *thread_1(void *);
void *thread_2(void *);
void *thread_3(void *);

int main(int argc, char **argv)
{
  int iterations;
  int counter0 = 0;
  int c1, c2;

  if (argc == 1){
    printf("usage: escalonamento-03 <number of iteration>\n");
    exit(1);
  }
    
  iterations = atoi(argv[1]);

  sthread_init();

  if (sthread_create(thread_1, (void*)0, 10) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  if (sthread_create(thread_2, (void*)0, 1) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  printf("created threads\n");

  for(; iterations > 0; iterations--)
    for(; counter0 < 300000000; counter0++);

  c1 = counter1;
  c2 = counter2;
  printf("1: %i, 2: %i\n", c1, c2);
  printf("2/1: %f\n", ((float)c2)/c1);

  if (c2/c1 >= 6 && c2/c1 <= 14)
    printf("PASSED\n");

  return 0;
}


void *thread_1(void *arg)
{
  int i;

  for (;;counter1++)
    for (i = 0; i < 1000000; i++);
  
  return 0;
}


void *thread_2(void *arg)
{
  int i;

  for (;;counter2++)
    for (i = 0; i < 1000000; i++);

  return 0;
}


