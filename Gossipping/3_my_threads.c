#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

void *stopclock(void *vargp)
{
    bool *stop = (bool *) vargp;
    
    char response;
    while(!(*stop))
    {
        printf("Stop the count? ");
        scanf("%c", &response);
        if(response == 'y')
            *stop = true;
    }

    return NULL;
}

int main(void)
{
    pthread_t stopwatch;
    int i = 0;
    
    bool flag = false;
    bool *stop = &flag;

    pthread_create(&stopwatch, NULL, stopclock, (void *)stop);

    while(!(*stop))
    {
        i++;
        sleep(1);
    }

    printf("Your time ran for %d seconds\n", i);

    pthread_exit(NULL);

    exit(EXIT_SUCCESS);
}