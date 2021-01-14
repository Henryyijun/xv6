#include "types.h"
#include "user.h"
#include "fcntl.h" 

volatile int global = 0;

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
    global += F(5);
    exit();
}

int main() {
    int t = 1;
    open("tmp", O_RDWR | O_CREATE);
    for (int i = 0; i < 5; i++) {
        thread_create(worker, &t);
    }
    
    thread_join();
      
    //printf(1, "thread id = %d . \n", pid);
    printf(1, "global = %d  \n", global);
    exit();
    return 0;
}