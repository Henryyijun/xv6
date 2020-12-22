#include "types.h"
#include "user.h"
#include "threadpool.h"

#define NUM1 4
#define NUM2 8
void king_counter(void *arg) {
    
    /*n_job *job = (n_job*)arg;
 
    int index = *(int *)job->arg;
    printf(1, "index: %d\n", index);
    free(job->arg);
    free(job);*/
}
int main() {
    n_thread_pool pool;
    int ret = threadpool_create(&pool, NUM1);
    if (ret == 1) {
        printf(1,"create thread pool successfully!\n");
    }
    
    for (int i = 0; i < NUM2; i++) {
        
        n_job* job = (n_job*)malloc(sizeof(n_job));
        job->job_func = king_counter;
        job->arg = malloc(sizeof(int));
        *(int*)job->arg = i;

        threadpool_push(&pool, job);
      
    }
    threadpool_shutdown(&pool, NUM2);
    printf(1, "lalalala\n");
    exit();
    return 0;
    
}