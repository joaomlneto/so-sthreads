/*
 * Threads with different priorities II
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>

static int counter1 = 0;
static int counter2 = 0;
static int counter3 = 0;

void *thread_1(void *);
void *thread_2(void *);
void *thread_3(void *);

int main(int argc, char **argv)
{
  int iterations;
  int counter0 = 0;
  int c1, c2, c3;

  if (argc == 1){
    printf("usage: escalonamento-05 <number of iteration>\n");
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

  if (sthread_create(thread_3, (void*)0, 1) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }

  printf("created threads\n");

  for(; iterations > 0; iterations--)
    for(; counter0 < 300000000; counter0++);

  c1 = counter1;
  c2 = counter2;
  c3 = counter3;
  printf("1: %i, 2: %i, 3: %i\n", c1, c2, c3);
  printf("2/1: %f\n", ((float)c2)/c1);
  printf("2/3: %f\n", ((float)c2)/c3);

  if (c2/c1 >= 6 && c2/c1 <= 14)
    if (c2/c3 >= 0 && c2/c3 <= 2)
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

void *thread_3(void *arg)
{
  int i;

  for (;;counter3++)
    for (i = 0; i < 1000000; i++);

  return 0;
}


