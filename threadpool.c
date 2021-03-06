#include "types.h"
#include "user.h"
#include "threadpool.h"
#define NULL 0

#define LL_ADD(item, list) do { \
 item->prev = NULL;   \
 item->next = list;   \
 list = item;    \
} while(0)

#define LL_REMOVE(item, list) do {        \
 if (item->prev != NULL) item->prev->next = item->next; \
 if (item->next != NULL) item->next->prev = item->prev; \
 if (list == item) list = item->next;     \
 item->prev = item->next = NULL;       \
} while(0)


void worker_thread(void* arg) {
    n_worker* worker = (void*)arg;
    while (1) {
        sem_p(worker->pool->mutex);
        while (worker->pool->jobs == NULL) {
            if (worker->terminate) break;
            sem_cond_p(worker->pool->cond, worker->pool->mutex);
        }
        n_job* job = worker->pool->jobs;
        LL_REMOVE(job, worker->pool->jobs);
        worker->pool->count++;
        sem_v(worker->pool->mutex);
        if (job == NULL) {
            continue;
        }
        job->job_func(job);
        if (worker->pool->count == worker->pool->n_threads) {
            //printf(1,"heeee,the count is %d\n", worker->pool->count);
            break;
        }
    }
    free(worker);
    exit();
}

int threadpool_create(n_thread_pool* pool, int num) {
    if (pool == NULL) {
        return 1;
    }
    if (num < 1) {
        num = 1;
    }
    memset(pool, 0, sizeof (n_thread_pool));

    pool->mutex = sem_create(1);
    pool->cond = sem_create(1);
    pool->count_mutex = sem_create(1);
    for (int i = 0; i < num; i++) {
        n_worker* worker = (n_worker*)malloc(sizeof (n_worker));
        worker->terminate = 0;
        memset(worker, 0, sizeof(n_worker));
        if (worker == NULL) {
            printf(1,"allocate error!\n");
            return -1;
        }
        worker->pool = pool;
        thread_create(worker_thread, (void*)worker);
        LL_ADD(worker, worker->pool->workers);
    }
    return 1;
}

void threadpool_shutdown(n_thread_pool* pool, int num) {

    for (n_worker* worker = pool->workers;worker != NULL;worker = worker->next) {
        worker->terminate = 1;
    }
    
    sem_p(pool->mutex);
    
    pool->workers = NULL;
    pool->jobs = NULL;
    sem_cond_broadcast(pool->cond);
    
    sem_v(pool->mutex);
    for (int i = 0; i < num; i++) {
        thread_join();
    }
    sem_free(pool->mutex);
    sem_free(pool->cond);
    printf(1, "shutdown succussfully!\n");

}
void threadpool_push(n_thread_pool* pool, n_job* job) {
    sem_p(pool->mutex);
    LL_ADD(job, pool->jobs);
    sem_cond_v(pool->cond);
    sem_v(pool->mutex);
}