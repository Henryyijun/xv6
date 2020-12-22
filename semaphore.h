#include "types.h"
#include "user.h"

#include "fcntl.h" 

/*
typedef struct  {
   
    struct proc* head;
    proc_list* next; 
}proc_list;

typedef struct  {
   
    int value;
    struct proc_list* list;
}semaphore;

int sem_init(semaphore* sem) {
    sem->value = 1;
    sem->list = malloc(sizeof (struct proc_list *));
}

void sem_wait(semaphore* sem) {
    sem->value--;
    struct proc* curthread = myproc();
    if (sem->value < 0) {
        proc_list* list = sem->list;
        list->next = curthread;
    }

}

void sem_signal(semaphore* sem) {

}
*/

typedef struct  {
    int value;
}semaphore;

// void sem(semaphore* s) {
//     s = malloc (sizeof (semaphore*));
//     s->value = 1;
// }
void P(semaphore* s) {
    //printf(1, "pooo");
    while (s->value <= 0);
    
    s->value--;
    
}

void V(semaphore* s) {
    s->value++;
}


