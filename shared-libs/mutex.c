#include <assert.h>

#include "mutex.h"

void spin_lock(struct mutex *lock) {
    assert(lock);
    while (__sync_lock_test_and_set(&lock->locked, LOCKED) != UNLOCKED)
        asm volatile ("pause");
}

void spin_unlock(struct mutex *lock) {
    assert(lock);
    lock->locked = UNLOCKED;
}
