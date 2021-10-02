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
int no_of_comp;
int no_of_zones;
int no_of_students;
int randoms(int a,int b)   //for generating random numbers
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
int theekhuakinhi(double a)     //checks if the antibody test was positive or negative
{
   int b=(int)((double)(RAND_MAX)*(double)a);
   int c=randoms(0,RAND_MAX);
   if(c<b)
   { return 1; }
   else
   {
      return 0;
   }
   


}
int min(int a,int b)
{if(a<=b)
 {return a;}
 else
 {
    return b;
 }
 
}
void * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (void*)shmat(shm_id, NULL, 0);
}

typedef struct comp{
int id;
int no_of_batches;
int no_per_batch;
double prob;
int zones_under[5];

}comp;

typedef struct zone{
int id;
int slots;
double prob;
int capacity;
int filled[8];
int stati[8];
int comp_assigned;


}zone;

typedef struct student{
int id;
int treated;
int chance;
int zone_assigned;

}student;

student ** students;
comp ** comps;
int over=0;
zone ** zones; 
pthread_mutex_t * mutex_for_comp;
pthread_mutex_t * mutex_for_zone;
void student_wait(student *curr_student);
int student_slotmein(student *curr_student);
void *zone_start(void * a);
void zone_2(zone *curr_zone);
void *comp_start(void * a);
void comp_2(comp *curr_comp);

//function for initialising students
void *student_start(void * a)
{
   student *curr_student=(student *)a;

  int b=randoms(1,5);     
  sleep(b);
  curr_time();
  printf("\e[1;34mStudent %d has arrived for his first round\e[0m\n",curr_student->id+1);
   curr_time();
  printf("\e[1;34mStudent %d is waiting to be allocated a zone\e[0m\n",curr_student->id+1);
  student_wait(curr_student);


}

//function for making students wait
void student_wait(student *curr_student)
{
    while(1)
    {  
       for(int i=0;i<no_of_zones;i++)
       {  
          pthread_mutex_lock(&mutex_for_zone[i]);
         if(zones[i]->slots>0)
         {zones[i]->slots--;
           curr_time();printf("\e[1;34mStudent %d has been allotted a slot in %d zone for the %d time\e[0m\n",curr_student->id+1,
           i+1,curr_student->chance);
           curr_student->zone_assigned=i;
           int a=student_slotmein(curr_student);
           if(a==1)
           {pthread_mutex_unlock(&mutex_for_zone[i]); return ;}
           else
           {
               goto area;
              
           }
           
           
         }
           goto area2;
           area:;
           pthread_mutex_unlock(&mutex_for_zone[i]); 
           int m=randoms(1,5);  
           sleep(m);
           curr_time();
           printf("\e[1;34mstudent %d has arrived for the %d time\e[0m\n",curr_student->id+1,curr_student->chance);
            curr_time();
           printf("\e[1;34mStudent %d is waiting to be allocated a zone\e[0m\n",curr_student->id+1);
           goto area3;
           area2:; 
           pthread_mutex_unlock(&mutex_for_zone[i]);
           area3:;
       }
     }


}
 //student getting vaccinated
