#include "util/thread.h"

#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>


static void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*) arg;
    while (1) {
        pthread_mutex_lock(&pool->lock);
        while (pool->jobQueueHead == NULL && !pool->stop)
            pthread_cond_wait(&pool->cond, &pool->lock);
        
        if (pool->stop) {
            pthread_mutex_unlock(&pool->lock);
            break;
        }

        Job* job = pool->jobQueueHead;
        pool->jobQueueHead = job->next;
        if (pool->jobQueueHead == NULL) pool->jobQueueTail = NULL;

        pthread_mutex_unlock(&pool->lock);
        job->func(job->arg);
        free(job);
        atomic_fetch_sub(&pool->jobsPending, 1);
        pthread_mutex_lock(&pool->lock);
        if (atomic_load(&pool->jobsPending) == 0)
            pthread_cond_signal(&pool->doneCond);
        pthread_mutex_unlock(&pool->lock);
    }
    return NULL;
}


void threadpoolAddJob(ThreadPool* pool, JobFunc func, void* arg) {
    Job* job = malloc(sizeof(Job));
    job->func = func;
    job->arg = arg;
    job->next = NULL;

    pthread_mutex_lock(&pool->lock);
    if (pool->jobQueueTail == NULL) {
        pool->jobQueueHead = pool->jobQueueTail = job;
    } else {
        pool->jobQueueTail->next = job;
        pool->jobQueueTail = job;
    }
    atomic_fetch_add(&pool->jobsPending, 1);
    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
}


void threadpoolWait(ThreadPool* pool) {
    pthread_mutex_lock(&pool->lock);
    while (atomic_load(&pool->jobsPending) > 0) {
        pthread_cond_wait(&pool->doneCond, &pool->lock);
    }
    pthread_mutex_unlock(&pool->lock);
}


ThreadPool* threadpoolCreate(int threadCount) {
    ThreadPool* pool = malloc(sizeof(ThreadPool));
    pool->threadCount = threadCount;
    pool->threads = malloc(sizeof(pthread_t) * threadCount);
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);
    pthread_cond_init(&pool->doneCond, NULL);
    pool->jobQueueHead = NULL;
    pool->jobQueueTail = NULL;
    atomic_init(&pool->jobsPending, 0);
    pool->stop = 0;

    for (int i = 0; i < threadCount; i++) {
        pthread_create(&pool->threads[i], NULL, worker, pool);
    }
    return pool;
}


void threadpoolDestroy(ThreadPool* pool) {
    pthread_mutex_lock(&pool->lock);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->lock);
    for (int i = 0; i < pool->threadCount; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->cond);
    free(pool->threads);
    free(pool);
}
