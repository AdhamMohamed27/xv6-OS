#include "types.h" //int, char etc
#include "stat.h" //file related sys call info
#include "user.h" //sys call like opne read etc
#include "fcntl.h" //file related flags

int sequential_compute(char *filename, int (*f)(int, int)) { //the func takes the filename and a function pointer as arg

    int fd = open(filename, O_RDONLY); //open a file in read only mode

    if(fd < 0) { 
        printf(1, "Error: Cannot open file %s\n", filename); //if file is not found
        return -1;
    }

    //since there isnt a way in xv6 to scan a file like fscanf in C, we will read the file manually

    char buffer[512]; //buffer to store the file content
    int n = read(fd, buffer, sizeof(buffer)); //read the file content and store it in buffer where n stores the number of bytes actually read
    printf(1, "File content: %s\n", buffer); //print the file content from the buffer "just for debugging"

    close(fd); //close the file

    char *pointer = buffer; //lets point to the start of the buffer
    int result = atoi(pointer); //initialize result to the first number in the file. why? bec our f will take 2 args and we need to pass 
    // the first number as the first arg to f and the next number as the second arg to f
    int sign = 1; //a flag to handle input neg nums

    //we wanna iterate through the buffer char by char and convert the numbers to int
    while (*pointer) {
        if (*pointer == '-') { //if we encounter a '-' sign, we set the sign flag to -1
            sign = -1;
            pointer++; //move past the neg char to read the num 
        } 
        if (*pointer == ',' || *pointer == ' ' || *pointer == '\n') {
            pointer++; //if we encounter a comma, space or newline, we move to the next char
        } else {
            int num = atoi(pointer) * sign; //convert the char to int, also of the num is neg we multiply that num with -1 which will make it neg
            //if the num is nt neg we will simply multiply the converted char to 1 which will not change anything
            result = f(result, num); //call the function f with the result and the next number
            pointer++; //move to the next char until we encounter a comma, space or newline 
            sign = 1; //reset the sign flag

            // printf(1, "parsed number: %d\n", num); //print the parsed nums for debugging
        }
    }

    return result;
}

int add(int a, int b) { //this add fucntion is passed to sequential_compute as f pointer
    return a + b;
}

//implement main
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(1, "Enter more arguments\n");
        exit();
    }

    int result = sequential_compute(argv[1], add); //call sequential_compute with the filename argv[1] and the add function
    printf(1, "Result: %d\n", result); 

    exit();
}    

