#ifndef __MUTEX_H__
#define __MUTEX_H__

#define LOCKED 1
#define UNLOCKED 0

struct mutex {
    volatile unsigned int locked;
};

/* Lock the spin lock. */
void spin_lock(struct mutex *lock);

/* Unlock the spin lock. */
void spin_unlock(struct mutex *lock);

#endif
