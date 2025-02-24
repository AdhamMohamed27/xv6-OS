#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) 
{
    if (argc < 3) 
    {
        printf(1, "Hey, (✿◕‿◕)っ \n I just wanted to let you know that this function has the structure: save <file name> <what you want appended to the file>\n ");
        exit();
    }

    char buffer[512];
    int thing2 = 0;

    int thing = open(argv[1], O_RDONLY);

    if (thing >= 0) 
    {
        thing2 = read(thing, buffer, sizeof(buffer) - 1);
        close(thing);
    }

    write(thing, " ", 1);

    thing = open(argv[1], O_CREATE | O_WRONLY);

    if (thing < 0) 
    {
        printf(1, "Your file is not opening. \n Nothing to do about it I guess ¯_(ツ)_/¯ \n");
        exit();
    }

    if (thing2 > 0)
    {
        write(thing, buffer, thing2);
    }

    if (thing2 > 0)
    {
        write(thing, "\n", 1);
    }


    for (int i = 2; i < argc; i++)
    {
        write(thing, argv[i], strlen(argv[i]));
        if (i < argc - 1) { 
            write(thing, " ", 1);
        }
    }

    close(thing);
    return 0;
}
