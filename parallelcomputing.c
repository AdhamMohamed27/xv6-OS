#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h> 

int biggerFunction (int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
    
}

int parallelCompute (const char *fileName, int (*f) (int, int))
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

    int coreNum;

    printf("How many cores do you want?? (✿◕‿◕)っ  \n");
    printf("Core Number: ");
    scanf("%d", &coreNum);
    printf("\n");

    if (coreNum <= 0 || coreNum > actualSize) 
    {
        printf("You have an invalid core number (╥_╥)\n Choose a value between 1 and %d.\n", actualSize);
        free(arr);
        exit(1);
    }

    int chunk_size = actualSize / coreNum;
    int remainder = actualSize % coreNum;

    int pipes[coreNum][2];

    for (int i = 0; i < coreNum; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            printf("couldn't create the pipe (〃￣ω￣〃ゞ) \n Sorry for disappointing you. \n");
            free(arr);
            exit(1);
        }
       
        pid_t child = fork();
        if (child == 0)
        {
            close(pipes[i][0]);

            int *temp2 = (int *)malloc(chunk_size * sizeof(int));

            if (i == coreNum -1)
            {
                chunk_size = chunk_size + remainder;
            }

            int start = i * chunk_size;
            int end = (i + 1) * (chunk_size) - 1;

            memcpy(temp2, &arr[start], (end - start + 1) * sizeof(int));

            /*for (int j = 0; j < chunk_size; j++)
            {
                temp2[j] = arr[start + j];
            }*/
            
            int answer = temp2[0];

            for (int k = 1; k < chunk_size; k++)
            {
                answer = biggerFunction(answer, temp2[k]);
            }

            write(pipes[i][1], answer, sizeof(int));
            close(pipes[i][1]);
            
            //printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());

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

    for (int i = 0; i < coreNum; i++)
    {
        close(pipes[i][1]); 
    }

    int realAns = 0;
    read(pipes[0][0], &realAns, sizeof(int));
    close(pipes[0][0]);

    for (int i = 1; i < coreNum; i++)
    {
        int tempAns = 0;
        read(pipes[i][0], &tempAns, sizeof(int));
        realAns = biggerFunction(realAns, tempAns);

        close(pipes[i][0]);
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

    int result = parallelCompute(fileName, biggerFunction); 
    printf("Result: %d\n", result); 

    return 0;
}


