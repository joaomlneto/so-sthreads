#include "list.h"
#include <stdlib.h>

list * list_create(){
	list * l = malloc(sizeof(list));
	l->first = NULL;
	return l;
}

void list_insert(list * l, void * key){
	list_node * new = malloc(sizeof(list_node));
	new->key = key;
	new->next = l->first;
	l->first = new;
}

void list_remove(list * l, void * key){
	list_node *aux, *aux2;
	/* Lista vazia */
	if(l->first == NULL) return;
	aux = l->first;
	aux2 = aux->next;
	/* Verificar primeiro elemento */
	if(l->first->key == key) {
		free(l->first);
		l->first = aux2;
		return;
	}
	/* Verificar resto da lista */
	while((aux2 != NULL) && (aux2->key != key)) {
		aux = aux2;
		aux2 = aux2->next;
	}
	if(aux2->key == key) {
		aux->next = aux2->next;
		free(aux2);
	}
}

void destroy(list * l){
	list_node * aux, * aux2;	
	if(l->first == NULL)
		free(l);
	else{
		aux = l->first;
		aux2 = aux;
		while(aux != NULL){
			aux2 = aux->next;
			free(aux);
			aux = aux2;
		}
		free(l);
	}
}
