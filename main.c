
#include <stdio.h>
#include "thread.h"

void thread_func1(void) {
    for (int i = 0; i < 3; ++i) {
        printf("Thread 1: iteration %d\n", i);
        thread_yield();
    }
    thread_exit();
}

void thread_func2(void) {
    for (int i = 0; i < 3; ++i) {
        printf("Thread 2: iteration %d\n", i);
        thread_yield();
    }
    thread_exit();
}

int main() {
    thread_init();  // Register main thread

    thread_create(thread_func1);
    thread_create(thread_func2);

    // Main thread will also yield until all threads exit
    while (1) {
        thread_yield();
    }
    printf("Exiting from thread library!\n");
    return 0;
}
