#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
#include<stdbool.h>
#include<semaphore.h>
#include<string.h>


int randoms(int a,int b)   //for random numbers
{
   return rand()%(b-a+1)+a;
}
void curr_time()
{
   time_t t;
   struct tm *timeinfo;
   time(&t);
   timeinfo=localtime(&t);
   printf("%d:%d:%d \t",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);


}

void * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (void*)shmat(shm_id, NULL, 0);
}
int no_of_artists,no_of_astages,no_of_estages,no_of_coords,t1,t2,t;
sem_t sem;
int no_of_stages;


//struct for artist
typedef struct artist{

int id;
char instrument;
int arritime;
char name[20];
int perf_time;
pthread_cond_t hmm;
struct timespec *timy;
int stage_req;
int stage_assigned;
int imp_flag;
int waiting;
}artist;

//struct for stage
typedef struct stage{
  char stagetype;
  int state;
  int id;
  int performers[2];

}stage;

//struct for coordinators
typedef struct coord{
  int id;

}coord;
int over=0;

artist ** artists;
stage ** stages;
coord ** coords;
pthread_mutex_t mut,mut2;
pthread_mutex_t ekaur;

struct timespec* bataotime(int a)
{
      struct timespec *stud=(struct timespec *)malloc(sizeof(struct timespec));
      clock_gettime(CLOCK_REALTIME,stud);
      stud->tv_sec+=a;
      return stud;


}
void artist_wait(artist *curr_artist);
void perf_ends(artist *curr_artist);
void tshirt(artist *curr_artist);

 //initialisation of artists
void *artist_start(void *a)
{
    artist *curr_artist=(artist *)a;
    curr_artist->perf_time=randoms(t1,t2);   //random performance time
    curr_artist->timy=bataotime(curr_artist->arritime+t);   //maximum waiting time
    curr_artist->stage_assigned=-1;
    sleep(curr_artist->arritime);
    curr_time();
    printf("\e[1;32m %s has arrived and plays %c instrument\e[0m\n",curr_artist->name,curr_artist->instrument);
    artist_wait(curr_artist);
     
}

