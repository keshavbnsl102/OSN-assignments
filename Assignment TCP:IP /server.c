#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#define PORT 8000
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    // valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer
    // printf("%s\n",buffer);
    // send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
    // printf("Hello message sent\n");
     
     while(1)
     {   char buffer[1000000] = {0};
        char buffer2[1000000]={0};
         recv(new_socket,buffer,1024,0);
         buffer[strlen(buffer)]='\0';
         send(new_socket,"ok",3,0);
         if(strcmp(buffer,"get")==0)
         {   //printf("NOOB\n");
             while(1)
             {   
                 read(new_socket,buffer2,1024);
                 buffer2[strlen(buffer2)]='\0';
                 char name[20];
                 strcpy(name,buffer2);
                 //printf("%s\n",buffer2);
                 if(strcmp(buffer2,"end")==0)
                 {break;}
                 int fd =open(buffer2,O_RDONLY);
                 if(fd<0)
                 {   if(strcmp(buffer2,"oki")!=0)
                     {printf("Error opening file %s\n",buffer2);}
                 send(new_socket,"Invalid",8,0);
                 continue;}
                 int z=lseek(fd,0,SEEK_END);
                 lseek(fd,0,SEEK_SET);
                 char str[20];
                 sprintf(str,"%d",z);


                 send(new_socket,str,strlen(str)+1,0);
                  read(new_socket,buffer2,5);
                  int n=0;
                 while(1)
                 {   //printf("rukja\n");
                     int x=read(fd,buffer2,99999);
                     buffer2[strlen(buffer2)]='\0';
                     //printf("2\n");
                     if(x==0)
                     {printf("\n");send(new_socket,"end",4,0);buffer2[0]='\0';break;}
                     n+=x;
                     
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
              strcat(su,name);
              strcat(su," sent");
              strcat(perc,su);
              perc[strlen(perc)]='\0';
              //printf("%s\n",perc);
              write(1,perc,strlen(perc));


                     send(new_socket,buffer2,x,0);
                      recv(new_socket,buffer2,4,0);
                 }
                 



             }


         }
         else if(strcmp(buffer,"exit")==0)
         {
             break;
         }
         else
         {
             printf("wrong command\n");
             buffer[0]='\0';
         }
         

         




     }




    return 0;
}
