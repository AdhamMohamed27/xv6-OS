#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_setpriority(void) 
{
  int pid, pr;

  if (argint(0, &pid) < 0 || argint(1, &pr) < 0)  //incorrect input
    return -1;

  struct proc *p;  //pointer to go through processes
  acquire(&ptable.lock);  //lock so no changes can happen while we are changing
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) //iterating
  { 
    if (p->pid == pid) 
    { //process found
      int old = p->priority; //saving old priority valye
      p->priority = pr; //setting new priority value
      release(&ptable.lock); //unlocking before return
      return old;  //return old priority value
    }
  }
  release(&ptable.lock);
  return -1; // can't find pid
}


int sys_printptable(void) 
{
  struct proc *p; //to iterate

  acquire(&ptable.lock); //locking to avoid changes happening while printing
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) //iterating
  {
    if (p->state != UNUSED) //only printing 'active' processes (running, ready, sleeping, ...)
    {
      cprintf("name: %s, pid: %d, state: %d, priority: %d\n",
              p->name, p->pid, p->state, p->priority); //printing
    }
  }
  release(&ptable.lock); //locking
  
  return 0;
}
