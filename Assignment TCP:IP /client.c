// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#define PORT 8000

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    // send(sock , hello , strlen(hello) , 0 );  // send the message.
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
    // printf("%s\n",buffer);
    
       while(1)
       {
           char buffer[1000000] = {0};
          char buffer2[1000000]={0};
           char *enter=(char *)malloc(1000*sizeof(char));
          size_t ab=0;
          char **t=(char **)malloc(1000*sizeof(char *));
          char* tok=(char *)malloc(1000*sizeof(char));
          for(int i=0;i<1000;i++)
          {t[i]=(char *)malloc(50*sizeof(char));}

          printf("client> ");
          ab=0;
          getline(&enter,&ab,stdin);
          
          
          tok=strtok(enter," \n\t");
          int a =0;
          while(tok!=NULL)
          {
              t[a]=tok;
              tok=strtok(NULL," \n\t");
              a++;

          }
          if(a==0)
          {continue;}
          t[a][0]='\0';
          //printf("%s\n",t[0]);
          send(sock,t[0],strlen(t[0]),0);
          recv(sock,buffer,1000,0);
          buffer[strlen(buffer)]='\0';
          //printf("yo\n");
          if(strcmp(t[0],"exit")==0)
          {break;}
          else if(strcmp(t[0],"get")==0)
          {  // printf("kan\n");
               for(int i=1;i<a;i++)
              {
                send(sock,t[i],strlen(t[i])+1,0);
                read(sock,buffer2,1000000);
                buffer2[strlen(buffer2)]='\0';
                if(strcmp(buffer2,"Invalid")==0)
                {printf("invalid file %s\n",t[i]);continue;}
                int j=0;
                int z=atoi(buffer2);
                send(sock,"ok",3,0);
                //printf("%d\n",z);
                int n=0;
                while(1)
                 {
                    int m=read(sock,buffer2,1000000);
                    buffer2[strlen(buffer2)]='\0';
                    n+=m;
                    if(strcmp(buffer2,"Invalid")==0)
                    {printf("\n");break;}
                    if(strcmp(buffer2,"end")==0)
                    {printf("\n");break;}
                    
                     send(sock,"oki",4,0);
                    int fd1;
                    if(j==0)
                    {fd1=open(t[i],O_CREAT|O_TRUNC|O_RDWR,0700);}
                    j++;
                    long double per=(long double)((long double)n/(long double)z);
                    char num[]="0123456789";
             per=per*100;
            char perc[50];
            perc[0]='\r';
            int a,b,c,d,e;
            a=per/100,b=((int)(per/10))%10,c=((int)(per))%10;
            d=((int)(per*10))%10;
            e=((int)(per*100))%10;
           // printf("%d %d %Lf",a,b,per);
              perc[1]=num[a];perc[2]=num[b];perc[3]=num[c];perc[4]='.',perc[5]=num[d],perc[6]=num[e],perc[7]='%';
              perc[8]='\0';
              char su[50];
              strcpy(su," of file ");
              strcat(su,t[i]);
              strcat(su," received");
              strcat(perc,su);
              perc[strlen(perc)]='\0';
              //printf("%s\n",perc);
              
              write(1,perc,strlen(perc));
                    write(fd1,buffer2,m);



                 }
               }
            send(sock,"end",4,0);
          }
          else
          {printf("wrong command\n");}

          

          


          
 



       }



    return 0;
}
