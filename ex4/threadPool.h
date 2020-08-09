#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "osqueue.h"

typedef struct thread_pool
{
    OSQueue* queue;
    pthread_t* threads;
    int numOfThreads;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    bool destroy;
    bool shouldWaitForTasks;
}ThreadPool;

typedef struct task
{
    void (*computeFunc) (void *);
    void* param;
}Task;

void* threadpoolRun(void* attr);

// given function
ThreadPool* tpCreate(int numOfThreads);

void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif
