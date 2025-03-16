#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define TrialNUMS 10
#define MOVING_AVG_WINDOW 5

typedef struct {
    int N;
    int thread_id;
    int n_proc;
} ThreadData;


int sequential_compute(const char *filename, int (*f) (int, int));
int add(int a, int b);
double execution_time_seq(int (*seq_func)(const char *, int (*f)(int, int)), const char *filename, int (*f)(int, int));
double execution_time_par(void (*par_func)(int, int), int N, int n_proc);
void parallel_computing(int N, int n_proc);
int crossover(double *seq_time, double *par_time, int size, int *N_Range);
int compare(const void *a, const void *b);
void smoothing_median(double *data, double *smoothed_data, int size, int window_size);




int main(int argc, char *argv[]){ 
    if (argc < 2){  //i.e the user didnt pass the file name or only the file name 
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    int n_proc = 6;
    int N_range[] = {10,20,30,40,50,60,70,80,90, 100, 200
                    ,300,400,500,600,700,800,900,1000,2000,
                    3000,4000,5000,6000,7000,8000,9000,10000
                    ,20000,30000,40000,50000,60000,70000,80000
                    ,90000,100000, 200000, 300000, 400000, 500000
                    ,600000, 700000, 800000, 900000, 1000000,
                    2000000, 3000000, 4000000, 5000000, 6000000,
                    7000000, 8000000, 9000000, 10000000};

    int num_N = sizeof(N_range) / sizeof(N_range[0]);

    double seq_times[num_N], par_times[num_N], smoothed_seq[num_N], smoothed_par[num_N];

    for (int i = 0; i < num_N; i++) {
        int N = N_range[i];
        seq_times[i] = execution_time_seq(sequential_compute, argv[1], add);
        par_times[i] = execution_time_par(parallel_computing, N_range[i], n_proc);
        printf("N=%d, Sequential: %.6f s, Parallel: %.6f s\n", N, seq_times[i], par_times[i]);
    }

    smoothing_median(seq_times, smoothed_seq, num_N, MOVING_AVG_WINDOW);
    smoothing_median(par_times, smoothed_par, num_N, MOVING_AVG_WINDOW);

    int crossover_N = crossover(smoothed_seq, smoothed_par, num_N, N_range);
    printf("Parallel computation outperforms sequential at N ≈ %d\n", crossover_N);

    FILE *file = fopen("Performance_results.csv", "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    fprintf(file, "N,Sequential,Parallel\n");
    for (int i = 0; i < num_N; i++) {
        fprintf(file, "%d,%.6f,%.6f\n", N_range[i], seq_times[i], par_times[i]);
    }
    fclose(file);
    printf("Results saved to Performance_results.csv\n");
    char path[256];
    realpath("Performance_results.csv", path);
    if (realpath("Performance_results.csv", path) == NULL) {
        perror("realpath failed");
    } else {
        printf("File saved at: %s\n", path);
    }
    return 0;
}

double execution_time_seq(int (*seq_func)(const char *, int (*f)(int, int)), const char *filename, int (*f)(int, int)) {
    struct timespec start, end;
    double total_time = 0.0;
    for (int i = 0; i < TrialNUMS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        seq_func(filename, f);
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_time += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    }
    return total_time / TrialNUMS;
}

double execution_time_par(void (*par_func)(int, int), int N, int n_proc) {
    struct timespec start, end;
    double total_time = 0.0;
    for (int i = 0; i < TrialNUMS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        par_func(N, n_proc);
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_time += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    }
    return total_time / TrialNUMS;
}


int sequential_compute(const char *filename, int (*f) (int, int)){ //func to process N in file sequentially
    FILE *file = fopen(filename, "r"); //just opening the file
    if (file == NULL){
        printf("Error: File not found\n");
        return 1;
    }

    int currentnum, result; //here we have the current number we are at in the file and the result which is the addition of (int a, int b)
    int first = 1; //just a flag to check if we are at the first number in the file
    char character; //to store the current character read by fgetc

    while (fscanf(file, "%d", &currentnum) == 1){ //loop for scanning all the N in the file and adding them. also we are passing the 
        //address of currentnum to fscanf so that it can store the value of N in the memory location of currentnum &
        if (first){
            result = currentnum; //if we are at the first number we assign that curr num to result else result will be empty bec there was no previous 
            //previous result before the first number
            first = 0; //then set the flag to zero
        } else {
            //pointer to function f that takes 2 numbers and returns a number so we pass the result and the current number to function f
            result = f(result, currentnum);
        }

        //fgetx reads one char at a time from a file and character stores the charcter that was read
        while ((character = fgetc(file)) == ' ' || character == ','){} //if the char read by fgetc is a space or commas the we skip it 
        //and the loop stops when we actually read a number 
        ungetc(character, file); //if the next char is an integer then we put it back in the file stream so that we can read it again
    }

    fclose(file); //after reading all the N close the file
    return result; 
}

//i chose my f fucntion to be addition 
int add(int a, int b){ //function to add 2 numbers
    return a + b;
}



void *parallel_computing_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int N = data->N;
    int thread_id = data->thread_id;
    int n_proc = data->n_proc;

    int start = thread_id * (N / n_proc);
    int end = (thread_id == n_proc - 1) ? N : (thread_id + 1) * (N / n_proc);

    double sum = 0.0;
    for (int i = start; i < end; i++) {
        sum += i * 0.01;
    }

    double *result = malloc(sizeof(double));
if (result == NULL) {
    perror("malloc failed");
    pthread_exit(NULL);
}
    *result = sum;
    pthread_exit((void *)result);
}

void parallel_computing(int N, int n_proc) {
    pthread_t threads[n_proc];
    ThreadData thread_data[n_proc];

    for (int i = 0; i < n_proc; i++) {
        thread_data[i].N = N;
        thread_data[i].thread_id = i;
        thread_data[i].n_proc = n_proc;
        pthread_create(&threads[i], NULL, parallel_computing_thread, &thread_data[i]);
    }

    double total_sum = 0.0;
    for (int i = 0; i < n_proc; i++) {
        double *thread_result;
        pthread_join(threads[i], (void **)&thread_result);
        total_sum += *thread_result;
        free(thread_result);
    }
}

int compare(const void *a, const void *b) {
    double diff = (*(double *)a - *(double *)b);
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}


void smoothing_median(double *data, double *smoothed_data, int size, int window_size) {
    for (int i = 0; i < size; i++) {
        int count = 0;
        double window[window_size]; // Temporary array for storing window elements

        // Extract elements within the window
        for (int j = i - window_size / 2; j <= i + window_size / 2; j++) {
            if (j >= 0 && j < size) {
                window[count++] = data[j];
            }
        }

        // Sort the elements in the window
        qsort(window, count, sizeof(double), compare);

        // Calculate median
        if (count % 2 == 1) {
            smoothed_data[i] = window[count / 2]; // Odd number of elements
        } else {
            smoothed_data[i] = (window[count / 2 - 1] + window[count / 2]) / 2.0; // Even number of elements
        }
    }
}


int crossover(double *seq_time, double *par_time, int size, int *N_Range) {
    for (int i = 0; i < size; i++) {
        if (seq_time[i] > par_time[i]) {
            return N_Range[i];
        }
    }
    return -1;
}



