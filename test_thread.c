#include "types.h"
#include "user.h"
#include "fcntl.h" 

volatile int global = 1;

int F(int n) {
    if (n < 0) {
        printf(1, "please input a positive integer\n");

    } else if (n == 1 || n == 2) {
        return 1;
    } else {
        return F(n - 1) + F(n - 2);
    }
    return 0;
}

void worker(void* arg) {
    int id = *(int*)arg;
    sem_p(id);
    printf(1, "thread %d is worker. \n", *(int*)arg);
    //global = F(*(int*)arg);
    //write(3, "hello\n", 6);
    sem_v(id);
    exit();
}

int main() {

    int id = sem_create(1);
    for (int i = 0; i < 5; i++) {
        thread_create(worker, &id);
    }

    for (int i = 0; i < 5; i++) {
        thread_join();
    }
    
    /*int pid = thread_create(worker, &t);
    thread_join();
    printf(1, "thread id = %d\n", pid);
    printf(1, "global = %d\n", global);
    t = 6;
    int pid1 = thread_create(worker, &t);
    thread_join();

    printf(1, "thread id = %d\n", pid1);
    printf(1, "global = %d\n", global);*/
    exit();
    return 0;
}