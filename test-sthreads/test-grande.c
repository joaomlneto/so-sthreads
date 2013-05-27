/* 
 * test-create.c
 *
 * Simple test of thread create
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sthread.h>

#define N 5
#define TAMANHO 10

void *thread_start(void *);
void *soma_linha (void *);

int success = 0;
int buffer[N][TAMANHO];

int main(int argc, char **argv)
{
	sthread_t tid[N];
	int i, j;

	for(i=0; i<N; i++) {
		for(j=0; j<(TAMANHO-1); j++) {
			buffer[i][j] = rand()%10;
		}
	}

	for(i=0; i<N; i++) {
		for(j=0; j<TAMANHO; j++) {
			printf(" %3d ", buffer[i][j]);
		}
		printf("linha %d \n", i);
	}

	printf("Testing sthread_join, impl: %s\n",
		   (sthread_get_impl() == STHREAD_PTHREAD_IMPL) ? "pthread" : "user");
	
	sthread_init();
    
	for(i=0; i<N; i++) {
		if ((tid[i] = sthread_create(soma_linha, (void*)&(buffer[i]), 1)) == NULL) {
			printf("ERRO NA CRIACAO DA TAREFA\n");
			exit(-1);
		}
	}
    
	for(i=0; i<N; i++) {
		sthread_join(tid[i], NULL);
	}

	printf("back in main\n");

	for(i=0; i<N; i++) {
		for(j=0; j<TAMANHO; j++) {
			printf(" %3d ", buffer[i][j]);
		}
		printf("linha %d \n", i);
	}

	return 0;
}


void *soma_linha(void *ptr) {
	int indice = 0;
	int soma = 0;
	int *b = (int *) ptr;
	while(indice < TAMANHO - 1) {
		soma += b[indice++];
	}
	b[indice] = soma;
	return NULL;
}
