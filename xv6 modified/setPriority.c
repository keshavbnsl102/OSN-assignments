#include "types.h"
#include "user.h"

int main(int argc,char *argv[])
{
  if(argc!=3)
  {printf(1,"ERROR: USAGE: setPriority <new_priority> <pid>\n");}
  else
  {
      int a=atoi(argv[1]);
  int b=atoi(argv[2]);
  if(a>100)
  {printf(1,"INVALID PRIORITY NUMBER: PRIORITY SHOULD BE LESS THAN OR EQUAL TO 100\n");
    exit();
  }

  set_priority(a,b);
      
  }
  
  

 

exit();



}