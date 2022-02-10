#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  pte_t* pte;
  uint64 va;
  uint64 buf;
	int num;
	uint64 abits;
	unsigned int tbits=0;
  //printf("==helo\n");
	
  if(argaddr(0, &buf) < 0)
    return -1;
  //printf("==arg1\n");
  if(argint(1, &num) < 0)
    return -1;
  //printf("==arg2\n");
  if(argaddr(2, &abits) < 0)
    return -1;
  //printf("==get args, num: %d\n", num); 
  for(int i=0; i<num; i++) {
    va = i * PGSIZE + (uint64)buf;
		//printf("==before walk %d\n", i);
		pte = walk(myproc()->pagetable, (uint64)va, 0);
	//	printf("==after walk %d %p\n",i, *pte);
		if(pte == 0)
			return 0;
		if((*pte & PTE_V) == 0)
			return 0;
		if((*pte & PTE_A) != 0) {
			// have access
			// why need this?
			*pte &=(~PTE_A);
	//		printf("==got %d\n", i);
			tbits |= (1L << i);
	//		printf("==tbits 0x%x\n", tbits);
		}
  }
  if(copyout(myproc()->pagetable, (uint64)abits, (char *)&tbits, sizeof(tbits)) < 0)
    return -1;
  return 0;
}
#endif

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
