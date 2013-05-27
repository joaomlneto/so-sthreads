#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sthread.h>
#include <values.h>

/* TESTES DE MUTEXES
 * verifica que o lock impede um conjunto de tarefas de avançar
 * e que o unlock as liberta correctamente 
 *
 * Joao Garcia, 2008 */


#define TOTAL_TAREFAS 10

static sthread_mutex_t mutex = NULL;
static int sucesso = -1;

void *thread_start(void *);

int main(int argc, char **argv) {
  int j=0;
  int arg[TOTAL_TAREFAS];
  printf("Testando sthread_mutex_*, impl: %s\n",
      (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
  
  sthread_init();

  mutex = sthread_mutex_init();
  
  printf("thread principal antes do mutex\n");
  sthread_mutex_lock(mutex);
  sucesso = 0;
  printf("thread principal dentro do mutex\n");
  for (j=0; j< TOTAL_TAREFAS; j++) {
    arg[j] = TOTAL_TAREFAS - j;
    if (sthread_create(thread_start, (void*)&arg[j], 1) == NULL) {
      printf("sthread_create falhou\n");
      exit(1);
    }
  }
  for(j=0;j< 300000000;j++) {}
  sthread_mutex_unlock(mutex);
  printf("thread principal depois do mutex\n");
  for(j=0;j< MAXINT;j++) {}
  if (sucesso == TOTAL_TAREFAS) {
    printf("\nSUCESSO!\n");
  } else {
    printf("resultado = %i, teste falhado...\n", sucesso);
  }
  return 1;
}


void *thread_start(void *arg) {
  int id = *((int *)arg);

  while (1) {
    printf("thread filha %i antes do mutex\n", id);
    sthread_mutex_lock(mutex);
    printf("thread filha %i dentro do mutex\n", id);
    if (sucesso == id - 1) {
      sucesso = id;
      sthread_mutex_unlock(mutex);
      return 0;
    }
    sthread_mutex_unlock(mutex);
    printf("thread filha %i depois do mutex\n", id);
    sthread_yield();
    }
  return 0;
}
