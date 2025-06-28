#ifndef THREAD_H
#define THREAD_H

int thread_create(void (*func)(void));
void thread_yield();
void thread_exit();
void thread_start();
void thread_init();

#endif
