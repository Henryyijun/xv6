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


struct Arg {
    n_worker* worker;
    int num;
};

void worker_thread(void* arg) {
    struct Arg a = *(struct Arg*)arg;
    n_worker* worker = a.worker;
    //int num = a.num;


    while (1) {
        sem_p(worker->pool->mutex);
        //printf(1, "the worker mutex is %d\n", worker->pool->mutex);
        while (worker->pool->jobs == NULL) {
            if (worker->terminate == 1) {
                break;
            }
            //printf(1, "waiting!\n");
            
            sem_v(worker->pool->mutex); /// ???
            sem_p(worker->pool->cond);
        }

        // if (worker->terminate == 1) {
        //     sem_v(worker->pool->mutex);
        //     break;
        // }
        
        n_job* job = worker->pool->jobs;
        if (job != NULL) {
            
            sem_p(worker->pool->mutex);
            LL_REMOVE(job, worker->pool->jobs);
            worker->pool->count++;
            sem_v(worker->pool->mutex);
        }     
        sem_v(worker->pool->mutex);
         
        job->job_func(job);
        if (worker->pool->count == worker->pool->n_threads) {
            exit();
            break;
        }
    }
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
    //printf(1,"create mutex successfully,the mutex id is %d!\n", pool->mutex);
    pool->cond = sem_create(1);
    //printf(1,"create cond successfully!\n");
    for (int i = 0; i < num; i++) {
        n_worker* worker = (n_worker*)malloc(sizeof (n_worker));
        worker->terminate = 0;
        memset(worker, 0, sizeof(n_worker));
        if (worker == NULL) {
            printf(1,"allocate error!\n");
            return -1;
        }
        worker->pool = pool;
        struct Arg arg ;
        arg.worker = worker;
        arg.num = num;
        thread_create(worker_thread, (void*)&arg);
        //printf(1,"create thread %d successfully!\n", pid);
        LL_ADD(worker, worker->pool->workers);
    }
    return 1;
}

void threadpool_shutdown(n_thread_pool* pool, int num) {
    //for (n_worker* w = pool->workers; w != NULL; w = w->next) {
    //    w->terminate = 1;
    //}
    for (int i = 0; i < num; i++) {
        thread_join();
    }
    //pass
}
void threadpool_push(n_thread_pool* pool, n_job* job) {
    sem_p(pool->mutex);
    LL_ADD(job, pool->jobs);
    //printf(1, "add job successfully!\n");
    sem_v(pool->cond);
    sem_v(pool->mutex);
    //printf(1, "exit push func\n");
}