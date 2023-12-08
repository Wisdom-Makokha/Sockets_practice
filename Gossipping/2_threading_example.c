#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//let us create a global variable oto change it in threads
int g = 0;

//the function to e executed by all threads
void *mythread(void *vargp)
{
    //store the value arguement passed to this thread
    int *myid = (int *)vargp;

    //let us create a static variable to observe its changes
    static int s = 0;

    //change static and global variables
    ++s; 
    ++g;

    //print the arguement, static and global variables
    printf("Thread ID: %d, Static: %d, Global: %d\n", *myid, s++, g++);

    return NULL;
}

int main(void)
{
    int i;
    pthread_t tid;

    for(i = 0; i < 3; i++)
        pthread_create(&tid, NULL, mythread, (void *) &tid);

    pthread_exit(NULL);

    exit(EXIT_SUCCESS);
}