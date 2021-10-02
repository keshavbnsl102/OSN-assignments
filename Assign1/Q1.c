#include<sys/types.h>
#include<sys/uio.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
extern int errno;
int main(int argc, char *argv[])
{
    
    // /Users/keshavbansal/Desktop/SEM 3 BOOKS;
    long long i,j,k,n,x,y,z,m;

      

       char name[5000];
       char name1[5000];
       //printf("%lld\n",strlen(arr));
       int fd=open(argv[1],O_RDONLY );
       x=0;
       for(i=strlen(argv[1])-1;i>=0;i--)
       { if(argv[1][i]=='/')
          {break;}
         name[x]=argv[1][i];
         x++;
       }
       y=11;
       name1[0]='A',name1[1]='S',name1[2]='S',name1[3]='I',name1[4]='G',name1[5]='N',name1[6]='M',name1[7]='E',name1[8]='N',name1[9]='T',name1[10]='/';
       for(i=x-1;i>=0;i--)
       { name1[y]=name[i];

          y++;
          }
       name1[y]='\0';


         if(fd<0)
         {perror("INPUT FILE DOESNOT EXIST");return 0;}

       //printf("%d\n",fd);
          int dire=mkdir("ASSIGNMENT",0700);
          chmod("ASSIGNMENT",0700);
          //if(dire<0)
          //{perror("d_I_R_E");}
          int fd2=open(name1,O_RDWR|O_TRUNC|O_CREAT,0600);
          
          if(fd2<0)
          {perror("ERROR IN CREATING OUTPUT FILE\n");

          }
         
            int buf;
            buf=10000000;
          z=lseek(fd,0,SEEK_END);
          lseek(fd,-1,SEEK_END);
          lseek(fd2,0,SEEK_SET);
          y=((z-1)/buf)*buf;
          char *str,*srev;
          str=(char *)malloc(buf*sizeof(char));
          srev=(char *)malloc(buf*sizeof(char));
          x=z-y;
          lseek(fd,y,SEEK_SET);
          m=0;
          char num[]="0123456789";
          while(1)
           {
               read(fd,str,x);
               m+=x;
               y=0;
               for(i=x-1;i>=0;i--)
               {
                 srev[y]=str[i];
                   y++;
               }
             long double per=(long double)((long double)m/(long double)z);
             per=per*100;
            char perc[20];
            perc[0]='\r';
            int a,b,c,d,e;
            a=per/100,b=((int)(per/10))%10,c=((int)(per))%10;
            d=((int)(per*10))%10;
            e=((int)(per*100))%10;
           // printf("%d %d %Lf",a,b,per);
              perc[1]=num[a];perc[2]=num[b];perc[3]=num[c];perc[4]='.',perc[5]=num[d],perc[6]=num[e],perc[7]='%';
              write(1,perc,8);
             srev[y]='\0';
             write(fd2,srev,x);

             if(lseek(fd,0,SEEK_CUR)-x-buf<0)
             {break;}

             lseek(fd,-x-buf,SEEK_CUR);
             x=buf;

             
           
           }
           close(fd);
           close(fd2);
           write(1,"\n",1);
           return 0;
           
          

        
         




    return 0;
}
