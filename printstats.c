#include "types.h" //we use this header for handling data types 
#include "user.h" //used tp provide syscalls for user programs


//average function
float average (int *num, int totalnum){ //so we are passing the numbers entered by the user and the count of the numbers
    int sum = 0; //initially the sum is zero before iterating the list

    for(int i = 0; i < totalnum; i++){ //iterate through the input list of num till the end of the list

        sum += num[i]; //and while iterating keep adding the nums together
    }

    float average = (float)sum / totalnum;//once out of the loop return the total sum divided by the total count of num which is basically the average
    //why did i add (float)sum? bec if i didnt the division will be performed as integer division bec both sum and totalnum are integers but the average doesnt 
    //necessary have to be an int so to avoid rounding the average we convert the sum to float to force the division to be float division. 

    return average; 
}

//standard deviation 
//unfortunatly for me there is no sqrt build in function that i can use so i will have to implement it myself 

//wht approach did i use? its known as incremental approach. now lets get into what my function does. 
//now if we have a num lets say 9 and we want to guess what two numbers when multiplied together gives me this 9 but the problem is idk what these two nums are so we will have to guess them 
//so i start by creating my sqrt function that could take FP numbers and my i is my guess i.e. the numbers when multiplied together give me 9 
float sqrt (float num){
    float i;

    //now this for loop is where i try my guesses so maybe i will try my i to be =0.5 so when i do 0.5*0.5 i get 0.25 which is < 9 so its to small 
    //i keep taking different guesses until i reach the two numbers when multiplied together gives me the 9 which will be 
    //i=3 so 3*3 = 9 = 9 so we found the correct guess. now why did i initialize i=0.01 instead of zero? bec multiplying zero by itself will always give me zero
    //why am i incrementing by 0.01? its obv slower but that way i am making sure that i will always get the accurate result because i will go over all the nums
    for (i = 0.01 ; i * i <= num ; i += 0.01);
    return i; //obv after finding the correct guess we return it
}

//finally lets implement the standard deviation fun which works mathematically as follows:
//we get the average of the input list of num and we will use the average function we implemented earlier 
//next for each (num[i] - average)^2 so we get an output of another list of nums in which we will use in the next step
//now we will sqrt using the sqrt function we implemented the sum of the list of nums we got from the second step dividing by the total count of num we got from the input list 
//we will finally have the standard deviation 

float standardDeviation (int *num, int totalnum){

    float avg = average(num, totalnum); //step 1 is completed we got the average of the input list of num

    float squaredDifference = 0.0;

    for (int i = 0 ; i < totalnum ; i++) { //here is implemented step 2 inwhich we subtracted each num from the average 
        float difference = num[i] - avg;
        squaredDifference += difference * difference; //and created a float to save all the nums that we square after subtracting 
    }

    float variance = squaredDifference / totalnum; //why did i name this variable variance? bec actually without sqrt we are calculating the variance 
    //bec we can obtain the standard deviation by square rooting the variance 

    float stdrd = sqrt(variance);//which is exactly wht we did in this step to get the standard deviation 

    return stdrd;
}

//sorting :(
//since we need to get the median of a list the list must be sorted and we cant gurantee that the inputed list will be sorted so 
//i will implement a quick and easy sorting algorithm (bubblesort) to sort the list. obv the bubble sort is not the fastest or most efficient as it has O(n^2) complexity but its one 
//of the easiest to implement  

