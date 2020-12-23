

struct N_THREADPOOL;
typedef void (*JOB_CALLBACK)(void *);

typedef struct N_WORKER{
    int thread;
    int terminate;
    struct N_THREADPOOL* pool;
    struct N_WORKER* next;
    struct N_WORKER* prev;

}n_worker;

typedef struct N_JOB {
 JOB_CALLBACK job_func;
 void* arg;
 struct N_JOB* next;
 struct N_JOB* prev;
} n_job;

typedef struct N_THREADPOOL {
    struct N_WORKER* workers;
    struct N_JOB* jobs;
    int mutex;
    int cond;
    int n_threads;
    int count;
}n_thread_pool;

void worker_thread(void* arg);
int threadpool_create(n_thread_pool* pool, int num);
void threadpool_shutdown(n_thread_pool* pool, int num);
void threadpool_push(n_thread_pool* pool, n_job* job);