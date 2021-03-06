/* Simplethreads Instructional Thread Package
 * 
 * sthread_user.c - Implements the sthread API using user-level threads.
 *
 *    You need to implement the routines in this file.
 *
 * Change Log:
 * 2002-04-15        rick
 *   - Initial version.
 * 2005-10-12        jccc
 *   - Added semaphores, deleted conditional variables
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <sthread.h>
#include <sthread_user.h>
#include <sthread_ctx.h>
#include <sthread_time_slice.h>
#include <sthread_user.h>
#include "red_black_tree.h"
#include "queue.h"
#include "list.h"


struct _sthread {
  int tid;
  int vruntime;
  int priority;
  int nice;
  sthread_ctx_t *saved_ctx;
  sthread_start_func_t start_routine_ptr;
  long wake_time;
  int join_tid;
  void* join_ret;
  void* args;
  long clock;
  long runtime;
  long sleeptime;
  long waittime;
};

/* Mutex */
struct _sthread_mutex
{
  lock_t l;
  struct _sthread *thr;
  queue_t* queue;
};

/* Monitor */
struct _sthread_mon {
 	sthread_mutex_t mutex;
	queue_t* queue;
};


static list * all_threads;
static list * mutex_list; /*lista de mutexes*/
static list * monitor_list;						 /*lista de monitores */
static rb_red_blk_tree *exe_thr_list;         /* lista de threads executaveis */
static queue_t *dead_thr_list;        /* lista de threads "mortas" */
static queue_t *sleep_thr_list;
static queue_t *join_thr_list;
static queue_t *zombie_thr_list;
static struct _sthread *active_thr;   /* thread activa */
static int tid_gen;                   /* gerador de tid's */
static int yield_delay;

#define CLOCK_TICK 10000
static long Clock;


/*********************************************************************/
/* Part 1: Creating and Scheduling Threads                           */
/*********************************************************************/


void sthread_user_free(struct _sthread *thread);

void sthread_aux_start(void){
  splx(LOW);
  active_thr->start_routine_ptr(active_thr->args);
  sthread_user_exit((void*)0);
}

void sthread_user_dispatcher(void);

void sthread_user_init(void) {

  all_threads = list_create();
  monitor_list = list_create(); 
  mutex_list = list_create();
  exe_thr_list = RBTreeCreate(IntComp, IntDest, InfoDest, IntPrint, InfoPrint);
  dead_thr_list = create_queue();
  sleep_thr_list = create_queue();
  join_thr_list = create_queue();
  zombie_thr_list = create_queue();
  tid_gen = 1;

  struct _sthread *main_thread = malloc(sizeof(struct _sthread));
  main_thread->start_routine_ptr = NULL;
  main_thread->args = NULL;
  main_thread->saved_ctx = sthread_new_blank_ctx();
  main_thread->wake_time = 0;
  main_thread->join_tid = 0;
  main_thread->join_ret = NULL;
  main_thread->tid = tid_gen++;
  main_thread->priority = 1;
  main_thread->nice = 0;
  main_thread->clock = 1;
  main_thread->runtime = 1;
  main_thread->sleeptime = 0;
  main_thread->waittime = 0;
  main_thread->vruntime = 0;
  
  active_thr = main_thread;

  yield_delay = 0;

  Clock = 1;
  sthread_time_slices_init(sthread_user_dispatcher, CLOCK_TICK);
}

sthread_t sthread_user_create(sthread_start_func_t start_routine, void *arg, int priority)
{
  struct _sthread *new_thread = (struct _sthread*)malloc(sizeof(struct _sthread));
  sthread_ctx_start_func_t func = sthread_aux_start;
  new_thread->args = arg;
  new_thread->start_routine_ptr = start_routine;
  new_thread->wake_time = 0;
  new_thread->join_tid = 0;
  new_thread->join_ret = NULL;
  new_thread->saved_ctx = sthread_new_ctx(func);
  new_thread->nice = 0;
  new_thread->clock = Clock;
  new_thread->runtime = 0;
  new_thread->sleeptime = 0;
  new_thread->waittime = 0;
  new_thread->vruntime = newVruntime(all_threads);

  if((priority >= 1) && (priority <= 10))
    new_thread->priority = priority;
  else
    new_thread->priority = 1;

  splx(HIGH);
  new_thread->tid = tid_gen++;
  RBTreeInsert(exe_thr_list, &(new_thread->vruntime), new_thread);
  list_insert(all_threads, new_thread);
  splx(LOW);
  return new_thread;
}


