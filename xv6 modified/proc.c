#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
void push(int a,struct proc* b);
void remove(int i,struct proc *p);
struct proc* pop(int a);
struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  for(int i=0;i<5;i++)
  {p->qnt[i]=0;}
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->ctime=ticks;
  p->etime=0;
  p->runtime=0;
  p->waittime=0;
  p->qn=0;
  p->curtime=0;
  p->nrun=0;
  p->priority=60;
  p->flagq=0;
  

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;
  #ifdef MLFQ
  push(0,p);
  #endif

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;
  #ifdef MLFQ
  push(0,np);
  #endif

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->etime=ticks;
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

int
waitx(int *wtime,int *rtime)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->etime=ticks;
        *rtime=p->runtime;
        *wtime=(p->etime-p->ctime)-p->runtime;
        if(*wtime<0)
        {*wtime=(-1)*(*wtime);}
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;

        

        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  
  struct cpu *c = mycpu();
  struct proc *p=0;
  c->proc = 0;
  #ifdef ROUND_ROBIN
  cprintf("rr\n");
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->nrun++;
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
  #endif
  #ifdef FCFS
  for(;;){
    // Enable interrupts on this processor.
    sti();
       struct proc* pro=0;

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        {continue;}
        if(pro==0)
        {pro=p;}
        else
        {if(p->ctime<pro->ctime)
          {pro=p;}
          
        }
        
        }
        if(pro!=0){
      p->nrun++;
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = pro;
      switchuvm(pro);
      pro->state = RUNNING;

      swtch(&(c->scheduler), pro->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
      }
    
    release(&ptable.lock);

  }
  #endif
  #ifdef MLFQ
   //int ak=5;
  for(;;){
    // Enable interrupts on this processor.
    sti();
     
    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    int i;
    // if(ak>=1)
    // {for(i=0;i<5;i++)
    //  {
    //   cprintf("%d %d\n",face[i],back[i]);
    //  }
    // }
    // ak--;
    for(i=1;i<5;i++)
    {  if(face[i]==-1){continue;}
      if(face[i]<=back[i])
      {  
        for(int j=face[i];j<=back[i];j++)
        { p=queue[i][j];
          //cprintf("itnatoh\n");
          if(p->waittime>age)
          { 
            remove(i,p);
            p->qn--;
            //cprintf("process %d is aging to higher priority queue\n",myproc()->pid);
           push(i-1,p);
          }
        }

      }
      else
      {
        for(int j=face[i];j<NPROC;j++)
        { p=queue[i][j];
          if(p->waittime>age)
          {remove(i,p);
           p->qn--;
           push(i-1,p);
          }
        }
        for(int j=0;j<=back[i];j++)
        { p=queue[i][j];
          if(p->waittime>age)
          {remove(i,p);
           p->qn--;
           push(i-1,p);
          }
        }

        
      }
      



    }
    //cprintf("yyeye\n");
    p=0;
    ///cprintf("asdnbamd\n");
    for(int i=0;i<5;i++)
      {
        if(face[i]!=-1)
        {   
            p=pop(i);
              
              break;
        }


      }
      
      
      if(p!=0&&p->state==RUNNABLE)
      {  //cprintf("mortl\n");
         p->waittime=0;
         p->curtime=0;
         p->nrun++;
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();
    
      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
           //cprintf("botl\n");
        if(p!=0&&p->state==RUNNABLE)
        { //cprintf("totl\n");
          
          if(p->flagq==1)
          {
            p->flagq=0;
            p->curtime=0;
            p->waittime=0;
            if(p->qn!=4)
            {p->qn++;}

          }
          else
          {
            p->curtime=0;
          }
          
          //cprintf("avadakedavra\n");
          push(p->qn,p);



        }
      }
    
    release(&ptable.lock);

  }

   #endif

  #ifdef PBS
  for(;;){
    // Enable interrupts on this processor.
    sti();
     struct proc *hp=0;
     struct proc *ekaur=0;
    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        {continue;}
        hp=p;
        for(ekaur=p+1;ekaur<&ptable.proc[NPROC];ekaur++)
        {
          if(ekaur->state!=RUNNABLE)
          {continue;}
          if(ekaur->priority<hp->priority)
          {
            hp=ekaur;
          }
         }
         for(ekaur=ptable.proc;ekaur<p;ekaur++)
         {
           if(ekaur->state!=RUNNABLE)
          {continue;}
          if(ekaur->priority<hp->priority)
          {
            hp=ekaur;
          }

         }
       if(hp!=0)
     { p=hp;
      p->nrun++;
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
  }
    release(&ptable.lock);

  }
  
  #endif
    
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
   #ifdef MLFQ
   myproc()->curtime=0;
   myproc()->waittime=0;


   #endif
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      {p->state = RUNNABLE;
         #ifdef MLFQ
         p->curtime=0;
         push(p->qn,p);
         #endif

      }
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        {p->state = RUNNABLE;
         #ifdef MLFQ
         push(p->qn,p);
         #endif
        }
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int set_priority(int new_priority,int pid)
{
   struct proc *a;
   acquire(&ptable.lock);
   int b=-1;
   int f=0;
   for(a=ptable.proc;a<&ptable.proc[NPROC];a++)
   {
     if(a->pid==pid)
     {  b=a->priority;
        f=1;
       a->priority=new_priority;
       break;
     }


   }
   release(&ptable.lock);
   if(f==0)
   {cprintf("NO such pid found\n");}
  return b;

}


