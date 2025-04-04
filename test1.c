#include <stdio.h> //printf
#include <stdlib.h> //for atoi
#include <string.h> //strcmp to compare strings
#include <sys/time.h> //gettimeofday to measure exectuion time (down to microseconds)

//since we have different compute functions we will use function pointers to 
//point to the function we want to use so compute func takes int (threads or procs) , a file, 
//and a function pointer to the function we want to use like add
//seq_func_t is a function pointer that takes a file name and a function pointer but it doesnt take
//the number of threads or procs
typedef unsigned long (*compute_func_t)(int, const char*, unsigned long (*)(int, int));
typedef unsigned long (*seq_func_t)(const char*, unsigned long (*)(int, int));

//calling the other 4 functions
unsigned long sequential_compute(const char *filepath, unsigned long (*f)(int, int));
unsigned long parallelCompute(int coreNum, const char *filepath, unsigned long (*f)(int, int));
unsigned long mmapCompute(int procNum, const char *filepath, unsigned long (*f)(int, int));
unsigned long thread_compute(int num_threads, const char *filepath, unsigned long (*f)(int, int));

//reminder:
//1. sequential_compute: reads the file and computes the result sequentially
//2. parallel_compute: reads the file and computes the result in parallel using processes
//3. mmap_compute: multiple child processes are created with fork(). 
//all processes write their partial results into a shared memory region.
//the parent process waits for children to finish and then reads from shared 
//memory to compute the final result.
//4. threads_compute: Threads share the same memory space by default
//each thread gets a chunk of the numbers, computes a partial result, 
//and stores it in a shared variable.

//the function that i will use for computation is add 
unsigned long add(int a, int b) {
    return a + b;
}


//get exectuion time will get the start and end struct timeval objects(both in micro and seconds)
//and returns the difference in seconds using 1e6 to convert microseconds to seconds
double get_execution_time(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}    


int main(int argc , char *argv[]){
    //if the user does enter the names of the 4 compute functions 
    //print standard error stream and exit 
    if (argc != 4) {
        fprintf(stderr, "enter the correct format\n");
        return 1;
    }

    const char *method = argv[1]; //which compute function to run 
    int n_threads_or_processes = atoi(argv[2]); //number of threads or processes
    const char *filename = argv[3]; //the file name to read from

    //start the timer 

    unsigned long result; //holds 18 quintillion

    struct timeval start, end;
    gettimeofday(&start, NULL); //gets the current time and stores it in start

    //string comparision compares method to the 4 compute functions
    //and calls the corresponding function and all the functions return the addition result  
    if (strcmp(method, "sequential") == 0) {
        result = sequential_compute(filename, add);
    } else if (strcmp(method, "parallel") == 0) {
        result = parallelCompute(n_threads_or_processes, filename, add);
    } else if (strcmp(method, "mmap") == 0) {
        result = mmapCompute(n_threads_or_processes, filename, add);
    } else if (strcmp(method, "threads") == 0) {
        result = thread_compute(n_threads_or_processes, filename, add);
    } else {
        fprintf(stderr, "Unknown method: %s\n", method);
        return 1;
    }

    gettimeofday(&end, NULL); //gets the current time and stores it in end

    double time_taken = get_execution_time(start, end);
    printf("Method: %s, N: %s, Threads/Procs: %d, Time: %.6f sec, Result: %lu\n",
           method, filename, n_threads_or_processes, time_taken, result);


}