#include<sys/types.h>
#include<sys/uio.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
extern int errno;
int arr[27]={0};
int main(int argc, char *argv[])
{
    long long  i,j,k,x,y,z,zo,buf,f;
    for(i=0;i<27;i++)
    {arr[i]=-1;}
    int ne=open(argv[1],O_RDONLY);
    int old=open(argv[2],O_RDONLY);

       struct stat olds,news,direc;
     if(stat(argv[1],&olds)>=0)
      {
          for(i=0;i<9;i++)
          {arr[i]=0;}
          
          if(olds.st_mode & S_IRUSR)
      {arr[0]=1;} 
    if(olds.st_mode & S_IWUSR) 
    {    arr[1]=1;}
    if(olds.st_mode & S_IXUSR)
    {arr[2]=1;} 
     if(olds.st_mode & S_IRGRP) 
     {arr[3]=1;}
    if(olds.st_mode & S_IWGRP)
    {arr[4]=1;}
    if(olds.st_mode & S_IXGRP) 
    {arr[5]=1;}
    if(olds.st_mode & S_IROTH)
    {arr[6]=1;} 
    if(olds.st_mode & S_IWOTH)
    {arr[7]=1;} 
    if(olds.st_mode & S_IXOTH) 
    {arr[8]=1;}
      }

      if(stat(argv[2],&news)>=0)
{      for(i=9;i<18;i++)
      {arr[i]=0;}
      if(news.st_mode & S_IRUSR)
      {arr[9]=1;} 
    if(news.st_mode & S_IWUSR) 
    {    arr[10]=1;}
    if(news.st_mode & S_IXUSR)
    {arr[11]=1;} 
     if(news.st_mode & S_IRGRP) 
     {arr[12]=1;}
    if(news.st_mode & S_IWGRP)
    {arr[13]=1;}
    if(news.st_mode & S_IXGRP) 
    {arr[14]=1;}
    if(news.st_mode & S_IROTH)
    {arr[15]=1;} 
    if(news.st_mode & S_IWOTH)
    {arr[16]=1;} 
    if(news.st_mode & S_IXOTH) 
    {arr[17]=1;}
}
     int g=0;
    if(stat(argv[3],&direc)<0){g=1;}
   if(g==1){goto yes;} 
   write(1,"Directory is created: YES\n",26);
     for(i=18;i<27;i++)
     {arr[i]=0;}
    if(direc.st_mode & S_IRUSR)
      {arr[18]=1;} 
    if(direc.st_mode & S_IWUSR) 
    {    arr[19]=1;}
    if(direc.st_mode & S_IXUSR)
    {arr[20]=1;} 
     if(direc.st_mode & S_IRGRP) 
     {arr[21]=1;}
    if(direc.st_mode & S_IWGRP)
    {arr[22]=1;}
    if(direc.st_mode & S_IXGRP) 
    {arr[23]=1;}
    if(direc.st_mode & S_IROTH)
    {arr[24]=1;} 
    if(direc.st_mode & S_IWOTH)
    {arr[25]=1;} 
    if(direc.st_mode & S_IXOTH) 
    {arr[26]=1;}
    goto no;
    yes:;
     write(1,"Directory is created: NO\n",26);
     no:;



    if(arr[0]==1&&arr[9]==1)
    {
            f=0;
            buf=10000000;
          z=lseek(old,0,SEEK_END);
          zo=lseek(ne,0,SEEK_END);
          if(z!=zo){f=1;goto yo;}
          
          lseek(old,-1,SEEK_END);
          lseek(ne,0,SEEK_SET);
          
          y=((z-1)/buf)*buf;
          char *str,*srev,*str2;
          str=(char *)malloc(buf*sizeof(char));
          srev=(char *)malloc(buf*sizeof(char));
          str2=(char *)malloc(buf*sizeof(char));
          x=z-y;
          lseek(old,y,SEEK_SET);
          while(1)
           {
             read(old,str,x);
             read(ne,str2,x);
               
             y=0;
             for(i=x-1;i>=0;i--)
             {
               srev[y]=str[i];

               y++;
             }
             for(i=0;i<x;i++)
             {if(srev[i]!=str2[i])
               {f=1;goto yo;}

             }
           
           
             if(lseek(old,0,SEEK_CUR)-x-buf<0)
             {break;}
             lseek(old,-x-buf,SEEK_CUR);
             x=buf;

             
           
           }
           yo:;
           if(f==1)
           {   char su[]="Whether file contents are reversed in new file: NO\n";
               write(1,su,strlen(su));
           }
           else
           {   char sv[]="Whether file contents are reversed in new file: YES\n";
               write(1,sv,strlen(sv));
           }
           
           close(old);
           close(ne);

    }
    if(arr[0]==0||arr[9]==0)
    {   if(arr[0]==0)
         {write(1,"READ PERMISSION IS NOT THERE ON NEW FILE\n",42);}
         if(arr[9]==0)
         {write(1,"READ PERMISSION IS NOT THERE ON OLD FILE\n",42);}


    }
     if(ne<0)
     {perror("couldn't open new file");}
   
    if(arr[0]!=-1)
    {   
        if(arr[0]==1)
        {char u[]="User has read permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has read permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[1]==1)
        {char u[]="User has write permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has write permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[2]==1)
        {char u[]="User has execute permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has execute permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[3]==1)
        {char u[]="Group has read permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has read permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[4]==1)
        {char u[]="Group has write permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has write permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[5]==1)
        {char u[]="Group has execute permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has execute permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[6]==1)
        {char u[]="Others has read permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has read permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[7]==1)
        {char u[]="Others has write permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has write permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[8]==1)
        {char u[]="Others has execute permissions on newfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has execute permissions on newfile: NO\n";
            write(1,u,strlen(u));

        }

    



    }
      if(old<0)
     {
         perror("couldn't open old file");
     }
    if(arr[9]!=-1)
    {if(arr[9]==1)
        {char u[]="User has read permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has read permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[10]==1)
        {char u[]="User has write permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has write permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[11]==1)
        {char u[]="User has execute permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has execute permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[12]==1)
        {char u[]="Group has read permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has read permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[13]==1)
        {char u[]="Group has write permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has write permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[14]==1)
        {char u[]="Group has execute permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has execute permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[15]==1)
        {char u[]="Others has read permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has read permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[16]==1)
        {char u[]="Others has write permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has write permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[17]==1)
        {char u[]="Others has execute permissions on oldfile: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has execute permissions on oldfile: NO\n";
            write(1,u,strlen(u));

        }


    }
    if(arr[18]!=-1)
    {if(arr[18]==1)
        {char u[]="User has read permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has read permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[19]==1)
        {char u[]="User has write permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has write permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[20]==1)
        {char u[]="User has execute permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="User has execute permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[21]==1)
        {char u[]="Group has read permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has read permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[22]==1)
        {char u[]="Group has write permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has write permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[23]==1)
        {char u[]="Group has execute permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Group has execute permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[24]==1)
        {char u[]="Others has read permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has read permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[25]==1)
        {char u[]="Others has write permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has write permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
        if(arr[26]==1)
        {char u[]="Others has execute permissions on directory: Yes\n";
            write(1,u,strlen(u));
        }
        else
        {char u[]="Others has execute permissions on directory: NO\n";
            write(1,u,strlen(u));

        }
}
else
{
    perror("directory not created");
}

    
return 0;

}