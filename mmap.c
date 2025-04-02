#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h> 
#include <sys/mman.h>

int biggerFunction (int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
    
}

int mmapCompute (const char *fileName, int (*f) (int, int))
{
    int *arr = NULL;
    int *temp = NULL;
    int num = 0;
    int size = 10;
    int actualSize = 0;

    FILE *file = fopen(fileName, "r");
    if (!file) 
    {
        printf("Hmmmm the file won't open, can you please fix that?\n");
        exit(1);
    }

    arr = (int *)malloc(size * sizeof(int));
    if(arr == NULL)
    {
        printf("You have a memory allocation problem, resolve it.\nPlease...? \n (〃￣︶￣〃) \n");
        exit(1);
    }

    while (fscanf(file, "%d%*[,]", &num) == 1) 
    {
        arr[actualSize] = num;
        actualSize++;

        if (actualSize >= size)
        {
            size = size * 2;
            temp = (int *)realloc(arr, size * sizeof(int));
            if (temp == NULL)
            {
                printf("You have a memory allocation problem, resolve it.\nPlease...? \n (〃￣︶￣〃) \n");
                free(arr);
                exit(1);
            }
            else 
            {
                arr = temp;
            }
        }

    }

    fclose(file);

    printf("File is LOADED!! (╯°□°）╯");

    int procNum;

    printf("How many processes do you want?? (✿◕‿◕)っ  \n");
    printf("Number of Processes: ");
    scanf("%d", &procNum);
    printf("\n");

    if (procNum <= 0 || procNum > actualSize) 
    {
        printf("You have an invalid core number (╥_╥)\n Choose a value between 1 and %d.\n", actualSize);
        free(arr);
        exit(1);
    }

    int chunk_size = actualSize / procNum;
    int remainder = actualSize % procNum;

    int *ptr = mmap(NULL, procNum * sizeof(int),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, 0, 0);

    if (ptr == MAP_FAILED) {
        printf("couldn't create the map (〃￣ω￣〃ゞ) \n Sorry for disappointing you. \n");
        exit(1);
    }

    for (int i = 0; i < procNum; i++)
    {
       
        pid_t child = fork();
        if (child == 0)
        {
            int start = i * chunk_size;
            
            if (i == procNum -1)
            {
                chunk_size = chunk_size + remainder;
            }

            int *temp2 = (int *)malloc(chunk_size * sizeof(int));

            int end = start + (chunk_size) - 1;

            memcpy(temp2, &arr[start], (end - start + 1) * sizeof(int));

            int answer = temp2[0];

            for (int k = 1; k < chunk_size; k++)
            {
                answer = biggerFunction(answer, temp2[k]);
            }

            ptr[i] = answer;

            free(temp2);
            exit(0);
        }
        else if (child < 0)
        {
            printf("Oh no!!! Child failed (╥_╥)\n");
            free(arr);
            exit(1);
        }
    }

    while(1)
    {
        int child_pid = wait(NULL);
        if (child_pid > 0)
        {
            printf("btw the child process %d has just finished, yay! (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧ \n", child_pid);
        }
        else
        {
            break;
        }
    }

    int realAns = ptr[0];
   
    for (int i = 1; i < procNum; i++)
    {
        int tempAns = ptr[i];
        realAns = biggerFunction(realAns, tempAns);
    }

    int err = munmap(ptr, procNum * sizeof(int));
    if (err != 0) {
        printf("I am so sad, the unmapping failed!!!!\n");
        return 1;
    }
    
    free(arr);
    return realAns;

}

int main ()
{
    char fileName[100];
    
    printf("Can you give me the file name you want to open (✿◕‿◕)っ ??? \n");
    printf("File Name: ");
    scanf("%s", fileName);
    printf("\n");

    int result = mmapCompute(fileName, biggerFunction); 
    printf("Result: %d\n", result); 

    return 0;
}