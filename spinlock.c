// Mutual exclusion spin locks.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  pushcli(); // disable interrupts to avoid deadlock.
  if(holding(lk))
    panic("acquire");

  // The xchg is atomic.
  while(xchg(&lk->locked, 1) != 0)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  __sync_synchronize();

  // Record info about lock acquisition for debugging.
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs);
}

// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk))
    panic("release");

  lk->pcs[0] = 0;
  lk->cpu = 0;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  popcli();
}

// Record the current call stack in pcs[] by following the %ebp chain.
void
getcallerpcs(void *v, uint pcs[])
{
  uint *ebp;
  int i;

  ebp = (uint*)v - 2;
  for(i = 0; i < 10; i++){
    if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
      break;
    pcs[i] = ebp[1];     // saved %eip
    ebp = (uint*)ebp[0]; // saved %ebp
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}

// Check whether this cpu is holding the lock.
int
holding(struct spinlock *lock)
{
  int r;
  pushcli();
  r = lock->locked && lock->cpu == mycpu();
  popcli();
  return r;
}


// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
  int eflags;

  eflags = readeflags();
  cli();
  if(mycpu()->ncli == 0)
    mycpu()->intena = eflags & FL_IF;
  mycpu()->ncli += 1;
}

void
popcli(void)
{
  if(readeflags()&FL_IF)
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}


/* sem*/

/* share memory*/
// struct  share_mem{
//     char buff[16][256];
//     int line_read;
//     int line_write;
// };
struct sem sems[MAX_SEM_NUM];
int share;


void init_sem(void) {
  for (int i = 0; i < MAX_SEM_NUM; i++) {
    initlock(&(sems[i].lock), "semaphore");
    sems[i].allocated = 0;
  }

  
}

int sys_sem_create(void) {
  int resources;
  if (argint(0, &resources) < 0) {
    return -1;
  } 

  for (int i = 0; i < MAX_SEM_NUM; i++) {
    acquire(&sems[i].lock);
    if (sems[i].allocated == 0) {
      
      sems[i].allocated = 1;
      sems[i].resources = resources;
      sems[i].used = 0;
      release(&sems[i].lock);
      return i;
    }
    release(&sems[i].lock);
  }
  return -1;
    
}

int sys_sem_free(void) {
  int i;
  if(argint(0, &i) < 0)
    return -1;
  
  if(i < 0 || i > MAX_SEM_NUM)  // 检查索引是否在范围内 
    return -1;

  acquire(&sems[i].lock);
  if(sems[i].allocated == 1 && sems[i].used == 0) {
    sems[i].allocated = 0;
    //cprintf("free %d sem\n", i);
  }
  release(&sems[i].lock);
  return 0;
}

int sys_sem_p(void) {
  int i ;
  if (argint(0, &i) < 0) {
    return -1;
  }
  acquire(&sems[i].lock);
  sems[i].resources--;
  if (sems[i].resources < 0) {
    sems[i].used++;
    sleep(&sems[i], &sems[i].lock);
  }
  release(&sems[i].lock);
  return 0;
}


int sys_sem_v() {
  int i;
  if (argint(0, &i) < 0) {
    return -1;
  }
  acquire(&sems[i].lock);
  sems[i].resources++;
  if (sems[i].resources < 1) {
    wakeup1p(&sems[i]);
    sems[i].used--;
  }
  release(&sems[i].lock);
  return 0;
}

int sys_sem_cond_p(void) {
  int cond, mutex; // cond, m
  if(argint(0, &cond) < 0) return -1;
  if(argint(1, &mutex) < 0) return -1;
  //unlock mutex
  acquire(&sems[mutex].lock);
  sems[mutex].resources++;
  if (sems[mutex].resources < 1) {
    wakeup1p(&sems[mutex]);
    sems[mutex].used--;
  }
  release(&sems[mutex].lock);

  //block
  acquire(&sems[cond].lock);
  sems[cond].resources = 0;
  if (sems[cond].resources <= 0) {
    sems[cond].used++;
    sleep(&sems[cond], &sems[cond].lock);
    while (1) {
    
      if (sems[cond].resources == 1) {
        
        goto done;
      }
    } 
  }
  //release(&sems[cond].lock);

  //lock
  done:
    release(&sems[cond].lock);
    
    acquire(&sems[mutex].lock);
    sems[mutex].resources--;
    if (sems[mutex].resources < 0) {
      sems[mutex].used++;
      sleep(&sems[mutex], &sems[mutex].lock);
    }
    release(&sems[mutex].lock);
  


  return 1;
}

int sys_sem_cond_broadcast(void) {
  int i;
  if (argint(0, &i) < 0) {
    return -1;
  }
  acquire(&sems[i].lock);
  sems[i].resources++;
  if (sems[i].resources < 1) {
    wakeup(&sems[i]);
    sems[i].used--;
  }
  release(&sems[i].lock);
  return 0;
}

int sys_sem_cond_v(void) {
  int i;
  if (argint(0, &i) < 0) {
    return -1;
  }
  acquire(&sems[i].lock);
  sems[i].resources = 1;
  if (sems[i].resources == 1) {
    wakeup(&sems[i]);
    sems[i].used--;
  }
  release(&sems[i].lock);
  return 0;
}