void push(int a,struct proc* b)
{  if(face[a]==-1)
   {goto yono;}
  for(int i=0;i<5;i++)
  {
    if(face[i]<=back[i])
    {
      for(int j=face[i];j<=back[i];j++)
      {
        if(queue[i][j]==b)
        {return;}
      }

    }
    else
    {
      for(int j=face[i];j<NPROC;j++)
      {if(queue[i][j]==b)
        {return;}
      }
      for(int j=0;j<=back[i];j++)
      {
        if(queue[i][j]==b)
        {return;}
      }      
    }
    


  }


  if((face[a]==back[a]+1)||(face[a]==0&&back[a]==NPROC-1))
  {
     cprintf("queue full\n");
     return;
  }
  yono:;
  b->curtime=0;
  b->waittime=0;
  if(face[a]==-1)
  {face[a]=0;}
  back[a]=(back[a]+1)%NPROC;
    sz[a]++;
    queue[a][back[a]]=b;
  // cprintf("keshav\n");

}

int showps(void)
{
  struct proc *a;
  acquire(&ptable.lock);
  cprintf("\nPID  Priority  State  r_time  w_time  n_run  cur_q     q0     q1      q2       q3        q4\n");
  
  for(a=ptable.proc;a<&ptable.proc[NPROC];a++)
    {
      if(a->pid==0)
      {continue;}
      char yo[10][50]={"UNUSED","EMBRYO","SLEEPING","RUNNABLE","RUNNING","ZOMBIE"};



      cprintf("%d\t%d\t%s  %d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",a->pid,a->priority,yo[a->state],a->runtime,a->waittime,a->nrun,a->qn,a->qnt[0],a->qnt[1],a->qnt[2],a->qnt[3],a->qnt[4]);



    }
   release(&ptable.lock);
   return 0;

}


 struct proc* pop(int a)
{  struct proc *p=0;
  if(face[a]==-1)
  {cprintf("can't pop from empty\n");return p;}
  p=queue[a][face[a]];
  sz[a]--;
  if(face[a]!=back[a])
  { face[a]++;
     face[a]%=NPROC;

  }
  else
  {
    face[a]=-1;back[a]=-1;
  }
  return p;
}
int preempt( int prior,int i)
{ int f=0;
   struct proc *a=0;
  if(i==1)
 {  acquire(&ptable.lock);
  for(a=ptable.proc;a<&ptable.proc[NPROC];a++)
       {
         if(a->priority<=prior&&a->pid!=0)
         {f=1;}

       }
       release(&ptable.lock);
       return f;
 }
 else
 { acquire(&ptable.lock);
  for(a=ptable.proc;a<&ptable.proc[NPROC];a++)
       {
         if(a->priority<prior&&a->pid!=0)
         {f=1;}

       }
       release(&ptable.lock);
       return f;
   
 }
 


}

void change_ticks(void)
{
  struct proc *a=0;
  //  struct proc *p=0;
  // acquire(&ptable.lock);
  //   for(p=ptable.proc;p<&ptable.proc[NPROC];p++)
  //   {
  //     if(p->pid==0||p->pid==1||p->pid==2)
  //     {continue;}
  //     if((ticks-GLOBAL)%10==0)
  //     cprintf("%d,%d,%d\n",p->pid,ticks-GLOBAL,p->qn);


  //   }



  // release(&ptable.lock);
   
  acquire(&ptable.lock);
  for(a=ptable.proc;a<&ptable.proc[NPROC];a++)
  {   
     if(a->state!=RUNNING)
      {
        a->waittime++;
        //#ifdef MLFQ
        //  if(a->waittime>age&&a->qn!=0)
        //  { 
        //    a->qn-=1;
        //    a->waittime=0;
        //    a->curtime=0;
        //    cprintf("aging process %d\n",a->pid);
        //    push(a->qn,a);

        //  }
        // #endif
      }
      else
      { 
        a->runtime++;
        #ifdef MLFQ
          a->curtime++;
          a->qnt[a->qn]++;
        #endif
        
      }
  }
   release(&ptable.lock);   

}


void remove(int i,struct proc *p)
{
   if(face[i]==back[i])
   {
     face[i]=-1;
     back[i]=-1;
    }
    else if(face[i]<=back[i])
    {int m=-1;
     int j;
      for(j=face[i];j<=back[i];j++)
      {
        if(queue[i][j]==p)
        {m=j;break;}


      }
      if(m==-1)
      {return;}
      if(m==face[i])
      {face[i]+=1;
        face[i]%=NPROC;
        return;
      }
      
      for(j=m;j>=face[i]+1;j--)
      {
        queue[i][j]=queue[i][j-1];

      }
      face[i]+=1;
      face[i]%=NPROC;



    }
    else
    { int m=-1;
      int j=0;
      for(j=face[i];j<NPROC;j++)
      {
        if(queue[i][j]==p)
        {m=j;break;}
      }
      if(m==-1)
      {
        for(j=0;j<=back[i];j++)
        {
          if(queue[i][j]==p)
           {m=j;break;}

        }
      }
      if(m==-1)
      {return;}
      if(m==face[i])
      {face[i]+=1;
        face[i]%=NPROC;
        return;
      }
      if(m>face[i])
      { 
        for(j=m;j>=face[i]+1;j--)
        {
          queue[i][j]=queue[i][j-1];

        }
        face[i]+=1;
        face[i]%=NPROC;
        return;

      }
      for(j=m;j>=0;j--)
      {
        if(j==0)
        {queue[i][j]=queue[i][NPROC-1];}
        else
        {
          queue[i][j]=queue[i][j-1];
        }
        

      }
      for(j=NPROC-1;j>=face[i]+1;j--)
      {
        queue[i][j]=queue[i][j-1];

      }
      face[i]+=1;
        face[i]%=NPROC;
        return;

      
      

      
    }
    



}



