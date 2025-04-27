#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) 
{
  if (argc != 3) 
  {
    printf(1, "Hey, (✿◕‿◕)っ \n I just wanted to let you know that this function has the structure: setpriority <process pid> <priority that you want>\n ");
    exit();
  }

  int pid = atoi(argv[1]);
  int pr = atoi(argv[2]);

  int old = setpriority(pid, pr);
  if (old < 0) 
  {
    printf(1, "Couldn't set your priority :( \n Nothing to do about it I guess ¯_(ツ)_/¯ \n");
    exit();
  } else 
  {
    printf(1, "Hey, so for process <%d> your old priority was %d, but nooooow your NEW priority is %d!!!! \n Yay \n", pid, old, pr);
  }

  exit();
}
