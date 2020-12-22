#include "types.h"
#include "user.h"
#include "fcntl.h" 
#include "semaphore.h"
#include "spinlock.h"
struct Arg {
    int arg1;
    int arg2;
    int arg3;
};

struct share_mem s;
void consumer(void* arg) {
    struct Arg a = *(struct Arg*) arg;
    int mutex = a.arg1;
    int empty = a.arg2;
    int full = a.arg3;
    for (int i = 0; i < 16; i++) {
        sem_p(full);
        sem_p(mutex);
        printf(1, "I am the consumer thread, -- ");
        printf(1, "%s\n" , s.buff[s.line_read]);
        memset(s.buff[s.line_read], 0, sizeof s.buff[s.line_read]);
        s.line_read = (s.line_read + 1) % 16;
        sem_v(mutex);
        sem_v(empty);
        
    }
    exit();
}

void producer(void* arg) {
    struct Arg a = *(struct Arg*) arg;
    int mutex = a.arg1;
    int empty = a.arg2;
    int full = a.arg3;
    for (int i = 0; i < 16; i++) {
        sem_p(empty);
        sem_p(mutex);
        printf(1, "I am the producer thread\n");
        strcpy(s.buff[s.line_write], "hello,I am producer, I am producing\n");
        s.line_write = (s.line_write + 1) % 16;
        sem_v(mutex);
        sem_v(full);
        sleep(1);
    }
    exit();
}


int main() {
    struct Arg  arg;
    int mutex = sem_create(1);
    int empty = sem_create(16);
    int full = sem_create(0);
    arg.arg1 = mutex;
    arg.arg2 = empty;
    arg.arg3 = full;
    s.line_read = 0;
    s.line_write = 0;
    thread_create(consumer, &arg);
    thread_create(producer, &arg);
    for (int i = 0; i < 2; i++) {
        thread_join();
    }
    sem_free(mutex);
    sem_free(empty);
    sem_free(full);
    
    //printf(1, "global = %p\n", read_share());
    
    
    exit();
    return 0;
}