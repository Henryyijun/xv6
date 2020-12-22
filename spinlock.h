// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uint pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};

#define MAX_SEM_NUM 128




struct sem {
  struct spinlock lock;
  int used;
  int resources;
  int allocated;
};
struct  share_mem{
    char buff[16][256];
    int line_read;
    int line_write;
};
extern struct sem sems[MAX_SEM_NUM];

