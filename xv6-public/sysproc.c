#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "sleeplock.h"
#include "mutex.h"

int sys_fork(void)
{
  return fork();
}

int sys_clone(void)
{
  int fn, stack, arg;
  argint(0, &fn);
  argint(1, &stack);
  argint(2, &arg);
  return clone((void (*)(void *))fn, (void *)stack, (void *)arg);
}

int sys_exit(void)
{
  exit();
  return 0; // not reached
}

int sys_wait(void)
{
  return wait();
}

int sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int sys_getpid(void)
{
  return myproc()->pid;
}

int sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

int sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  if (n == 0)
  {
    yield();
    return 0;
  }
  acquire(&tickslock);
  ticks0 = ticks;
  myproc()->sleepticks = n;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  myproc()->sleepticks = -1;
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// nice system call
int sys_nice(void)
{
  int inc;
  int curr = myproc()->nice;
  argint(0, &inc);
  if (inc + curr > 19)
  {
    myproc()->nice = 19;
  }
  else if (inc + curr < -20)
  {
    myproc()->nice = -20;
  }
  else
  {
    myproc()->nice += inc;
  }
  return 0;
}

int sys_macquire(void){
  //int pastPid; uncomment!!!
  // start
  mutex *userLock;
  argptr(0, (void *)&userLock, sizeof(mutex *));
  acquire(&userLock->lk);
  // end
  while (userLock->locked) {
    // struct proc *lowerProc;
    // pastPid = userLock->pid;
    // Ptable ptable = getptable();

    // if(pastPid > myproc()->pid){
    //   //find lower priority process holding lock
    //   for(lowerProc = ptable.proc; lowerProc < &ptable.proc[NPROC]; lowerProc++){
    //     if(lowerProc->pid == pastPid){
    //       break;
    //     }
    //   }
      


    // }
    sleep(userLock, &userLock->lk);
  }
  userLock->locked = 1;
  userLock->pid = myproc()->pid;
  release(&userLock->lk);
  return 0;
}

int sys_mrelease(void){
  mutex *userLock;
  argptr(0, (void *)&userLock, sizeof(mutex *));
  acquire(&userLock->lk);
  userLock->lk.locked = 0;
  userLock->pid = 0;
  wakeup(userLock);
  release(&userLock->lk);
  return 0;
}
