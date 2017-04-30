#include "../mutex.h"
#include <stdio.h>

struct mutex lock;

int main() {
    mutex_lock(&lock);
    mutex_unlock(&lock);
    mutex_lock(&lock);
    mutex_unlock(&lock);
    return 0;
}
