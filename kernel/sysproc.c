#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
sys_sleep(void)
{

	// do backtrace
	backtrace();
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


uint64
sys_sigalarm(void) {

  int tick;
  uint64 handler_ptr;
  void (*handler)();
  printf("==sys 1\n");
  if(argint(0, &tick) < 0)
    return -1;
  printf("==sys 2\n");
  if(argaddr(1, &handler_ptr) < 0)
    return -1;
  printf("==sys 3\n");
  if (tick == 0 && handler_ptr == 0)
		return 0;
  printf("== t6 0x%x\n", myproc()->trapframe1->t6);
  if(myproc()->trapframe1->t6 == 0xde) {
    printf("last trap not return yet.\n");
	  return 0;
  }
  handler = (void (*)())handler_ptr;
  myproc()->tick_handler = handler;
  myproc()->tick = tick;
  //handler();
  return 0;

}
uint64
sys_sigreturn(void) {
	// put back 
//	printf("==put back trapfram\n");
  struct proc *p = myproc();
		p->trapframe->ra = p->trapframe1->ra;
		p->trapframe->sp = p->trapframe1->sp;
		p->trapframe->gp = p->trapframe1->gp;
		p->trapframe->tp = p->trapframe1->tp;
		p->trapframe->a0 = p->trapframe1->a0;
		p->trapframe->a1 = p->trapframe1->a1;
		p->trapframe->a2 = p->trapframe1->a2;
		p->trapframe->a3 = p->trapframe1->a3;
		p->trapframe->a4 = p->trapframe1->a4;
		p->trapframe->a5 = p->trapframe1->a5;
		p->trapframe->a6 = p->trapframe1->a6;
		p->trapframe->a7 = p->trapframe1->a7;
		p->trapframe->s0 = p->trapframe1->s0;
		p->trapframe->s1 = p->trapframe1->s1;
		p->trapframe->s2 = p->trapframe1->s2;
		p->trapframe->s3 = p->trapframe1->s3;
		p->trapframe->s4 = p->trapframe1->s4;
		p->trapframe->s5 = p->trapframe1->s5;
		p->trapframe->s6 = p->trapframe1->s6;
		p->trapframe->s7 = p->trapframe1->s7;
		p->trapframe->s8 = p->trapframe1->s8;
		p->trapframe->s9 = p->trapframe1->s9;
		p->trapframe->s10 = p->trapframe1->s10;
		p->trapframe->s11 = p->trapframe1->s11;
		p->trapframe->epc = p->trapframe1->epc;

	  p->trapframe1->t6 = 0xed;
	return 0;
}