void sthread_user_exit(void *ret) {
  splx(HIGH);
   
   int is_zombie = 1;

   // unblock threads waiting in the join list
   queue_t *tmp_queue = create_queue();   
   while (!queue_is_empty(join_thr_list)) {
      struct _sthread *thread = queue_remove(join_thr_list);
      //printf("Test join list: join_tid=%d, active->tid=%d\n", thread->join_tid, active_thr->tid);

      if (thread->join_tid == active_thr->tid) {
         thread->join_ret = ret;
         thread->clock = Clock;
         RBTreeInsert(exe_thr_list, &(thread->vruntime), thread);
         is_zombie = 0;
      } else {
         queue_insert(tmp_queue,thread);
      }
   }
   delete_queue(join_thr_list);
   join_thr_list = tmp_queue;
 
   if (is_zombie) {
      queue_insert(zombie_thr_list, active_thr);
   } else {
      queue_insert(dead_thr_list, active_thr);
   }
   

   if(all_threads->first == NULL/*exe_thr_list->root->left->info == NULL*/){  /* pode acontecer se a unica thread em execucao fizer */
    free(exe_thr_list);              /* sthread_exit(0). Este codigo garante que o programa sai bem. */
    delete_queue(dead_thr_list);
    sthread_user_free(active_thr);
    printf("Exec queue is empty!\n");
    exit(0);
  }

  
   // remove from exec list
   struct _sthread *old_thr = active_thr;
   active_thr = RBDelete(exe_thr_list);
   checkNoThreads();
   active_thr->clock = Clock;
   sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);
   list_remove(all_threads, old_thr);

   splx(LOW);
}


void sthread_user_dispatcher(void)
{
   Clock++;
   queue_t *tmp_queue = create_queue();
   queue_element_t *qe;
   list_node * list_node_aux;
   sthread_mutex_t mutex_aux;
   sthread_mon_t mon_aux;

   active_thr->vruntime += active_thr->priority + active_thr->nice; // Actualizar vruntime

   // Proteccao de overflows do vruntime
   if(active_thr->vruntime > VRUNTIME_MAX)
      decrementVruntime(all_threads, newVruntime(all_threads));

   /* Percorrer sleep_thr_list */
   while (!queue_is_empty(sleep_thr_list)) {
      struct _sthread *thread = queue_remove(sleep_thr_list);
      thread->sleeptime++; // Actualizar sleeptime das threads em sleep
      
      if (thread->wake_time == Clock) {
         thread->wake_time = 0;
         thread->clock = Clock;
         RBTreeInsert(exe_thr_list, &(thread->vruntime), thread);
      } else {
         queue_insert(tmp_queue,thread);
      }
   }
   delete_queue(sleep_thr_list);
   sleep_thr_list = tmp_queue;

   /* Percorrer join_thr_list */
   qe = join_thr_list->first;
   while (qe != NULL) {
      qe->thread->sleeptime++; // Actualizar sleeptime
      qe = qe->next;
   }

   /* Percorrer mutex queues */
   for(list_node_aux = mutex_list->first; list_node_aux != NULL; list_node_aux = list_node_aux->next){
      mutex_aux = (sthread_mutex_t) list_node_aux->key;
      queue_t *aux = create_queue();
      while (!queue_is_empty(mutex_aux->queue)) {
         struct _sthread *thread = queue_remove(mutex_aux->queue);
         thread->sleeptime++;
         queue_insert(aux, thread);
      }
      delete_queue(mutex_aux->queue);
      mutex_aux->queue = aux;
   }

   /* Percorrer monitor queues */
   for(list_node_aux = monitor_list->first; list_node_aux != NULL; list_node_aux = list_node_aux->next) {
      mon_aux = (sthread_mon_t)list_node_aux->key;
      queue_t *aux = create_queue();
      while (!queue_is_empty(mon_aux->queue)) {
         struct _sthread *thread = queue_remove(mon_aux->queue);
         thread->sleeptime++;
         queue_insert(aux, thread);
      }
      delete_queue(mon_aux->queue);
      mon_aux->queue = aux;
      // Ver lista do mutex do monitor
      aux = create_queue();
      while(!queue_is_empty(mon_aux->mutex->queue)) {
         struct _sthread *thread = queue_remove(mon_aux->mutex->queue);
         thread->sleeptime++;
         queue_insert(aux, thread);
      }
      delete_queue(mon_aux->mutex->queue);
      mon_aux->mutex->queue = aux;
   }

   if((yield_delay < MIN_DELAY) && (active_thr->tid >= 1)) {
      InorderTreeIncrement(exe_thr_list); // Actualizar waittime
      yield_delay++;
   }
   else {
      //printf("active_thr->vruntime = %d || min_vruntime = %d\n", active_thr->vruntime, newVruntime(all_threads));
      if(exe_thr_list->root->left->info != NULL) // Verificar se ha tarefas para colocar em execucao
         // Em caso afirmativo, verificar as diferencas entre vruntimes
         if(active_thr->vruntime > ((struct _sthread *)exe_thr_list->first->info)->vruntime)
            sthread_user_yield();
   }
}


