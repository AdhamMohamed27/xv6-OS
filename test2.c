#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

//declaring the 4 computes 
int sequential_compute(const char *filepath, int (*f)(int, int));
int parallelCompute(int coreNum, const char *filepath, int (*f)(int, int));
int mmapCompute(int procNum, const char *filepath, int (*f)(int, int));
int thread_compute(int num_threads, const char *filepath, int(*f)(int, int));

//the addition function
int add(int a, int b) {
    return a + b;
}

double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}


int main(){
    const char *fileName = "numbers_1000000.txt"; //N = 1000000
    struct timeval start, end;
    double time;

    //i will run sequential compute once because it doesnt get affect by n_proc or n_threads
    gettimeofday(&start, NULL);
    int seq_result = sequential_compute(fileName, add);
    gettimeofday(&end, NULL);
    time = get_time_diff(start, end);
    printf("Sequential Time: %.6f sec, Result: %d\n", time, seq_result);


    for (int n = 1; n <= 10; n++) {
        
        //parallel compute as n goes from 1 to 10 
        gettimeofday(&start, NULL);
        int par_result = parallelCompute(n, fileName, add);
        gettimeofday(&end, NULL);
        time = get_time_diff(start, end);
        printf("Parallel (n_proc=%d): %.6f sec, Result: %d\n", n, time, par_result);


        //mmap compute as n goes from 1 to 10
        gettimeofday(&start, NULL);
        int mmap_result = mmapCompute(n, fileName, add);
        gettimeofday(&end, NULL);
        time = get_time_diff(start, end);
        printf("Mmap (n_proc=%d): %.6f sec, Result: %d\n", n, time, mmap_result);

        //threads compute as n goes from 1 to 10
        gettimeofday(&start, NULL);
        int thread_result = thread_compute(n, fileName, add);
        gettimeofday(&end, NULL);
        time = get_time_diff(start, end);
        printf("Threads (n_thread=%d): %.6f sec, Result: %d\n", n, time, thread_result);
    }

    return 0;
}