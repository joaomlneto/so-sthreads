#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>

/* TESTES DO ESCALONADOR
 * verifica se a cedência do processador sobe a prioridade
 *
 * Joao Garcia, 2008 */

int main(int argc, char **argv) {
  printf("Testing sthread_mutex_*, impl: %s\n",
      (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
  
  sthread_init();
  //dump_stats("dump0.tsv");
  sthread_yield(); /*retirar no fim*/
  //dump_stats("dump1.tsv");
  sthread_yield(); /*retirar no fim*/
  //dump_stats("dump2.tsv");
  sthread_yield(); /*retirar no fim*/
  //dump_stats("dump3.tsv");
  sthread_yield(); /*retirar no fim*/
  //dump_stats("dump4.tsv");
  printf("\nteste concluido: consultar dump*.tsv\n");
  return 1;
}
