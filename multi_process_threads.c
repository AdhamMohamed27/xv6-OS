#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// how to run the code: cc -o multi_process_program multi_process_threads.c -lpthread
// ./multi_process_program <num_processes> <num_threads_per_process> <filepath>

typedef struct {
    int *nums; // pointer for the array of numbers
    int start; // starting index for the thread
    int end;   // ending index for the thread
    unsigned long (*func)(int, int); // function pointer
    unsigned long result; // result of the function
    int process_id; // to identify which process this thread belongs to
} ThreadData;

// Shared memory structure to communicate results between processes
typedef struct {
    unsigned long *results; // Array to store results from each process
    int num_processes;      // Number of processes
} SharedResults;

// Function to create shared memory for inter-process communication
SharedResults* create_shared_memory(int num_processes) { 
    SharedResults *shared = mmap(NULL, sizeof(SharedResults) + num_processes * sizeof(unsigned long),PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (shared == MAP_FAILED) {
        perror("Error creating shared memory");
        return NULL;
    }
    
    shared->num_processes = num_processes; // set the number of processes
    // Allocate memory for results array
    // The results array is placed right after the SharedResults structure in memory
    // This allows us to access it using the shared pointer 
    shared->results = (unsigned long*)((char*)shared + sizeof(SharedResults)); // cast to char* for pointer arithmetic
    
    // Initialize results to 0
    for (int i = 0; i < num_processes; i++) {
        shared->results[i] = 0; 
    }
    
    return shared;
}

unsigned long add(int a, int b) {
    return a + b;
}

void *thread_work(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    unsigned long output = 0;
    
    // Use a local variable instead of modifying data->start
    int start_index = data->start;
    
    if (start_index == 0) // check if this thread handles the first element
    {
        output = data->nums[0]; // initialize with the first number
        start_index = 1; // start the loop from index 1 instead
    }
    
    for (int i = start_index; i <= data->end; i++) {
        output = data->func(output, data->nums[i]); // apply the function to the output and the current number
    }
    
    data->result = output;
    pthread_exit(NULL);
}

unsigned long process_thread_compute(int process_id, int num_of_threads, const char *filepath, 
                                    unsigned long (*f)(int, int), int total_processes, SharedResults *shared_results) // function to compute the result for each process 
                                    {
    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror("Error when opening the file");
        return 0;
    }

    struct stat sb; // gets the file size.
    // stat is like fstat but it gets the file size from the file name instead of the file descriptor
    if (fstat(fd, &sb) == -1) {
        perror("Error when getting the file size");
        close(fd);
        return 0;
    }

    char *file_numbers = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0); // map the file in the memory using mmap
    // mmap is used to map a file or device into memory, allowing for efficient file I/O
    // file_numbers is a pointer to the mapped file in memory
    if (file_numbers == MAP_FAILED) {
        perror("Error when mapping the file");
        close(fd);
        return 0;
    }

    int *nums = malloc(sizeof(int) * (sb.st_size / 2));
    int num_count = 0;

    char *token, *saveptr;
    char *copy = strdup(file_numbers);
    token = strtok_r(copy, " \n", &saveptr);
    while (token != NULL) {
        nums[num_count++] = atoi(token);
        token = strtok_r(NULL, " \n", &saveptr);
    }
    free(copy);

    // Divide the work among processes
    int numbers_per_process = num_count / total_processes;
    int process_remaining = num_count % total_processes;
    
    int process_start = process_id * numbers_per_process + (process_id < process_remaining ? process_id : process_remaining);
    int process_end = (process_id + 1) * numbers_per_process + (process_id < process_remaining ? process_id + 1 : process_remaining) - 1;
    
    // Create threads for this process
    pthread_t *threads = malloc(num_of_threads * sizeof(pthread_t));
    ThreadData *thread_data = malloc(num_of_threads * sizeof(ThreadData));

    int numbers_per_thread = (process_end - process_start + 1) / num_of_threads;
    int thread_remaining = (process_end - process_start + 1) % num_of_threads;

    for (int i = 0; i < num_of_threads; i++) {
        thread_data[i].nums = nums;
        thread_data[i].process_id = process_id;
        
        // Calculate the start and end indices for each thread within this process's range
        thread_data[i].start = process_start + i * numbers_per_thread + (i < thread_remaining ? i : thread_remaining);
        thread_data[i].end = process_start + (i + 1) * numbers_per_thread + (i < thread_remaining ? i + 1 : thread_remaining) - 1;
        
        thread_data[i].func = f;
        thread_data[i].result = 0;

        if (pthread_create(&threads[i], NULL, thread_work, &thread_data[i]) != 0) {
            perror("Error when creating the thread");
            free(nums);
            free(threads);
            free(thread_data);
            munmap(file_numbers, sb.st_size);
            close(fd);
            return 0;
        }
    }

    unsigned long process_result = 0;
    int first_result = 1;
    
    // Join threads and combine results for this process
    for (int i = 0; i < num_of_threads; i++) {
        pthread_join(threads[i], NULL);
        
        if (first_result) {
            process_result = thread_data[i].result;
            first_result = 0;
        } else {
            process_result = f(process_result, thread_data[i].result);
        }
    }
    
    // Store this process's result in shared memory
    shared_results->results[process_id] = process_result;
    
    printf("Process %d with %d threads computed result: %lu\n", process_id, num_of_threads, process_result);

    free(nums);
    free(threads);
    free(thread_data);
    munmap(file_numbers, sb.st_size);
    close(fd);
    
    return process_result;
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 4) {
        printf("Usage: %s <num_processes> <num_threads_per_process> <filepath>\n", argv[0]);
        return 1;
    }

    // Parse the number of processes and threads
    int num_processes = atoi(argv[1]); //num of processes 
    int num_threads_per_process = atoi(argv[2]); // num of threads per process
    
    if (num_processes <= 0 || num_threads_per_process <= 0) {
        printf("Error: Number of processes and threads must be positive integers\n");
        return 1;
    }

    // Get the filepath
    const char *filepath = argv[3];

    printf("Computing with %d processes, each with %d threads, using file: %s\n", 
           num_processes, num_threads_per_process, filepath);
    
    // Create shared memory for inter-process communication
    SharedResults *shared_results = create_shared_memory(num_processes); // create shared memory
    if (!shared_results) {
        return 1;
    }
    
    pid_t pid; // process ID
    int process_id; // process ID for the child process
    // Initialize the results array in shared memory
    
    // Create child processes
    for (process_id = 0; process_id < num_processes - 1; process_id++) {
        pid = fork(); // create a new process using fork 
        
        if (pid < 0) { 
            perror("Fork failed");
            return 1;
        } else if (pid == 0) {
            // Child process
            process_thread_compute(process_id, num_threads_per_process, filepath, add, num_processes, shared_results);
            exit(0);
        }
    }
    
    // Parent process does its share of the work
    process_thread_compute(num_processes - 1, num_threads_per_process, filepath, add, num_processes, shared_results); // compute the result for the last process
    
    // Wait for all child processes to complete
    for (int i = 0; i < num_processes - 1; i++) {
        wait(NULL);
    }
    
    // Combine results from all processes
    unsigned long final_result = shared_results->results[0];
    for (int i = 1; i < num_processes; i++) {
        final_result = add(final_result, shared_results->results[i]);
    }
    
    printf("Final combined result: %lu\n", final_result);
    
    // Clean up shared memory
    munmap(shared_results, sizeof(SharedResults) + num_processes * sizeof(unsigned long));
    
    return 0;
}