void sthread_user_yield(void)
{
  splx(HIGH);
  struct _sthread *old_thr;
  InorderTreeIncrement(exe_thr_list); // Actualizar waittime
  old_thr = active_thr;
  old_thr->clock = Clock;
  if((old_thr->tid == 1) && (old_thr->priority == 8)) printf("POIS\n");
  if(old_thr->tid >= 1) /* Se for uma thread decente, inserir na arvore */
    RBTreeInsert(exe_thr_list, &(old_thr->vruntime), old_thr);
  active_thr = RBDelete(exe_thr_list);
  checkNoThreads();
  active_thr->clock = Clock;
  active_thr->runtime++; /* Actualizar runtime */
  sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);
  if(old_thr->tid < 1) /* Se for uma dummy thread, apagar */
    sthread_user_free(old_thr);
  yield_delay = 0;
  splx(LOW);
}




void sthread_user_free(struct _sthread *thread)
{
  sthread_free_ctx(thread->saved_ctx);
  free(thread);
}


/*********************************************************************/
/* Part 2: Join and Sleep Primitives                                 */
/*********************************************************************/

int sthread_user_join(sthread_t thread, void **value_ptr)
{
   /* suspends execution of the calling thread until the target thread
      terminates, unless the target thread has already terminated.
      On return from a successful pthread_join() call with a non-NULL 
      value_ptr argument, the value passed to pthread_exit() by the 
      terminating thread is made available in the location referenced 
      by value_ptr. When a pthread_join() returns successfully, the 
      target thread has been terminated. The results of multiple 
      simultaneous calls toa pthread_join() specifying the same target 
      thread are undefined. If the thread calling pthread_join() is 
      canceled, then the target thread will not be detached. 

      If successful, the pthread_join() function returns zero. 
      Otherwise, an error number is returned to indicate the error. */

   
   splx(HIGH);
   // checks if the thread to wait is zombie
   int found = 0;
   queue_t *tmp_queue = create_queue();
   while (!queue_is_empty(zombie_thr_list)) {
      struct _sthread *zthread = queue_remove(zombie_thr_list);
      if (thread->tid == zthread->tid) {
         if(value_ptr != NULL) *value_ptr = thread->join_ret;
         queue_insert(dead_thr_list,thread);
         found = 1;
      } else {
         queue_insert(tmp_queue,zthread);
      }
   }
   delete_queue(zombie_thr_list);
   zombie_thr_list = tmp_queue;
  
   if (found) {
       splx(LOW);
       return 0;
   }

   
   // search active queue
   if (active_thr->tid == thread->tid) {
      found = 1;
   }
   
   queue_element_t *qe = NULL;

   // search exe
/*   qe = exe_thr_list->first->info;
   while (!found && qe != NULL) {
      if (qe->thread->tid == thread->tid) {
         found = 1;
      }
      qe = qe->next;
   }*/
   found = RBSearch(exe_thr_list, exe_thr_list->root->left, thread);

   // search sleep
   qe = sleep_thr_list->first;
   while (!found && qe != NULL) {
      if (qe->thread->tid == thread->tid) {
         found = 1;
      }
      qe = qe->next;
   }

   // search join
   qe = join_thr_list->first;
   while (!found && qe != NULL) {
      if (qe->thread->tid == thread->tid) {
         found = 1;
      }
      qe = qe->next;
   }

   // if found blocks until thread ends
   if (!found) {
      splx(LOW);
      return -1;
   } else {
      active_thr->join_tid = thread->tid;
      
      struct _sthread *old_thr = active_thr;
      old_thr->clock = Clock;
      queue_insert(join_thr_list, old_thr);
      active_thr = RBDelete(exe_thr_list);

      checkNoThreads();

      active_thr->clock = Clock;
      sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);
  
      if(value_ptr != NULL) *value_ptr = thread->join_ret;
   }
   
   splx(LOW);
   return 0;
}


