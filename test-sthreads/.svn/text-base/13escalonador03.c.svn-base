#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>
#include <values.h>

/* TESTES DE MONITORES
 * verifica se o consumo de CPU baixa a prioridade e 
 * se a cedencia do CPU sobe a prioridade.
 *
 * Joao Garcia, 2008 */
void * thread_start(void * arg) {
  int i;

  //dump_stats("dump0.tsv");
  for (i = 0; i < 3000000; i++) {}
  //dump_stats("dump1.tsv");
  for (i = 0; i < 3000000; i++) {}
  //dump_stats("dump2.tsv");
  for (i = 0; i < 3000000; i++) {}
  //dump_stats("dump3.tsv");
  for (i = 0; i < 3000000; i++) {}
  //dump_stats("dump4.tsv");
  sthread_yield();
  //dump_stats("dump5.tsv");
  sthread_yield();
  //dump_stats("dump6.tsv");
  sthread_yield();
  //dump_stats("dump7.tsv");
  return NULL;
}


int main(int argc, char **argv) {
  sthread_t thread = NULL;
  int * res = (int *) malloc(sizeof(int));

  printf("Testing sthread_mutex_*, impl: %s\n",
      (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");

  sthread_init();
  if ((thread = sthread_create(thread_start, (void *) NULL, 1)) == NULL) {
    printf("sthread_create failed\n");
    exit(1);
  }
  sthread_join(thread, (void**)&res);
  printf("\nteste concluido: consultar dump*.tsv\n");
  return 1;
}
