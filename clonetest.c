#include "types.h"
#include "stat.h"
#include "user.h"
#include <stddef.h>

int global = 0xB38;  // Global variable

void thread_func(void *arg) {
    int *heap_var = (int *)malloc(sizeof(int));
    *heap_var = 0x1234;  // Assign a value in heap

    printf(1, "[Child] PID: %d, Global: %x, Heap: %x, Stack: %p\n", getpid(), global, *heap_var, &heap_var);
    exit();
}

int main() {
    // Proper stack allocation
    void *stack = sbrk(4096);
    if ((uint)stack % 4096) {   
        stack = (void*)((uint)stack + (4096 - ((uint)stack % 4096))); 
    }

    int *heap_var = (int *)malloc(sizeof(int));  // Allocate heap
    *heap_var = 0xA000;

    printf(1, "[Parent] PID: %d, Global: %x, Heap: %x, Stack: %p\n", getpid(), global, *heap_var, &heap_var);

    int pid = clone(thread_func, NULL, stack);
    if (pid < 0) {
        printf(1, "clone failed\n");
        exit();
    }

    wait();
    printf(1, "[Parent] After Child Execution - Heap: %x\n", *heap_var); // Check if child modified heap
    exit();
}