int student_slotmein(student *curr_student)
{
   int i=0;
   int a=curr_student->zone_assigned;
   for(i=0;i<8;i++)
   {  
      if(zones[a]->filled[i]==-1)
     {   zones[a]->filled[i]=curr_student->id;
           int x=theekhuakinhi(zones[a]->prob);
           curr_student->treated=x;
           zones[a]->stati[i]=x;
           curr_student->chance++;
           
          if(curr_student->chance==4||x==1)
          {  curr_time(); 
             if(x==1)
               { printf("\e[1;34mstudent %d vaccinated on zone %d\e[0m\n",curr_student->id+1,a+1);
                  curr_time();printf("\e[1;34mStudent %d tested positive for antibodies\e[0m\n",curr_student->id+1);
                }
              else
              {  printf("\e[1;34mstudent %d vaccinated on zone %d\e[0m\n",curr_student->id+1,a+1);
                  curr_time();printf("\e[1;34mStudent %d tested negative for antibodies\e[0m\n",curr_student->id+1);
                 
              }
              
              return 1;
              }
          else
          { curr_time();printf("\e[1;34mstudent %d vaccinated on zone %d\e[0m\n",curr_student->id+1,a+1);
                  curr_time();printf("\e[1;34mStudent %d tested negative for antibodies\e[0m\n",curr_student->id+1);
            return 0;
          }
     }
   
     
   }

}
//function for zone refilling and initialisation
void *zone_start(void * a)
{  zone *curr_zone=(zone *)a;
  int i;
  for(i=0;i<8;i++)
  {curr_zone->filled[i]=-1;}
  curr_time();
  printf("\e[1;33mVaccination zone %d is entering vaccination phase\e[0m\n",curr_zone->id+1);
  
  
  while(1)
   {  int f=0;
      for(i=0;i<no_of_comp&&f==0;i++)
      { 
         pthread_mutex_lock(&mutex_for_comp[i]);
         if(comps[i]->no_of_batches>0)
           {comps[i]->no_of_batches--;
             curr_zone->capacity=comps[i]->no_per_batch;
             curr_zone->prob=comps[i]->prob;
             curr_zone->comp_assigned=i;
             curr_time();
             printf("\e[1;32mpharma company %d is delivering a batch to zone %d which has success probability %lf\e[0m\n",i+1,curr_zone->id+1,curr_zone->prob);
             curr_time();
             printf("\e[1;32mpharma company %d has delivered vaccines to zone %d\e[0m\n",i+1,curr_zone->id+1);
             for(int j=0;j<5;j++)
             {if(comps[i]->zones_under[j]==-1)
                {f=1;comps[i]->zones_under[j]=curr_zone->id;break;}
             }
             
            }

        pthread_mutex_unlock(&mutex_for_comp[i]);


      }
      if(f==1)
      { 
         while(curr_zone->capacity>0)
         {
           zone_2(curr_zone);  
           if(over==1)
           {break;}

         }
         if(over==1)
         {return NULL;}
         curr_time();
         printf("\e[1;33mVaccination zone %d has run out of vaccines\e[0m\n",curr_zone->id+1);
         
      }
   }




}

//function for making new slots
void zone_2(zone *curr_zone)
{   int i; 
   for(i=0;i<8;i++)
   { curr_zone->filled[i]=-1;curr_zone->stati[i]=-1;}
   int a =min(curr_zone->capacity,8);
   curr_zone->capacity-=a;
   curr_zone->slots=a;
   curr_time();
   printf("\e[1;33mvaccination zone %d is ready with %d new slots\e[0m\n",curr_zone->id+1,a);
   while(curr_zone->slots>0)
   {if(over==1){break;}}

   // for(i=0;i<8;i++)
   // {if(curr_zone->filled[i]!=-1)
   //   {curr_time();
   //      if(curr_zone->stati[i]==1)
   //         printf("Student %d vaccinated successfully \n",curr_zone->filled[i]+1);
   //       else 
   //       {printf("Student %d vaccinated unsuccessfully and will go through queue again\n",curr_zone->filled[i]+1);
            
   //       }     
   //    }
   // }

   for(i=0;i<8;i++)
   { curr_zone->filled[i]=-1;curr_zone->stati[i]=-1;}
   if(over==1)
   {return;}


}
//function for initialising companies
void *comp_start(void * a)
{  
   comp *curr_comp=(comp *)a;
   int seconds=randoms(2,5);  //making time
   int x=randoms(10,20);   //no of vaccines per batch
   int y=randoms(1,5);    //no of batches
   for(int i=0;i<5;i++)
   {curr_comp->zones_under[i]=-1;}
   curr_time();
   printf("\e[1;32mpharma company %d is preparing %d batches and have success probability %lf\e[0m\n",
     curr_comp->id+1,y,curr_comp->prob
   );
   sleep(seconds);
   curr_comp->no_per_batch=x;
   curr_comp->no_of_batches=y;

   curr_time();
   printf("\e[1;32mpharma company %d has manufactured %d batches and each batch has %d vaccines\e[0m\n",
              curr_comp->id+1,curr_comp->no_of_batches,curr_comp->no_per_batch);
      while(curr_comp->no_of_batches>0)
      {if(over==1)
        {break;}
      continue;
      }  
      if(over==1)
      {return NULL;}
    
     comp_2(curr_comp);



}
//function for making the companies wait till batches get over
void comp_2(comp *curr_comp)
{
   int x=0;
   while(1)    //checking if all the batches are over
   {  int flag=0;
      for(int i=0;i<5;i++)
     {  
        if(curr_comp->zones_under[i]!=-1)
           { 
              if(zones[curr_comp->zones_under[i]]->capacity>0)
              {flag=1;}

           }
     }
     if(flag==0)
     {break;}
     if(over==1)
     {return;}
   }
   curr_time();printf("\e[1;32mAll the vaccines produced by pharma company %d are over\e[0m\n",curr_comp->id+1);
  for(int i=0;i<5;i++)
  { curr_comp->zones_under[i]=-1; }
  comp_start(curr_comp);


}

