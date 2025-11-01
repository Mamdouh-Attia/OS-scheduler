//@mamdouh
#include "headers.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
void clearResources(int);
int msgQ;
struct  Process *P;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    //  TODO Initialization
    //==================  Variables  ==================//
    char* fileName = (char*)malloc(40 * sizeof(char));
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
    P = (struct Process*)malloc(100 * sizeof(struct Process));
    // 2. Read the input files.
    FILE* processesInput;
    processesInput = fopen("processes.txt", "r");
    if (processesInput == NULL)
    {
        perror("Error While reading processes.txt file\n");
        exit(-1);
    }
    int a, b, c, d;
    //TODO: Change this method to skip first line
    char q[10], w[10], e[10], r[10];
    fscanf(processesInput, "%s %s %s %s", q, w, e, r);
    int NO_Processes = 0;
    while (fscanf(processesInput, "%d %d %d %d", &a, &b, &c, &d) != -1)
    {
        P[NO_Processes].id = a;
        P[NO_Processes].arrivalTime = b;
        P[NO_Processes].runTime = c;
        P[NO_Processes].remningTime = c;
        P[NO_Processes].priority = d;
        NO_Processes++;
    }
    char Processes[500];
    sprintf(Processes, "%d", NO_Processes);
   
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
   
    // 3. Initiate and create the scheduler and clock processes.
    printf("\n=====================================================================\n");

    int CLK_PID = fork();
    if (CLK_PID == -1)
    {
        printf("Error in fork!\n");
        
    }
    else if (CLK_PID == 0)
    {
        printf("??????????????????????????!");
         execl("clk.out", "clk", NULL);
    }
    int Sch_PID = fork();
    if (Sch_PID == -1)
    {
        printf("Error in fork!");
        //exit(2);
    }
    else if (Sch_PID == 0)
    {
         printf("??????????????????????????!");
        system("gcc scheduler.c -o scheduler.out");
        if (argc > 4)
        {
            execl("scheduler.out", "scheduler", argv[3], Processes, argv[5], NULL);
        }
        else
        {
            printf("??????????????????????????!");
            execl("scheduler.out", "scheduler", argv[3], Processes, NULL);
        }
    }
    FILE *f;
    f = fopen("key", "r");           // this file 3la4an a4wof el data mn 3and doo7aa, ana w do7aa lazem nazbatha swa
    key_t key_id = ftok("key", 'a'); // same id of above lazem azabt ma3 do7aa
    msgQ = msgget(key_id, 0666 | IPC_CREAT);
    if (msgQ == -1)
    {
        perror("Error in creating message queue");
        return -1;
    }

    // sleep(1);
    //  4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x;
    // int x = getClk();
    // printf("current time is %d\n", x);
    // TODO Generation Main Loop
    int i = 0;
    x = -1;
    while (i < NO_Processes) // send info
    {
        P[i].valid = 0;
        // printf("current time is %d\n", x);
        /* code */
        // sleep(1);
        int tmp = P[i].arrivalTime; // arrival 1  <=  CLK 2
        // 5. Create a data structure for processes and provide it with its parameters.
        // 6. Send the information to the scheduler at the appropriate time.
        while (x == getClk());//revise this@@@@@@@@@@@@@@@@@@@@@
            
        // loop while p[i].arrivaltime == clk
        x = getClk();
        if (tmp > getClk())
        {
            //send a not-valid process until we reach appropriate sending time 
                int send_val = msgsnd(msgQ, &P[i], sizeof(P[i]), !IPC_NOWAIT);
                if (send_val == -1)
                    perror("\nErrror in sending process\n");
                
        }
        else
        {
            while (tmp == getClk())
            {
                /* send ==  set valid==true */
                P[i].valid = 1;
                int send_val = msgsnd(msgQ, &P[i], sizeof(P[i]), !IPC_NOWAIT);
                if (send_val == -1)
                    perror("\nErrror in sending process\n");
                i++;
                tmp = P[i].arrivalTime;
            }
        }

        // i++;//send

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
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    destroyClk(true);
   // msgctl(msgQ, IPC_RMID, (struct msqid_ds *)0);
     kill(getpid(), SIGKILL);
    // 7. Clear clock resources
    
    exit(0);
}
