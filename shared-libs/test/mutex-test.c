#include "../mutex.h"
#include <stdio.h>

struct mutex lock;

int main() {
    spin_lock(&lock);
    spin_unlock(&lock);
    spin_lock(&lock);
    spin_unlock(&lock);
    return 0;
}