/* minimum sleep time of 1 clocktick.
  1 clock tick, value 10 000 = 10 ms */

int sthread_user_sleep(int time)
{
   splx(HIGH);
   
   long num_ticks = time / CLOCK_TICK;
   if (num_ticks == 0) {
      splx(LOW);
      
      return 0;
   }
   
   active_thr->wake_time = Clock + num_ticks;
   active_thr->clock = Clock;

   queue_insert(sleep_thr_list,active_thr); 
   sthread_t old_thr = active_thr;
   active_thr = RBDelete(exe_thr_list);
   checkNoThreads();

   sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);
   
   splx(LOW);
   return 0;
}

/* --------------------------------------------------------------------------*
 * Synchronization Primitives                                                *
 * ------------------------------------------------------------------------- */

/*
 * Mutex implementation
 */



sthread_mutex_t sthread_user_mutex_init()
{
  sthread_mutex_t lock;

  if(!(lock = malloc(sizeof(struct _sthread_mutex)))){
    printf("Error in creating mutex\n");
    return 0;
  }

  /* mutex initialization */
  lock->l=0;
  lock->thr = NULL;
  lock->queue = create_queue();
  
  list_insert(mutex_list, lock);

  return lock;
}

void sthread_user_mutex_free(sthread_mutex_t lock)
{

  list_remove(mutex_list, lock);
  delete_queue(lock->queue);
  free(lock);
}

void sthread_user_mutex_lock(sthread_mutex_t lock)
{
  while(atomic_test_and_set(&(lock->l))) {}

  if(lock->thr == NULL){
    lock->thr = active_thr;
    atomic_clear(&(lock->l));

  } else {
    active_thr->clock = Clock;
    queue_insert(lock->queue, active_thr);
    atomic_clear(&(lock->l));

    splx(HIGH);
    struct _sthread *old_thr;
    old_thr = active_thr;
    active_thr = RBDelete(exe_thr_list);
    checkNoThreads();
    active_thr->clock = Clock;
    sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);
    splx(LOW);
  }
}

void sthread_user_mutex_unlock(sthread_mutex_t lock)
{
  if(lock->thr!=active_thr){
    printf("unlock without lock!\n");
    return;
  }

  while(atomic_test_and_set(&(lock->l))) {}

  if(queue_is_empty(lock->queue)){
    lock->thr = NULL;
    atomic_clear(&(lock->l));
  } else {
    do {
       lock->thr = queue_remove(lock->queue);
       //printf("!");
    } while (lock->thr == NULL);
    atomic_clear(&(lock->l));

    if(lock->thr->vruntime < active_thr->vruntime){

	splx(HIGH);
	struct _sthread *old_thr;
	old_thr = active_thr;
	active_thr = lock->thr;
	checkNoThreads();
	old_thr->clock = Clock;
	RBTreeInsert(exe_thr_list, &(old_thr->vruntime), old_thr);
	sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);

	splx(LOW);
		

    }else{
	lock->thr->clock = Clock;
	RBTreeInsert(exe_thr_list, &(lock->thr->vruntime), lock->thr);
    }
  }


}

/*
 * Monitor implementation
 */



sthread_mon_t sthread_user_monitor_init()
{
  sthread_mon_t mon;
  if(!(mon = malloc(sizeof(struct _sthread_mon)))){
    printf("Error creating monitor\n");
    return 0;
  }

  mon->mutex = sthread_user_mutex_init();
  list_remove(mutex_list, mon->mutex);
  mon->queue = create_queue();
  list_insert(monitor_list, mon);
  return mon;
}

void sthread_user_monitor_free(sthread_mon_t mon)
{
  list_remove(monitor_list, mon);
  sthread_user_mutex_free(mon->mutex);
  delete_queue(mon->queue);
  free(mon);
}

void sthread_user_monitor_enter(sthread_mon_t mon)
{
  sthread_user_mutex_lock(mon->mutex);
}

void sthread_user_monitor_exit(sthread_mon_t mon)
{
  sthread_user_mutex_unlock(mon->mutex);
}

