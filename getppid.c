//Before creating this file in your xv6 make sure to implement the getppid() sys call in the tutorial

#include "types.h" //integer types
#include "user.h" //for system calls like fork() and getpid() and exit() which i will use here 

//we are running this in userspace
int main (){

    int parent_pid = getpid(); //get the parent process id using the already existing system call getpid() which i will use to compare with the child process id
    // to make sure that my getppid() system call is working correctly
    printf(1, "Parent pid: %d\n", parent_pid); //print the parent process id on the terminal 

    int pid = fork(); //create a child process using fork() sys call and as we learned the child will start exectuing from the same point as the parent
    //bec the child is an exact copy of the parent and fork() returns the child pid to the parent and 0 to the child

    if (pid < 0) { //if the fork() returned a neg value this means that the child was not created
        printf(1, "Fork failed\n"); //print that the fork failed

    } else if (pid == 0) { //the getppid() is called by the child
        int ppid = getppid(); //get the parent pid of the child using the getppid() system call i implemented
        printf(1, "Child process: parent pid = %d\n", ppid); //if my implementation of the getppid() sys call is correct then the ppid = parent_pid

        if (ppid == parent_pid){
            printf(1, "Test completed the system call getppid() is working\n"); //print that the test is completed and the getppid() system call is working
        }
        else{
            printf(1, "Test failed the system call getppid() is not working and a wrong number was returned\n", ppid, parent_pid); 
        }
    
        exit(); //after the test the child process will terminate it self using the exit() sys call

    } else { //this else handles the case when (pid > 0) why do we handle it? bec the parent process has to wait for the child to finish executing
        //and if we didnt handle it the child will become a zombie if the parent exited before the child is done executing
        wait(); //let the parent process wait for the child process to finish executing
    }

    exit(); //when the child exits the parent exits and we are done 

}
