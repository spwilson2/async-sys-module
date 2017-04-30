#ifndef __MUTEX_H__
#define __MUTEX_H__
#include "asm-primitives.h"
#include <assert.h>

#define LOCKED 1
#define UNLOCKED 0

struct mutex {
    volatile unsigned int locked;
};

static inline void mutex_init(struct mutex *lock) {
    assert(lock);
    lock->locked = UNLOCKED;
}

static inline void mutex_lock(struct mutex *lock) {
    assert(lock);
    while (__sync_lock_test_and_set(&lock->locked, LOCKED) != UNLOCKED)
        __asm_pause();
}

static inline void mutex_unlock(struct mutex *lock) {
    assert(lock);
    lock->locked = UNLOCKED;
}

#endif
