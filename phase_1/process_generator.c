#include "headers.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
void clearResources(int);

int msgQ;
struct Process *P;
#define n 2

int *shmIdTerm, shmidterm; //for the running process
int main(int argc, char *argv[])
{

    signal(SIGINT, clearResources);
    char *fileName = (char *)malloc(40 * sizeof(char));
    int chosenAlgorithm = -1;
    int processParam = -1;
    // 1. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    if (argc < 4)
    {
        perror("Invlaid number of arguments");
        exit(-1);
    }
    else
    {
        strcpy(fileName, argv[1]);
        chosenAlgorithm = atoi(argv[3]);
        if (argc > 4)
            processParam = atoi(argv[5]);
    }

    

    // 5. Create a data structure for processes and provide it with its parameters.
    P = (struct Process *)malloc(100 * sizeof(struct Process));
    // 2. Read the input files.
    FILE *Input;
    Input = fopen("processes1.txt", "r");
    if (Input == NULL)
    {
        perror("Error While reading processes.txt file\n");
        exit(-1);
    }
    int x1, x2, x3, x4;
    //TODO: Change this method to skip first line
    char a[10], b[10], c[10], d[10];
    fscanf(Input, "%s %s %s %s", a, b, c, d);
    int numOfProcesses = 0;
    while (fscanf(Input, "%d %d %d %d", &x1, &x2, &x3, &x4) != -1)
    {
        P[numOfProcesses].id = x1;
        P[numOfProcesses].arrivalTime = x2;
        P[numOfProcesses].runTime = x3;
        P[numOfProcesses].remningTime = x3;
        P[numOfProcesses].priority = x4;
        // P[numOfProcesses].memSize = x5;
        numOfProcesses++;
    }
    char numProcesses[500];
    sprintf(numProcesses, "%d", numOfProcesses);

    // 3. Initiate and create the scheduler and clock processes.
    int clkforking = fork();
    if (clkforking == -1)
    {
        perror("Errorrrrrrrrrrrrrrrrrrrrrrrrrrr \n");
    }
    else if (clkforking == 0)
    {
        execl("clk.out", "clk", NULL);
        //exit(0);
    }

    int schedulerforking = fork();
    if (schedulerforking == -1)
    {
        perror("Errorrrrrrrrrrrrrrrrrrrrrr \n");
    }
    //TODO Send procHeaders while executing
    else if (schedulerforking == 0)
    {
        printf("forking scheduler now...\n");
        system("gcc scheduler.c -o scheduler.out");
        if (argc > 3)
            execl("scheduler.out", "scheduler", argv[3], numProcesses, argv[5], NULL);
        else
            execl("scheduler.out", "scheduler", argv[3], numProcesses, NULL);
       
    }
    FILE *f = fopen("key", "r");
    key_t key_id = ftok("key", 'a');
    msgQ = msgget(key_id, 0666 | IPC_CREAT);
    if (msgQ == -1)
    {
        perror("Errorrrrrrrrrrrrrrrr IN message queue");
        return -1;
    }
    else
    {
        printf("msgQ created Successfully \n");
    }

   

    // 4. Use this function after creating the clock Process to initialize clock.
    initClk();
    int i = 0;
    int x = -1;
    while (i < numOfProcesses)
    {
        int temp = P[i].arrivalTime;
        P[i].valid = false;
        while (x == getClk())
           ;
        x = getClk();
        if (temp > getClk())
        {
            printf("%d\n", getClk());
            int valid = msgsnd(msgQ, &P[i], sizeof(P[i]), !IPC_NOWAIT);
            if (valid == -1)
                printf("Errror in sendding #%d\n", i);
        }
        else
        {
            // 6. Send the information to the scheduler at the appropriate time.
            while (temp == getClk())
            {
                P[i].valid = true;
                P[i].sendTime = temp;
                printf("Sending Process#%d\n", i);
                int valid = msgsnd(msgQ, &P[i], sizeof(P[i]), !IPC_NOWAIT);
                if (valid == -1)
                    printf("Errror in sending #%d\n", i);
                i++;
                temp = P[i].arrivalTime;
            }
        }
    }

    // TODO Generation Main Loop
    // 7. Clear clock resources
    while (1)
    {
        printf("Here");
        Process temp;
        temp.mtype = 1;
        temp.valid = false;
        int val = msgsnd(msgQ, &P[i], sizeof(P[i]), !IPC_NOWAIT);
        while (x == getClk())
            ;
        x = getClk();
    }
    destroyClk(false);
}

void clearResources(int signum)
{
    //printf("Terminate msgQ from generator\n");
    //msgctl(msgQ, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    kill(getpid(), SIGKILL);
    //signal(SIGINT, clearResources);
}



 