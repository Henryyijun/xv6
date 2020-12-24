#include "types.h"
#include "user.h"
#include "threadpool.h"

#define NUM1 8
#define NUM2 64

int id;
void king_counter(void *arg) {
    sem_p(id);
    n_job *job = (n_job*)arg;
    int index = *(int *)job->arg;
    printf(1, "the thread index: %d\n", index);
    write_share(read_share()+1);
    sem_v(id);
    free(job->arg);
    free(job);

}
int main() {
    id  = sem_create(1);
    n_thread_pool pool;
    int ret = threadpool_create(&pool, NUM1);
    if (ret == 1) {
        printf(1,"create thread pool successfully!\n");
    }
    pool.n_threads = NUM2;
    pool.count = 0;
    for (int i = 0; i < NUM2; i++) {
        n_job* job = (n_job*)malloc(sizeof(n_job));
        job->job_func = king_counter;
        job->arg = malloc(sizeof(int));
        *(int*)job->arg = i;

        threadpool_push(&pool, job);
      
    }

    for (int i = 0; i < NUM2; i++) {
        thread_join();
    }
    // while (1) {
    //     if (pool.count == NUM2){
    //         threadpool_shutdown(&pool, NUM2);
    //         break;
    //     }  
    // }
    
    printf(1, "the gloabl is %d\n", read_share());
    sem_free(id);
    
    exit();
    return 0;
    
}