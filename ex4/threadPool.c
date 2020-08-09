
#include "threadPool.h"

void *threadpoolRun(void *attr) {

    ThreadPool *threadPool = (ThreadPool *) attr;

    while (true) {
        //lock mutex
        if (pthread_mutex_lock(threadPool->mutex) != 0) {
            fprintf(stderr, "Error occurred - cannot lock.");
            return NULL;
        }

        //wait while the queue is empty
        while (osIsQueueEmpty(threadPool->queue) && !threadPool->destroy) {
            if (pthread_cond_wait(threadPool->cond, threadPool->mutex) != 0) {
                fprintf(stderr, "Error occurred - cannot change condition value to wait.");
                return NULL;
            }
        }

        //there are tasks to perform - unlock mutex and perform task, after checking if the threadpool is being closed
        if (threadPool->destroy) {
            if (threadPool->shouldWaitForTasks && osIsQueueEmpty(threadPool->queue)) {
                //no need to wait
                break;
            } else if (!threadPool->shouldWaitForTasks) {
                break;
            }
        }

        //reached here only if threadPool should be destroyed but waiting for tasks, or if threadpool is running.

        Task *task = osDequeue(threadPool->queue);

        //unlock
        if (pthread_mutex_unlock(threadPool->mutex) != 0) {
            fprintf(stderr, "Error occurred - cannot unlock.");
            return NULL;
        }

        if (task == NULL) {
            fprintf(stderr, "Error occurred - dequeue.");
            return NULL;
        }

        (*(task->computeFunc))(task->param);
        free(task);
    }

    //unlock and exit thread
    if (pthread_mutex_unlock(threadPool->mutex) != 0) {
        fprintf(stderr, "Error occurred - cannot unlock.");
        return NULL;
    }
    return NULL;
}

ThreadPool *initializeStruct(int numOfThreads) {
    //initialize struct
    ThreadPool *threadPool = malloc(sizeof(ThreadPool));
    threadPool->queue = osCreateQueue();
    threadPool->numOfThreads = numOfThreads;
    threadPool->threads = malloc(sizeof(pthread_t) * threadPool->numOfThreads);

    if (threadPool == NULL || threadPool->queue == NULL || threadPool->threads == NULL) {
        fprintf(stderr, "Error occurred in creation or memory allocation.\n");
        return NULL;
    } else return threadPool;
}

ThreadPool *tpCreate(int numOfThreads) {

    ThreadPool *threadPool = initializeStruct(numOfThreads);

    //check if initialization succeeded
    if (threadPool == NULL || threadPool->queue == NULL || threadPool->threads == NULL) {
        fprintf(stderr, "Error occurred in creation or memory allocation.\n");
        return NULL;
    }

    threadPool->destroy = false;
    threadPool->shouldWaitForTasks = false;
    threadPool->mutex = malloc(sizeof(pthread_mutex_t));
    threadPool->cond = malloc(sizeof(pthread_cond_t));


    if ((pthread_mutex_init(threadPool->mutex, NULL) != 0)
        || (pthread_cond_init(threadPool->cond, NULL) != 0)) {
        fprintf(stderr, "Error occurred in initialization of mutex or condition value.\n");
        return NULL;
    }

    //create all threads
    pthread_t *threads = threadPool->threads;
    int i;
    for (i = 0; i < numOfThreads; ++i) {
        pthread_t *current = threads + i;
        pthread_create(current, NULL, &threadpoolRun, threadPool);
    }
    return threadPool;
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {

    //cannot call tpDestroy by two threads
    if (threadPool->destroy) {
        return;
    }

    //lock mutex
    if (pthread_mutex_lock(threadPool->mutex) != 0) {
        fprintf(stderr, "Error occurred - cannot lock.");
        return;
    }

    //lock mutex and set booleans
    threadPool->destroy = true;
    threadPool->shouldWaitForTasks = shouldWaitForTasks;

    //broadcast to all threads
    if (pthread_cond_broadcast(threadPool->cond) != 0) {
        fprintf(stderr, "Error occurred - cannot change condition value to broadcast.");
        return;
    }

    //unlock
    if (pthread_mutex_unlock(threadPool->mutex) != 0) {
        fprintf(stderr, "Error occurred - cannot unlock.");
        return;
    }

    //join all threads so that they'll finish their action
    int i;
    pthread_t *threads = threadPool->threads;
    for (i = 0; i < threadPool->numOfThreads; ++i) {
        pthread_t *current = threads + i;
        if (pthread_join(*current, NULL) != 0) {
            fprintf(stderr, "Error in joining threads - cannot terminate");
            return;
        }
    }

    //free allocation
    osDestroyQueue(threadPool->queue);
    free(threadPool->threads);
    free(threadPool->cond);
    free(threadPool->mutex);
    free(threadPool);
}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {

    if (threadPool->destroy) {
        fprintf(stderr, "Cannot insert task after threadpool is destroyed!");
        return -1;
    }

    //lock mutex
    if (pthread_mutex_lock(threadPool->mutex) != 0) {
        fprintf(stderr, "Error occurred - cannot lock.");
        return -1;
    }

    //add task
    Task *task = malloc(sizeof(Task));
    task->param = param;
    task->computeFunc = computeFunc;

    osEnqueue(threadPool->queue, task);

    //signal a thread - a task is added so at least one thread should stop waiting
    if (pthread_cond_signal(threadPool->cond) != 0) {
        fprintf(stderr, "Error - cannot change condition value to signal.");
        return -1;
    }

    //unlock
    if (pthread_mutex_unlock(threadPool->mutex) != 0) {
        fprintf(stderr, "Error occurred - cannot unlock.");
        return -1;
    }

    return 0;
}
