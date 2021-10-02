Name: keshav Bansal 
Roll number : 2019101019


IMPLEMENTATION FOR SCHEDULING:

FCFS:  I have added a variable called ‘ctime’ in the structure of the proc which allows me to select the process with the least creation time to be selected till it gets over.

PBS:  The process with the least priority is selected and made to run and at every clock tick the process with lower priority is checked. In case a process with equal priority is found, I am interrupting the current one in order to run them in a round robin fashion.

MLFQ: I have taken the value of a variable ‘age’ as 18 which i am comparing for all the process waiting times and I am sending the process in a higher priority queue after checking that condition.


SYSTEM CALLS AND USER PROGRAMS:

WAITX 
I have increased the size of the proc structure and included more variables like ctime, runtime,endtime and in the waitx system call I am calculating wait time by subtracting the ctime and runtime from endtime.

I.e. wtime=end time-ctime-runtime.
I have implemented a time.c user program for this.

set_priority:

          I have implemented a system call and user program which just looks for the process with a given pid in the ptable and updates the value of priority.

 showps:
The user program ps.c uses this system call by iterating on the process table and prints all the details of the processes in the system call itself.



GRAPH(BONUS):

I have made three graphs for various sleeping times, aging time and run time slices per queue.

Observations for different processes:

FOR CPU BOUND PROCESSES:

The PID 4,5 are  CPU BOUND PROCESSES and they remain in the lower queues ,i.e. They remain in queue number 3 and queue number 4 and keep on interchanging between these queues.
      
  The pid 7,8 are I/O bound processes and they remain in the higher priority queues for most of their time.