void sthread_user_monitor_wait(sthread_mon_t mon)
{
  struct _sthread *temp;

  if(mon->mutex->thr != active_thr){
    printf("monitor wait called outside monitor\n");
    return;
  }

  /* inserts thread in queue of blocked threads */
  temp = active_thr;
  temp->clock = Clock;
  queue_insert(mon->queue, temp);

  /* exits mutual exclusion region */
  sthread_user_mutex_unlock(mon->mutex);

  splx(HIGH);
  struct _sthread *old_thr;
  old_thr = active_thr;
  active_thr = RBDelete(exe_thr_list);
  active_thr->clock = Clock;
  sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);
  splx(LOW);
}

void sthread_user_monitor_signal(sthread_mon_t mon)
{
  struct _sthread *temp;

  if(mon->mutex->thr != active_thr){
    printf("monitor signal called outside monitor\n");
    return;
  }

  while(atomic_test_and_set(&(mon->mutex->l))) {}
  if(!queue_is_empty(mon->queue)){
    temp = queue_remove(mon->queue);

    if(temp->vruntime < active_thr->vruntime){
	mon->mutex->thr = temp;
	active_thr->clock = Clock;
	queue_insert(mon->mutex->queue,active_thr);
	atomic_clear(&(mon->mutex->l));

	splx(HIGH);
	struct _sthread *old_thr;

	old_thr = active_thr;
	active_thr = temp;
	checkNoThreads();
	RBTreeInsert(exe_thr_list, &(old_thr->vruntime), old_thr);
	sthread_switch(old_thr->saved_ctx, active_thr->saved_ctx);

	splx(LOW);

    }
    else{    queue_insert(mon->mutex->queue, temp);
	     atomic_clear(&(mon->mutex->l));
    }

  }
  else atomic_clear(&(mon->mutex->l));
}

void sthread_user_monitor_signalall(sthread_mon_t mon)
{
  struct _sthread *temp;

  if(mon->mutex->thr != active_thr){
    printf("monitor signalall called outside monitor\n");
    return;
  }

  while(atomic_test_and_set(&(mon->mutex->l))) {}
  while(!queue_is_empty(mon->queue)){
    /* changes blocking queue for thread */
    temp = queue_remove(mon->queue);
    temp->clock = Clock;
    queue_insert(mon->mutex->queue, temp);
  }
  atomic_clear(&(mon->mutex->l));
}


/* The following functions are dummies to 
 * highlight the fact that pthreads do not
 * include monitors.
 */

sthread_mon_t sthread_dummy_monitor_init()
{
   printf("WARNING: pthreads do not include monitors!\n");
   return NULL;
}


void sthread_dummy_monitor_free(sthread_mon_t mon)
{
   printf("WARNING: pthreads do not include monitors!\n");
}


void sthread_dummy_monitor_enter(sthread_mon_t mon)
{
   printf("WARNING: pthreads do not include monitors!\n");
}


void sthread_dummy_monitor_exit(sthread_mon_t mon)
{
   printf("WARNING: pthreads do not include monitors!\n");
}


void sthread_dummy_monitor_wait(sthread_mon_t mon)
{
   printf("WARNING: pthreads do not include monitors!\n");
}


void sthread_dummy_monitor_signal(sthread_mon_t mon)
{
   printf("WARNING: pthreads do not include monitors!\n");
}

void sthread_dummy_monitor_signalall(sthread_mon_t mon)
{
   printf("WARNING: pthreads do not include monitors!\n");
}

void IntDest(void* a) { ; }
int IntComp(const void* a, const void* b) {
	if( *(int*)a > *(int*)b) return(1);
	if( *(int*)a < *(int*)b) return(-1);
	return(0);
}
void IntPrint(const void* a) {
	printf("%i", *(int*)a);
}
void InfoPrint(void* a) {
  struct _sthread * t = (struct _sthread *) a;
  printf("id: %d priority: %d vruntime: %d\n", t->tid, t->priority, t->vruntime);
  printf("runtime: %ld sleeptime: %ld waittime: %ld\n", t->runtime, t->sleeptime, t->waittime);
}
void InfoDest(void* a) { ; }

int sthread_user_nice(int nice) {
  if((nice >= 0) && (nice <= 10))
    active_thr->nice = nice;
  return active_thr->nice + active_thr->priority;
}

void sthread_dump() {
  printf("==== dump start ====\n");
  printf("active_thread\n");
  InfoPrint(active_thr);
  printf("\n>>>>RB-Tree<<<<\n");
  RBTreePrint(exe_thr_list);
  printf(">>>>JoinList<<<< \n");
  JoinList();
  printf(">>>>SleepList<<<<\n");
  SleepList();
  printf(">>>>BlockedList<<<<\n");
  BlockList();
  printf("\n==== dump end ====\n");
}