int main()
{
   
   srand(time(0));
   printf("Enter the number of pharma companies: ");
   scanf("%d",&no_of_comp);
   printf("Enter the number of zones: ");
   scanf("%d",&no_of_zones);
   printf("Enter the number of students: ");
   scanf("%d",&no_of_students);
   printf("Enter the success rate prob of each company: ");
   double proba[10];
   for(int i=0;i<no_of_comp;i++)
   {  scanf("%lf",&proba[i]); }
   if(no_of_comp==0||no_of_zones==0||no_of_students==0)
   {printf("\e[1;31m\nerror:enter valid values\e[0m\n");return 0;}

   students=(student **)shareMem(no_of_students*sizeof(student *));
   comps=(comp **)shareMem(no_of_comp*sizeof(comp *));
   zones=(zone **)shareMem(no_of_zones*sizeof(zone *));
   
     //allocating memory to mutexes
   mutex_for_comp=(pthread_mutex_t *)shareMem(no_of_comp*(sizeof(pthread_mutex_t)));
    mutex_for_zone=(pthread_mutex_t *)shareMem(no_of_zones*(sizeof(pthread_mutex_t)));

   //allocating memory to thread arrays
   pthread_t * student_threads=(pthread_t *)shareMem(no_of_students*sizeof(pthread_t));
    pthread_t * comp_threads=(pthread_t *)shareMem(no_of_comp*sizeof(pthread_t));
   pthread_t * zone_threads=(pthread_t *)shareMem(no_of_zones*sizeof(pthread_t));
    
    //initialising structs for students
   for(int i=0;i<no_of_students;i++)
   {
       students[i]=(student *)shareMem(sizeof(student));
       students[i]->id=i;
       students[i]->treated=0;
       students[i]->chance=1;
       students[i]->zone_assigned=-1;
    }
    //initialising structs for pharma companies
    for(int i=0;i<no_of_comp;i++)
   {
       comps[i]=(comp *)shareMem(sizeof(comp));
      pthread_mutex_init(&mutex_for_comp[i], NULL);
      comps[i]->no_of_batches=0;
      comps[i]->no_per_batch=0;
      comps[i]->prob=proba[i];
      comps[i]->id=i;


    }
    //initialising structs for vaccination zones
    for(int i=0;i<no_of_zones;i++)
   {
       zones[i]=(zone *)shareMem(sizeof(zone));
        pthread_mutex_init(&mutex_for_zone[i], NULL);
        zones[i]->id=i;
        zones[i]->slots=0;
        zones[i]->prob=0;
        zones[i]->comp_assigned=-1;
     }
     int i;
     //launching all the threads
     for(i=0;i<no_of_students;i++)
     {          pthread_create(&student_threads[i], NULL, student_start, (void*)(students[i]));
             
      }
     for(i=0;i<no_of_zones;i++)
     {          pthread_create(&zone_threads[i], NULL, zone_start, (void*)(zones[i]));
             
      }
     
     for(i=0;i<no_of_comp;i++)
     {          pthread_create(&comp_threads[i], NULL, comp_start, (void*)(comps[i]));
             
      }

      //threads returning back
      for(i=0;i<no_of_students;i++)
     {          pthread_join(student_threads[i],NULL);
      }
      over=1;
      goto yes;
      
      for(i=0;i<no_of_zones;i++)
      {pthread_join(zone_threads[i],NULL);}
      for(i=0;i<no_of_comp;i++)
      {pthread_join(comp_threads[i],NULL);}
      yes:;

      printf("SIMULATION OVER\n");

     



return 0;
}
