#include "types.h"
#include "user.h"

int main(int argc,char *argv[])
{
 int wtime;
 int rtime;
 int pid=fork();

 if(pid<0)
 {
    printf(1,"ERROR while forking\n") ;
 }
 else if(pid==0)
 {

     exec(argv[1],argv+1);
 }
 else
 {
     waitx(&wtime,&rtime);
     printf(1,"rtime=%d wtime=%d\n",rtime,wtime);
 }
 




exit();

}