//ir a lista dos mutexes e imprimir as threads as quais os gajos fizeram lock.
void JoinList() {
	rb_red_blk_tree * join_thr_tree = RBTreeCreate(IntComp, IntDest, InfoDest, IntPrint, InfoPrint);
	queue_element_t * aux = join_thr_list->first;
	
	while(aux != NULL){
		RBTreeInsert(join_thr_tree, &(aux->thread->clock), aux->thread);
		aux = aux->next;
	}

	RBTreePrint(join_thr_tree);
}


void SleepList() {
	rb_red_blk_tree * sleep_thr_tree = RBTreeCreate(IntComp, IntDest, InfoDest, IntPrint, InfoPrint);         /* árvore de threads adormecidas */
	queue_element_t * aux = sleep_thr_list->first;

	while(aux != NULL){
		RBTreeInsert(sleep_thr_tree, &(aux->thread->wake_time), aux->thread);	
		aux = aux->next;	
	}

	RBTreePrint(sleep_thr_tree);
}


void BlockList(){
   list_node* list_node_aux;
   sthread_mutex_t mutex_aux;
	sthread_mon_t mon_aux;

	//printf(">>>> Mutexes <<<<\n");
	for(list_node_aux = mutex_list->first; list_node_aux != NULL; list_node_aux = list_node_aux->next){
		mutex_aux = (sthread_mutex_t)list_node_aux->key;
		queue_t *aux = create_queue();
		while (!queue_is_empty(mutex_aux->queue)) {
			struct _sthread *thread = queue_remove(mutex_aux->queue);
			InfoPrint(thread);
			queue_insert(aux, thread);
		}
		delete_queue(mutex_aux->queue);
		mutex_aux->queue = aux;
	}
	//printf("------------------------------------------------\n");
	//printf(">>>> Monitores <<<<\n");
   for(list_node_aux = monitor_list->first; list_node_aux != NULL; list_node_aux = list_node_aux->next) {
		mon_aux = (sthread_mon_t)list_node_aux->key;
		queue_t *aux = create_queue();
		while (!queue_is_empty(mon_aux->queue)) {
			struct _sthread *thread = queue_remove(mon_aux->queue);
			InfoPrint(thread);
			queue_insert(aux, thread);
		}
		delete_queue(mon_aux->queue);
		mon_aux->queue = aux;
		// Ver lista do mutex do monitor
		aux = create_queue();
		while(!queue_is_empty(mon_aux->mutex->queue)) {
			struct _sthread *thread = queue_remove(mon_aux->mutex->queue);
			InfoPrint(thread);
			queue_insert(aux, thread);
		}
		delete_queue(mon_aux->mutex->queue);
		mon_aux->mutex->queue = aux;
	}
}


	
void InorderTreeIncrementAux(rb_red_blk_tree* tree, rb_red_blk_node* x) {
  rb_red_blk_node* nil=tree->nil;
  struct _sthread * t = (struct _sthread *) x->info;
  if (x != nil) {
    InorderTreeIncrementAux(tree,x->left);
    t->waittime++;
    InorderTreeIncrementAux(tree,x->right);
  }
}

void InorderTreeIncrement(rb_red_blk_tree* tree) {
  InorderTreeIncrementAux(tree, tree->root->left);
}

void* inactivity_dummy_thread(void* in) {
	while(1);
}

void checkNoThreads() {
   if(active_thr == NULL || active_thr->tid < 1) {
      sthread_user_create(inactivity_dummy_thread, NULL, 8);
      active_thr = RBDelete(exe_thr_list);
      active_thr->tid = 0;
      tid_gen--;
   }
}

int newVruntime(list * l) {
	list_node * aux = l->first;
	int res;
	if(l->first == NULL) return 0;
	res = ((sthread_t)aux->key)->vruntime;
	aux = aux->next;
	while(aux != NULL) {
		if(((sthread_t)aux->key)->vruntime < res)
			res = ((sthread_t)aux->key)->vruntime;
		aux = aux->next;
	}
	return res;
}

void decrementVruntime(list * l, int value) {
	list_node * aux = l->first;
	if(l->first == NULL) return;
	while(aux != NULL) {
		((sthread_t)aux->key)->vruntime -= value;
		aux = aux->next;
	}
}
