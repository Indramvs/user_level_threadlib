# User-Level Thread Library

This project implements a simple **user-level thread library** using `ucontext.h` and custom 'malloc' in C. The scheduler follows a **First-Come, First-Served (FCFS)** policy. Threads can be created, yield control to others, and exit.

# How to Run
To compile and run the program, use the following commands
```bash
make
````

---

# Thread\_library

**Global variables/structures/enums:**

**enum thread\_state**
*Description:*
Contains all the possible thread states.

**struct TCB**
*Description:*
Thread control block structure.
Structure fields contain:

1. `thread_id`: the ID of the thread
2. `state`: state of the thread
3. `stack`: points to the stack allocated for the thread. later used for the cleanup
4. `context`: context of the particular thread
5. `next`: pointer to the next TCB node

**bool thread\_lib\_init**
*Description:*
Checks the status of thread initialization.

**int thread\_id\_counter**
*Description:*
Assigns a unique thread ID for each thread.

**TCB* ready\_queue*\*
*Description:*
Contains a list of runnable threads.

**TCB* current\_thread*\*
*Description:*
Points to the currently running thread.

---

**Thread functions:**

**thread\_init()**
*Description:*
Initializes the thread library. Called only once before other thread functions.

1. Checks the `thread_lib_init` flag and returns if set, indicating the function was called previously.
2. Sets the `thread_lib_init` flag to true.
3. Allocates space for the TCB.
4. Gets the context of the TCB and sets the TCB fields:
   a. `stack` points to `NULL` because the stack is allocated for the main function by the OS itself, so there is no need to assign one.
   b. Assigns thread ID and increments `thread_id_counter`.
   c. Sets the state to `RUNNING`.
   d. Sets the main thread as the `current_thread` and `ready_queue` points to nothing since there are no runnable threads.

---

\**thread\_create(void (*func)())**
*Description:*
Creates a new thread and associates it with a function.

1. Checks whether `thread_init` was called.
2. Allocates space for a TCB.
3. Allocates stack for the newly created thread.
4. Gets the context of the thread and sets the TCB fields:
   a. Assigns the stack address to the `uc_stack` stack pointer.
   b. Assigns thread ID, state to `RUNNABLE`.
5. Calls `makecontext`, and if the ready queue is `NULL`, sets `ready_queue` to the new TCB; otherwise, places the new TCB at the tail of the ready queue.

---

**thread\_yield()**
*Description:*
Yields the current running thread and makes the next thread in the ready queue running.

1. Checks whether `thread_init` was called.
2. Invokes `thread_exit` if there are no runnable threads.
3. Saves the running thread and first runnable thread context; puts the running thread at the end of the ready queue and makes the first runnable thread the current thread.
4. Swaps the context of the running thread and first runnable thread present in the ready queue using saved contexts.

---

**thread\_exit()**
*Description:*
Clears the memory allocated for the running thread and updates the current thread and ready queue. Upon all threads completing, exits the program.

1. Checks whether `thread_init` was called.
2. Makes the current thread state `END`.
3. Frees memory and exits the program if the ready queue is empty.
4. Otherwise, makes the first runnable thread the current thread and sets its context.

---

**Scheduler:**
The scheduling policy is **First-Come, First-Served (FCFS)**. Threads are executed in the order they enter the ready queue.

---

Let me know if you want it in markdown formatting or any additional formatting!


## 🧪 Example Usage

```c
void thread_func1() {
    for (int i = 0; i < 3; ++i) {
        printf("Thread 1: iteration %d\n", i);
        thread_yield();
    }
    thread_exit();
}

void thread_func2() {
    for (int i = 0; i < 3; ++i) {
        printf("Thread 2: iteration %d\n", i);
        thread_yield();
    }
    thread_exit();
}

int main() {
    thread_init();

    thread_create(thread_func1);
    thread_create(thread_func2);

    // Main thread yields until all threads exit
    while (1) {
        thread_yield();
    }

    return 0;
}
