#include "headers.h"

/* Modify this file as needed*/
//int remainingtime;

int *shmId; //instead of remainingtime


int main(int agrc, char * argv[])
{
    initClk();
    printf("hello from main process.c\n");
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    key_t sharedMemKey = ftok("Makefile", 65);
    int shmid = shmget(sharedMemKey, 4000, 0666 | IPC_CREAT); // crete shared
    if (shmid == -1)
    {
        perror("Error in creating shared memory");
        return -1;
    }
    shmId = (int*)shmat(shmid, (void*)0, 0);
    int previous = -1; //prevtime
    while (*shmId > 0)
    {
        // remainingtime = ??;
        while (previous == getClk()) ;
        if (previous != getClk())
        {
            // decrease 1 sec from *shmId   
            (*shmId)--;
            previous = getClk();
        }
    }
    
    destroyClk(false);
    
    return 0;
}