//artists waiting for stage
void artist_wait(artist *curr_artist)
{  int i;
  int flag=0;
  int flag2=0;
  int flag3=0;
  pthread_mutex_lock(&mut);
     
   if(curr_artist->stage_req==0)   //if stage required is acoustic
   {
       for(i=0;i<no_of_stages;i++)
       { if(stages[i]->stagetype=='a'&&stages[i]->performers[0]==-1)
         {
             stages[i]->performers[0]=curr_artist->id;
             curr_artist->stage_assigned=i;
             flag=1;
             break;
         }

       }
       

   }
   else if(curr_artist->stage_req==1)  //if stage required is electric
   {
       for(i=0;i<no_of_stages;i++)
       {
           if(stages[i]->stagetype=='e'&&stages[i]->performers[0]==-1)
           {
            curr_artist->stage_assigned=i;
            stages[i]->performers[0]=curr_artist->id;
            flag=1;
            break;
           }
       }
   }
   else    //if stage can be both electric and acoustic
   {
       for(i=0;i<no_of_stages;i++)
       {
           if(stages[i]->performers[1]==-1)
           { 
            if(stages[i]->performers[0]!=-1&&curr_artist->instrument=='s'&&artists[stages[i]->performers[0]]->instrument!='s')   
            {curr_artist->stage_assigned=i;
            stages[i]->performers[1]=curr_artist->id;
            flag=1;
            flag2=1;
            flag3=1;
            artists[stages[i]->performers[0]]->imp_flag=1;
            }
            else if(stages[i]->performers[0]==-1)
            {
               curr_artist->stage_assigned=i;
              stages[i]->performers[0]=curr_artist->id;
              flag=1;
              if(curr_artist->instrument=='s')
              {flag3=1;}

            }

            
            if(flag==1)
             {break;}
           }
       }
       
       
   }
   
   if(flag==1) //if stage is assigned
   {     pthread_mutex_unlock(&mut);
        curr_time();
        char oy[10];
        if(stages[curr_artist->stage_assigned]->stagetype=='e')
        {strcpy(oy,"electric");}
        else
        {
            strcpy(oy,"acoustic");
        }
        

        if(flag2==1)  //in case singer joins other performance
        {printf("\e[1;33mSINGER  %s HAS JOINED  musician %s\e[0m\n",curr_artist->name,artists[stages[curr_artist->stage_assigned]->performers[0]]->name);}
        else if(flag3==1)
        {printf("\e[1;34msinger %s has started solo performance on (%s)stage %d for duration %d\e[0m\n",curr_artist->name,oy,curr_artist->stage_assigned+1,curr_artist->perf_time);}
        else
          {printf("\e[1;35m%s with instrument %c started performing on (%s)stage %d and will take %d seconds\e[0m\n",curr_artist->name,
       curr_artist->instrument,oy,curr_artist->stage_assigned+1,curr_artist->perf_time);
       }
       if(flag2==0)
        {sleep(curr_artist->perf_time);
          
        
        }
        else
        {
            pthread_mutex_lock(&mut2);
            pthread_cond_wait(&curr_artist->hmm,&mut2);
            pthread_mutex_unlock(&mut2);
            goto yes;
        }
        
       if(curr_artist->imp_flag==1)
       {sleep(2);}
       perf_ends(curr_artist);
       curr_time();
       char oy2[10];
        if(stages[curr_artist->stage_assigned]->stagetype=='e')
        {strcpy(oy2,"electric");}
        else
        {
            strcpy(oy2,"acoustic");
        }
       printf("\e[1;35mperformance by %s on (%s) stage %d ends\e[0m\n",curr_artist->name,oy2,curr_artist->stage_assigned+1);
       yes:;
       tshirt(curr_artist);
       return;
   }
   else  //artist didnt find free stage
   {  
       curr_artist->waiting=1;
      curr_time();
     printf("\e[1;36m%s couldn't find any stage and is waiting\e[0m\n",curr_artist->name);
      int x= pthread_cond_timedwait(&curr_artist->hmm,&mut,curr_artist->timy);
       pthread_mutex_unlock(&mut);
      if(x==0)
      {  
        artist_wait(curr_artist);

        return;
       }
       else
       {   curr_time();
           printf("\e[1;31m %s with instrument %c has left because of impatience\e[0m\n",curr_artist->name,curr_artist->instrument);
           return;
       }
       

    }
   
   
}
void tshirt(artist *curr_artist)  //tshirt distribution
{
   sem_wait(&sem);
   pthread_mutex_lock(&ekaur);
   curr_time();
   printf("\e[1;32m %s is collecting t-shirt\e[0m\n",curr_artist->name);
   pthread_mutex_unlock(&ekaur);
    sleep(2);
   sem_post(&sem);

}
// vacating the stage
void perf_ends(artist * curr_artist)
{ int i;
  int x=curr_artist->stage_assigned;

  pthread_mutex_lock(&mut);
  
  int num=0;

  for(i=0;i<no_of_artists&&num<2;i++)
  {   
      if(i!=curr_artist->id)
      {  
          if(artists[i]->waiting==1)
          {
              if(num==0&&artists[i]->stage_req==0&&stages[x]->stagetype=='a')
              {   
                  pthread_cond_signal(&artists[i]->hmm);num++; //sending signal to waiting performers
                  stages[x]->performers[0]=-1;
                  if(stages[x]->performers[1]!=-1)
                  { 
                      pthread_cond_signal(&artists[stages[x]->performers[1]]->hmm);
                  }                           
                   stages[x]->performers[1]=-1;
              
               }
              else if(num==0&&artists[i]->stage_req==1&&stages[x]->stagetype=='e')
              {   
                  pthread_cond_signal(&artists[i]->hmm);num++; //sending signal
                  stages[x]->performers[0]=-1;
                  if(stages[x]->performers[1]!=-1)
                  { pthread_cond_signal(&artists[stages[x]->performers[1]]->hmm);}                           
                   stages[x]->performers[1]=-1;
              
              }
              else if(artists[i]->stage_req==2&&num==0&&artists[i]->instrument!='s')
              {   
                  
                  pthread_cond_signal(&artists[i]->hmm);num++;   //sending signal
                 stages[x]->performers[0]=-1;
                  if(stages[x]->performers[1]!=-1)
                  { pthread_cond_signal(&artists[stages[x]->performers[1]]->hmm);}                            
                   stages[x]->performers[1]=-1;
              
              }
              else 
              {   
                  pthread_cond_signal(&artists[i]->hmm);num++;   //sending signal
                 stages[x]->performers[0]=-1;
                  if(stages[x]->performers[1]!=-1)
                  { pthread_cond_signal(&artists[stages[x]->performers[1]]->hmm);}                           
                   stages[x]->performers[1]=-1;
              
              
              }
            }
        }


  }
  pthread_mutex_unlock(&mut);


}

