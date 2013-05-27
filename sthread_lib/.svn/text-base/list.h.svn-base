#ifndef LIST_H
#define LIST_H

typedef struct list_node{
	void * key;
	struct list_node * next;
} list_node;

typedef struct list{
	list_node * first;
} list;

list * list_create();
void list_insert(list *, void *);
void list_remove(list *, void *);
void list_destroy(list *);

#endif
