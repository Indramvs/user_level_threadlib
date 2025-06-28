#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "stdbool.h"
#include "mymalloc.h"

#define STACK_SIZE (32 * 1024) // 32 KB

// Thread states
typedef enum thread_state {
    START = 0,
    RUNNABLE,
    RUNNING,
    BLOCKED,
    END
} thread_state;

// Thread Control Block (TCB)
typedef struct TCB {
    int thread_id;
    thread_state state;
    void *stack;
    ucontext_t context;
    struct TCB *next;
} TCB;

// Global variables
bool thread_lib_init = false;
TCB* current_thread = NULL;
TCB* ready_queue = NULL;
int thread_id_counter = 0;

// Initialize the threading system and register the main thread
void thread_init() {
    if(thread_lib_init)return;
    thread_lib_init = true;
    TCB* tcb1 = malloc(sizeof(TCB));
    if (!tcb1) {
        perror("Failed to allocate TCB for main thread");
        exit(1);
    }

    getcontext(&(tcb1->context));

    tcb1->stack = NULL;
    tcb1->thread_id = thread_id_counter++;
    tcb1->state = RUNNING;
    tcb1->next = NULL;

    current_thread = tcb1;
    ready_queue = NULL;
}

// Create a new thread
int thread_create(void (*func)()) {
    if(!thread_lib_init)return -1;
    TCB* newtcb = mymalloc(sizeof(TCB));
    if (!newtcb) return -1;

    newtcb->stack = mymalloc(STACK_SIZE);
    if (!newtcb->stack) {
        free(newtcb);
        return -1;
    }

    getcontext(&(newtcb->context));
    newtcb->context.uc_stack.ss_sp = newtcb->stack;
    newtcb->context.uc_stack.ss_size = STACK_SIZE;
    newtcb->context.uc_link = NULL;

    newtcb->thread_id = thread_id_counter++;
    newtcb->state = RUNNABLE;
    newtcb->next = NULL;

    makecontext(&(newtcb->context), func, 0);

    if (ready_queue == NULL) {
        ready_queue = newtcb;
    } else {
        TCB* temp = ready_queue;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newtcb;
    }

    return 0;
}

void thread_yield() {
    if(!thread_lib_init)return ;
    TCB* prev_thread = current_thread;

    if (ready_queue == NULL) {
        thread_exit(); 
    }
    prev_thread->state = RUNNABLE;

    TCB* temp = ready_queue;
    while (temp->next != NULL) 
        temp = temp->next;
    temp->next = prev_thread;
    prev_thread->next = NULL;
    TCB* next_thread = ready_queue;
    ready_queue = ready_queue->next;
    next_thread->next = NULL;
    next_thread->state = RUNNING;   
    current_thread = next_thread;

    swapcontext(&(prev_thread->context), &(next_thread->context));
}


void thread_exit() {
    if(!thread_lib_init)return;
    TCB* freethread = current_thread;
    current_thread->state = END;

    if (ready_queue == NULL) {
        printf("exiting from thread library!\n");
        free(freethread);
        thread_lib_init = false;
        exit(0); 
    }

    TCB* next_thread = ready_queue;
    ready_queue = ready_queue->next;
    next_thread->next = NULL;
    next_thread->state = RUNNING;
    current_thread = next_thread;

    free(freethread);  

    // Transfer control to next thread
    setcontext(&(current_thread->context));
}