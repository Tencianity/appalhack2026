#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <stdatomic.h>

typedef void (*JobFunc)(void* arg);

typedef struct Job {
    JobFunc func;
    void* arg;
    struct Job* next;
} Job;

typedef struct {
    pthread_t* threads;
    int threadCount;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    pthread_cond_t doneCond;
    Job* jobQueueHead;
    Job* jobQueueTail;
    atomic_int jobsPending;
    int stop;
} ThreadPool;

typedef struct {
    atomic_int nextTile;
    int totalTiles;
} TileQueue;


ThreadPool* threadpoolCreate(int threadCount);

void threadpoolDestroy(ThreadPool* pool);

void threadpoolAddJob(ThreadPool* pool, JobFunc func, void* arg);

void threadpoolWait(ThreadPool* pool);


#endif
