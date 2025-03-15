#include <stdio.h> //i/o functions
#include <stdlib.h> //dynamic memory allocation functions


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

//main function
int main(int argc, char *argv[]){ 
    if (argc < 2){  //i.e the user didnt pass the file name or only the file name 
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    //else call the sequential_compute function and pass the file name argv[1] and the f function add 
    int result = sequential_compute(argv[1], add); 
    printf("Result: %d\n", result); 
    return 0;
}

