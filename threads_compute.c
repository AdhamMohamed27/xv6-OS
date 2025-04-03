#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// how to run the code: cc -o thread_program threads_compute.c -lpthread
//./thread_program 4 numbers.txt

//use those command, it is for mac, and i think it is the same for windows 
// in the offchance it is not then when we call the type the file in the terminal give it the number of threads first, then the file name

typedef struct {
    int *nums; // pointer for the array of numbers
    int start; //starting index for the thread
    int end; // ending index for the thread
    unsigned long (*func)(int,int); // function pointer to the function to be used
    unsigned long result; // result of the function 
} ThreadData; 

unsigned long Tripyramid(int n)
{
    return n * (n + 1) * (n + 2) / 6;
}

// Define the add function before it's used
unsigned long add(int a, int b) {
    return a + b;
}

void *thread_work(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    unsigned long output = 0;
    
    // Use a local variable instead of modifying data->start
    int start_index = data->start;
    
    if (start_index == 0) // check if this thread handles the first element
    {
        output = data->nums[0]; // initialize with the first number
        start_index = 1; // start the loop from index 1 instead
    }
    
    for (int i = start_index; i <= data->end; i++)
    {
        output = data->func(output, data->nums[i]); // apply the function to the output and the current number
    }
    
    data->result = output;
    pthread_exit(NULL);
}

unsigned long thread_compute(int num_of_threads, const char *filepath, unsigned long (*f)(int,int)) 
{ 
    int fd = open(filepath, O_RDONLY); // open the file to read from it 
    //fd is the file descriptor that is used to access the file 
    //A file descriptor (fd) is an integer that uniquely identifies an open file or I/O resource in a process. It acts as a handle to access files, sockets, pipes, or other resources in the operating system.
    if (fd == -1) // check if the file was opened successfully
    {
        perror("Error when opening the file"); 
        return 0; // return 0 if the file could not be opened
    }

    struct stat sb; // gets the file size.
    //stat is like fstat but it gets the file size from the file name instead of the file descriptor
    //sb is a structure that contains information about the file like size, permissions, etc...
    if (fstat(fd, &sb) == -1) // check if the file size was obtained successfully
    {
        perror("Error when getting the file size"); 
        close(fd); // close the file descriptor
        return 0; // return 0 if the file size could not be obtained
    }

    char *file_numbers = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0); //map the file in the memory using mmap
    if (file_numbers == MAP_FAILED)//Check if the mapping was successful or not
    {
        perror("Error when mapping the file"); 
        close(fd); // close the file descriptor
        return 0; // return 0 if the mapping failed
    }

    int *nums = malloc(sizeof(int) * (sb.st_size / 2)); // allocate memory for the numbers where the size has a Conservative estimate which is half the size of the file
    int num_count = 0; // counter to keep track of the number of nums in the file 

    char *token , *saveptr; // token is used to split the string into numbers and saveptr is used to save the position of the last token
    char *copy = strdup(file_numbers);// duplicate the nums string to avoid modifying the original string 
    //strdup is used to create a copy of the string in the heap memory
    token = strtok_r(copy, " \n", &saveptr); // split the string into tokens using space as a delimiter
    //strtok_r is a reentrant version of strtok that is thread-safe and allows to pass a pointer to the last token position
    while (token !=NULL) //loop until there are no more tokens
    {
        nums[num_count++] = atoi(token); // convert the token to an integer and store it in the nums array
        token = strtok_r(NULL, " \n", &saveptr); // get the next token 
    }
    free(copy); // free the copy of the string to avoid memory leaks

    pthread_t *threads = malloc(num_of_threads * sizeof(pthread_t)); // allocate memory for the threads
    // pthread_t is a data type that represents a thread in the pthread library
    ThreadData *thread_data = malloc(num_of_threads * sizeof(ThreadData)); // allocate memory for the thread data using the same size as the number of threads 

    int nums_per_thread = num_count / num_of_threads; //calculate the number of nums per thread
    int remaining_nums = num_count % num_of_threads; //calculate the remaining nums that cannot be divided evenly among the threads

    for (int i = 0; i < num_of_threads; i++) // loop through the number of threads to Initialize thread data and create threads
    {
        thread_data[i].nums = nums; // assign the nums array to the thread data
        //for calculating the start and end indices for each thread
        thread_data[i].start = i * nums_per_thread + (i < remaining_nums ? i : remaining_nums );// calculate the start index for each thread
        //If i < remaining: The thread is one of the first remaining threads, so it gets an extra element. We add i to shift the start index accordingly
        //If i >= remaining: The thread is past the extra-element range, so it only accounts for remaining extra elements assigned before it
        thread_data[i].end = (i + 1) * nums_per_thread + (i < remaining_nums ? i + 1 : remaining_nums) - 1; // calculate the end index for each thread
        //If i + 1 < remaining: The next thread index is still within the extra elements zone, so it gets i + 1 additional elements.
        //If i + 1 >= remaining: The extra elements have been fully assigned, so we just use remaining
        thread_data[i].func = f; // assign the function to the thread data
        thread_data[i].result = 0; // initialize the result to 0 

        // For creating the threads
        if (pthread_create(&threads[i], NULL, thread_work, &thread_data[i]) != 0)
        {
            perror("Error when creating the thread"); 
            free(nums); // free the nums array to avoid memory leaks
            free(threads); // free the threads array to avoid memory leaks
            free(thread_data); // free the thread data array to avoid memory leaks 
            munmap(file_numbers, sb.st_size); // unmap the file numbers and file size , i.e. the file, from the memory
            close(fd); // close the file descriptor
            return 0;
        }
    }
    unsigned long final_result = 0; // to store the final result of the function
    int first_result_of_thread = 1; // to check if the first thread has finished processing
    // Join threads and combine results
    for(int i = 0 ; i < num_of_threads ; i++)
    {
        pthread_join(threads[i], NULL); // wait for the thread to finish
        if (first_result_of_thread) // check if this is the first thread with a result
        {
            final_result = thread_data[i].result; // store the result of the first thread
            first_result_of_thread = 0; //set the flag to 0 
        }
        else {
            final_result = f(final_result, thread_data[i].result); // combine the result of the first thread with the result of the current thread
            // f is the function that is passed to the thread 
        }
    }

    free(nums); //free the nums array 
    free(threads); // free the threads array 
    free(thread_data); // free the thread data array
    munmap(file_numbers, sb.st_size); // unmap the file numbers and file size, i.e. the file, from the memory
    close(fd); // close the file descriptor
    return final_result; 
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        printf("Usage: %s <num_threads> <filepath>\n", argv[0]);
        return 1;
    }

    // Parse the number of threads
    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        printf("Error: Number of threads must be a positive integer\n");
        return 1;
    }

    // Get the filepath
    const char *filepath = argv[2];

    printf("Computing with %d threads using file: %s\n", num_threads, filepath);
    
    // Call your thread_compute function
    unsigned long result = thread_compute(num_threads, filepath, add);
    
    printf("Result: %lu\n", result);
    
    return 0;
}
