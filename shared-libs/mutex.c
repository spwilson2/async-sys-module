#include <assert.h>

#include "mutex.h"
#include "asm-primitives.h"

void spin_lock(struct mutex *lock) {
    assert(lock);
    while (__sync_lock_test_and_set(&lock->locked, LOCKED) != UNLOCKED)
        __asm_pause();

}

void spin_unlock(struct mutex *lock) {
    assert(lock);
    lock->locked = UNLOCKED;
}
