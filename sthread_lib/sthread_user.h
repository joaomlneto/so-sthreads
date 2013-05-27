/*
 * sthread_user.h - This file defines the user-level thread
 *                  implementation of sthreads. The routines
 *                  are all described in the sthread.h file.
 *
 */

#ifndef STHREAD_USER_H
#define STHREAD_USER_H 1

#include "red_black_tree.h"
#include "list.h"

#define MIN_DELAY 5

#define VRUNTIME_MAX 1000000

/* Basic Threads */
void sthread_user_init(void);
sthread_t sthread_user_create(sthread_start_func_t start_routine, void *arg, int priority);
void sthread_user_exit(void *ret);
void sthread_user_yield(void);

/* Advanced Threads */
int sthread_user_sleep(int time);
int sthread_user_join(sthread_t thread, void **value_ptr);


/* Synchronization Primitives */
sthread_mutex_t sthread_user_mutex_init(void);
void sthread_user_mutex_free(sthread_mutex_t lock);
void sthread_user_mutex_lock(sthread_mutex_t lock);
void sthread_user_mutex_unlock(sthread_mutex_t lock);

sthread_mon_t sthread_user_monitor_init();
void sthread_user_monitor_free(sthread_mon_t mon);
void sthread_user_monitor_enter(sthread_mon_t mon);
void sthread_user_monitor_exit(sthread_mon_t mon);
void sthread_user_monitor_wait(sthread_mon_t mon);
void sthread_user_monitor_signal(sthread_mon_t mon);
void sthread_user_monitor_signalall(sthread_mon_t mon);

/* RBT Thread Functions */
void IntDest(void* a);
int IntComp(const void* a, const void* b);
void IntPrint(const void* a);
void InfoPrint(void* a);
void InfoDest(void* a);

int sthread_user_nice(int nice);
void sthread_dump();
void SleepList();
void JoinList();
void BlockList();

void InorderTreeIncrementAux(rb_red_blk_tree* tree, rb_red_blk_node* x);
void InorderTreeIncrement(rb_red_blk_tree* tree);
void* inactivity_dummy_thread(void* in);
void checkNoThreads();
int newVruntime(list *);
void decrementVruntime(list *, int);

#endif /* STHREAD_USER_H */