void *coord_start(void *a)
{
    coord *curr_coord=(coord *)a;

}

int main()
{
    srand(time(0));
    printf("Enter the number of artists, acoustic stages,electric stages,coordinators,t1,t2,t:\n");
    scanf("%d %d %d %d %d %d %d",&no_of_artists,&no_of_astages,&no_of_estages,&no_of_coords,&t1,&t2,&t);
    printf("Enter the details of the artists:\n");
     pthread_t *artiststhread=(pthread_t *)shareMem(no_of_artists*sizeof(artist));
     pthread_t *coordsthread=(pthread_t *)shareMem(no_of_coords*sizeof(coord));
     
     artists=(artist **)shareMem(no_of_artists*sizeof(artist *));
     stages=(stage **)shareMem((no_of_astages+no_of_estages)*sizeof(stage *));
     coords=(coord **)shareMem(no_of_coords*sizeof(coord *));
     no_of_stages=no_of_astages+no_of_estages;
       int i;
     for( i=0;i<no_of_artists;i++)
     {
        artists[i]=(artist*)shareMem(sizeof(artist));
        artists[i]->id=i;
        pthread_cond_init(&artists[i]->hmm,NULL);
        artists[i]->imp_flag=0;
        artists[i]->waiting=-1;

     }
     pthread_mutex_init(&mut,NULL);
     pthread_mutex_init(&mut2,NULL);
     pthread_mutex_init(&ekaur,NULL);
     sem_init(&sem,0,no_of_coords);

     //initialising stage structs
     for(i=0;i<no_of_astages+no_of_estages;i++)
     {
         if(i<no_of_astages)
          {
              stages[i]=(stage *)shareMem(sizeof(stage));
              stages[i]->id=i;
              stages[i]->stagetype='a';
              stages[i]->state=-1;
              stages[i]->performers[0]=-1;
              stages[i]->performers[1]=-1;
          }
          else
          {
              stages[i]=(stage *)shareMem(sizeof(stage));
              stages[i]->id=i;
              stages[i]->stagetype='e';
              stages[i]->state=-1;
              stages[i]->performers[0]=-1;
              stages[i]->performers[1]=-1;

          }
          

      }
      //initialising coord structs
      for(i=0;i<no_of_coords;i++)
      {coords[i]=(coord *)shareMem(sizeof(coord));
        coords[i]->id=i;
      }

    for(int i=0;i<no_of_artists;i++)
    {
      scanf("%s %c %d",artists[i]->name,&artists[i]->instrument,&artists[i]->arritime);
      if(artists[i]->instrument=='p'||artists[i]->instrument=='g'||artists[i]->instrument=='s')
      {artists[i]->stage_req=2;}
      if(artists[i]->instrument=='b')
      {artists[i]->stage_req=1;}
      if(artists[i]->instrument=='v')
      {artists[i]->stage_req=0;}

    }
    for(i=0;i<no_of_artists;i++)
    {
        pthread_create(&artiststhread[i],NULL,artist_start,(void *)artists[i]);
    }
    //for(i=0;i<no_of_coords;i++)
    //{//pthread_create(&coordsthread[i],NULL,coord_start,(void *)coords[i]);}

    for(i=0;i<no_of_artists;i++)
    {
        pthread_join(artiststhread[i],NULL);
    }
    over=1;
    printf("FINISH\n");
    //for(i=0;i<no_of_coords;i++)
    //{
     //   pthread_join(coordsthread[i],NULL);
    //}
    
    
    
    
    return 0;
}