void bubblesort(int *num, int totalnum){ //a function that doesnt return anything bec it already modifies the original input list in memory
    //and takes the input list of num pointer and the total count of num in the list 
    for(int i = 0; i < totalnum - 1 ; i++){ //we have two pointers here, the outerloop pointer counts how many passes we have done and in each iteration we move the largest unsorted 
        //element to its correct position

        int swap = 0; //incase we are lucky enough and the user is kind they could input a storted list in that case we dont need to go through this function at all and waste time

        for (int j = 0; j < totalnum - i - 1 ; j++){ //after each iteration in the outerloop we have one or more elements that are sorted at the end of the list so we dont need 
            //to compare the last i elements bec they are already sorted and also this loop is responsible for comparing and swaping elements in the unsorted part of the list

            if(num[j] > num[j+1]){ //now the bubble sort works as follows we compare two adjacent elements in the list and if the element at the left is greater than 
                //the element at the right we swap them 
                int temp = num[j]; //so we store the num[j] in a temp variable 
                num[j] = num[j+1]; //and we assign the value of num[j] to num[j+1]
                num[j+1] = temp; //and finally assign back the original num[j] to be the next element in the list
                swap = 1; 

            }
        }

        if (swap == 0) break; //and when the rare case happen and we get a sorted input we just break 
    }
}

//median, max and min
//since again i had to implement everything from the start we can finally move on to obtain the median of the input list but we can also use the fact that we sorted the input list
//so we can go ahead and get the min and max from the sorted list thankfully 

void calculations (int *num, int totalnum, float *median, int *max, int *min){
    //so here we are passing the address of the input list in memory, the count of elements in the list, 
    //a pointer to store the value of the median, min and max so the result will be updated in the calling func directly

    //for the min we will simply get the first element in the sorted list 
    *min = num[0]; 

    //we will dp the same same thing in max and get the last element in the sorted list 
    *max = num[totalnum - 1];

    //now for the median we have two cases, either the inputed list has even numbers in which we will find the two middle numbers add and then divide them 
    //or the list has odd elements in which we will just find the middle index and thats it 

    if (totalnum % 2 == 0){ //if the list has even num of elements

        int middle1 = totalnum / 2 - 1 ; //we get the index of the first middle element
        int middle2 = totalnum / 2; //and here we get the index of the second middle element

        *median = (num[middle1] + num[middle2]) / 2.0 ; //why 2.0? to force FP division bec we want the correct median not the rounding of it
    }

    else{ //in the case that the num of elements are odd we go ahead and get the index of the middle number. 
        int middle = totalnum / 2;
        *median = num[middle]; 
    }
    //you may ask yourself why totalnum / 2 in either the even or odd case to get the index? bec C always does integer division and when it does it rounds the number or "floor" it 
    //meaning that if we have a list of 5 nums then we will divide 5/2 which will give us 2.5 but bec C floors the num the result will actually be 2 which is the index of the 
    //middle number in the list
}

//lets finally take a list of numbers from the user as command line arguments 
//the main takes two things the argument count and the inputed array of strings
int main(int argc , char *argv[]){

    if(argc < 2){ //incase only one argument was entered which will be the file name, output to the terminal for the user to enter more arguments
        printf(1, "Enter a list of numbers:"); 
        exit();
    }

    //since anything written to the CL will be interpreted as a string we need to convert the string to integer and thankfully we can do this 
    //using the buildin atoi in ulib.c 

    int totalnum = argc - 1; //we are subtracting the program name from the argc 
    int num[totalnum]; 
    for(int i = 0; i < totalnum ; i++){
        num[i] = atoi(argv[i+1]); //so we fill the input array with integers 
    }

    float avg = average(num, totalnum);

    float standard = standardDeviation(num, totalnum);

    bubblesort(num, totalnum);

    float median = 0.0;
    int min = 0 , max = 0;

    calculations(num, totalnum, &median, &min, &max);//passing them my reference bec in the calculations func i assigned them pointers so 
    //we can save them without returning values

    //printing to the terminal 
    //we are simply printing the whole number first and then the decimal part

    printf(1, "Average: %d.%d \n", (int)avg , (int) ((avg - (int)avg) * 100));

    printf(1, "Standard Deviation: %d.%d \n", (int)standard , (int) ((standard - (int)standard) * 100));

    printf(1, "Median: %d.%d \n", (int)median , (int) ((median - (int)median) * 100));

    printf(1, "Max: %d.%d \n", max);

    printf(1, "Min: %d.%d \n", min);


    exit();
}
