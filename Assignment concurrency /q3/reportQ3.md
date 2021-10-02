Name : keshav bansal
roll : 2019101019

Q3

Threads:
I have initialised threads only for the musicians and singers.

Semaphores:
Variable for semaphore-> "sem",
I have used semaphores only for the purpose of t-shirt distribution by coordinators.

Mutexes:
mut,mut2 : used them while searching for empty stages.

Logic:
I have used timed conditional waiting when an artist fails to find an empty required stage. When the performance of the artist ends I am calling the function perf_ends()
and I am clearing the stage and giving signal to the conditional wait to allow other threads to pass through. 

Once the performer is done he goes to the tshrirt where I have initialised a semaphore with the value ,i.e. number of coordinators.

For checking if a stage is available or not, I am running a for loop inside which there is a  mutex so that no two threads go into a particular stage at the same time.  

when a singer joins a musician, I am setting the value of a variable named "imp_flag" and making the thread sleep for 2 more seconds.
