#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

//A normal function that is executed as a thread when its name is 
//specified in pthread_create()

void *mythread(void *vargp)
{
    // sleep(1);
    printf("Printing GeeksQuiz from thread \n");
    return NULL;
}

int main(void)
{
    pthread_t thread_id;
    
    printf("Before Thread\n");

    pthread_create(&thread_id, NULL, mythread, NULL);
    pthread_join(thread_id, NULL);

    printf("After Thread \n");    
    
    exit(EXIT_SUCCESS);
}