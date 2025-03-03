#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int main (int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        printf(1, "Hey, (✿◕‿◕)っ \n I just wanted to let you know that this function has the structure save <file name> <what you want appended to the file>\n ");
        exit(1);
    }

    int thing = open(argv[1], O_CREATE | O_WRONLY);
    lseek(thing, 0, SEEK_END);

    if (thing < 0) 
    {
        printf(1, "Your file is not opening. \n Nothing to do about it I guess ¯_(ツ)_/¯ \n");
        exit(1);
    }

    if (strlen(argv[2]) == 0)
    {
        printf(1, "You want to write nothing written in the file ... \n Weird (￢_￢) \n");
        sleep(1);
        printf (1, "If you do, rewrite the instruction. \n And make sure it is in this format: \n <file name> <what you want appended to the file>\n (ﾉ◕ヮ◕)ﾉ*:･ﾟ✧ Good luck!!!\n");
        close(thing);
        exit(0);
    }

    char space = ' ';

    for (int i = 2; i < argc; i++)
    {
        write(thing, argv[i], strlen(argv[i]));
        write(thing, &space, 1);
    }

    close(thing);

    return 0;

}