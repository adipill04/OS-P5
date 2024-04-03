#include "spinlock.h"
#include "types.h"

typedef struct
{
    struct spinlock lk;
    // Lock state, ownership, etc.
    uint locked; // 0 - free/1 - held
    char *name;  // Name of lock. (debugging, no use prob)
    int pid;     // Process holding lock
} mutex;