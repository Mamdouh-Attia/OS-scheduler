#include "headers.h"

//#include "buddy_algo.c"
int size;
typedef struct block_start_address
{
    int start_address;
    int end_address;
    struct block_start_address* next;
} block_start_address;

// Global vector of pairs to track all
// the free nodes of various sizes
block_start_address* arr[100];
///////////////////////////////////////mahmoud
void HPF();
void RR(int quantum);
void SRTN();
block_start_address* allocate(int s);
void deallocate(int id, int endMemoryAddress);
void Buddy(int s);
void cleaningofsharedmemory(int signum);
void clearResources(int signum);
struct Queue* q = NULL;
struct priorityQueue* pq = NULL;
struct priorityQueue* pq1 = NULL;
struct Process processRecv;

bool memory[1024];
      
bool isRunning=0;
//struct linkedlist *waitList = NULL;

int chosenAlgorithm, paramter = -1, numOfProcesses = -1;
int countProcess = 0;
int *shmId, shmid; //shared memory id of (pointer and value) the running process  mainly the remaining time
int msgQ;

FILE* schedulerLog;
FILE* MemoryLog;
int main(int argc, char * argv[])
{
    Buddy(1024);
    signal(SIGINT, cleaningofsharedmemory);

    initClk();
    
    //TODO implement the scheduler :)
    printf("hello frommmmm scheduler...\n");
    FILE* f;
    f = fopen("key", "r"); // this file 3la4an a4wof el data mn 3and doo7aa, ana w do7aa lazem nazbatha swa  
    key_t key_id = ftok("key", 'a'); // same id of above lazem azabt ma3 do7aa
    msgQ = msgget(key_id, 0666 | IPC_CREAT);
    if (msgQ == -1)
    {
        perror("Errorrrrrrrrrrrrrrrrrrrrrrrrr in msgQ ");
        return -1;
    }

    schedulerLog = fopen("schedular.log", "w");
    fprintf(schedulerLog, "at time x process y started arrive time w running time z remning time y waiting time k\n");
    MemoryLog = fopen("memory.log", "w");
    fprintf(MemoryLog, "at time x allocated y bytes for Processs z from i to j\n");



    chosenAlgorithm = atoi(argv[1]);
    numOfProcesses = atoi(argv[2]);
    if (chosenAlgorithm == 3) // RR 
        paramter = atoi(argv[3]);

    key_t sharedMemKey = ftok("Makefile", 65);
    shmid = shmget(sharedMemKey, 4000, 0666 | IPC_CREAT); // create shared
    if (shmid == -1)
    {
        perror("Error in creating shared memory");
        return -1;
    }
    shmId = (int*)shmat(shmid, (void*)0, 0);
    switch (chosenAlgorithm)
    {
    case 1:
        HPF();
        break;
    case 2:
        SRTN();
        break;
    case 3:
        RR(paramter);
        break;
    default:
        break;
    } 
   
    //upon termination release the clock resources.
    
    //  termination of message queue

    printf("Terminate msgQ from Scheduler\n");
    fclose(MemoryLog);
    fclose(schedulerLog);
    msgctl(msgQ, IPC_RMID, (struct msqid_ds*)0);

    destroyClk(true);
}


int startnewProcess(Process p)
{
    p.pid = fork();
    
    if (p.pid == 0)
    {
        system("gcc process.c -o process.out");
        //system("gcc process.c -o process.out");
        execl("./process.out", "process", NULL);
    }

    p.waitingTime = getClk() - p.arrivalTime;
    printf("at time %d process %d started arrive time %d running time %d remning time %d waiting time %d\n", getClk(), p.id, p.arrivalTime, p.runTime, p.remningTime, p.waitingTime);
   // we have to print the privous info to schedulerLog file after we create it ISA (done)
    fprintf(schedulerLog, "at time %d process %d started ---> arrive time %d running time %d remning time %d waiting time %d\n", getClk(), p.id, p.arrivalTime, p.runTime, p.remningTime, p.waitingTime);

    return p.pid;
}
void continueProcess(Process p)
{
    
    kill(p.pid, SIGCONT);
    p.waitingTime += getClk() - p.stopTime;
    printf("at time %d process %d continued arrive time %d running time %d remning time %d waiting time %d\n", getClk(), p.id, p.arrivalTime, p.runTime, p.remningTime, p.waitingTime);
    // we have to print the privous info to schedulerLog file after we create it ISA (done)
    fprintf(schedulerLog, "at time %d process %d continued ---> arrive time %d running time %d remning time %d waiting time %d\n", getClk(), p.id, p.arrivalTime, p.runTime, p.remningTime, p.waitingTime);


}
void stopcurrentProcess(Process p)
{

    kill(p.pid, SIGSTOP);
    p.stopTime = getClk();
    // we have to print the privous info to schedulerLog file after we create it ISA (done)
    fprintf(schedulerLog, "at time %d process %d stoped ---> arrive time %d running time %d remning time %d waiting time %d\n", getClk(), p.id, p.arrivalTime, p.runTime, p.remningTime, p.waitingTime);

}
void finishcurrentProcess(Process p)
{
    // clac waiting , fininsh time
   
    double WTA = (getClk() - p.arrivalTime) * 1.0 / p.runTime;
    p.remningTime = 0;
    *shmId = -1;
    // we have to print the privous info to schedulerLog file after we create it ISA (done)
    fprintf(schedulerLog, "at time %d process %d finished ---> arrive time %d running time %d remning time %d waiting time %d\n", getClk(), p.id, p.arrivalTime, p.runTime, p.remningTime, p.waitingTime);

}
void RR(int quantum)
{
    printf("Schuder: RR ...\n");
    q = (Queue*)malloc(sizeof(Queue));
    queueConstructor(q);
    int cntQuantum = quantum; //conuter for track the quantum of the running Process
    struct Process ProcessinCPU;
    *shmId = 0;
     isRunning = 0; // that flag to check for the status of current process
    int timeWillEndHisQuantum = 0;
    while (1)
    {

        if (isRunning && timeWillEndHisQuantum <= getClk()) //no finish yet(stop it and push it pack to queue)
        {
            printf("Sche : stop process\n");
            ProcessinCPU.remningTime -= quantum;


            if (ProcessinCPU.remningTime > 0)
            {
                queuePush(q, ProcessinCPU);
                stopcurrentProcess(ProcessinCPU);
            }

            else
            {
                ProcessinCPU.remningTime = 0;
                finishcurrentProcess(ProcessinCPU);
            }

            isRunning = 0;
        }
        if (!queueIsEmpty(q) && !isRunning) //pick front of the q
        {
            printf("Sche : pick process\n");
            ProcessinCPU = queuePop(q);
            *shmId = ProcessinCPU.remningTime + 1;
            timeWillEndHisQuantum = min(getClk() + quantum, getClk() + ProcessinCPU.remningTime);
            isRunning = 1;
            //int m=allocate(ProcessinCPU.memSize); 
            
            
              if (ProcessinCPU.remningTime >= ProcessinCPU.runTime) //first time to run
             {
                   
               //int m=allocate(ProcessinCPU.memSize);
                
               
                 
                //   printf("allocated");   
                printf("Sche : start process will stop at %d\n", timeWillEndHisQuantum);
                ProcessinCPU.waitingTime = getClk() - ProcessinCPU.arrivalTime;
                ProcessinCPU.pid = startnewProcess(ProcessinCPU);
               
               
             }
              else // it's contiue  (running time > remning time )
             {
                printf("Sche : continue process\n");
                continueProcess(ProcessinCPU);

             }
           
          
        }

        processRecv.valid = 0; //clear prev recv mess
        processRecv.mtype = 1;
        if (countProcess < numOfProcesses)
        {
            int val = msgrcv(msgQ, &processRecv, 100 * sizeof(processRecv), 0, !IPC_NOWAIT);
            if (val == -1)
            {
                perror("Error in Receiving");
                break;
            }
            else
            {
                if (processRecv.valid == 1)
                {
                    //int m=allocate(processRecv.memSize);

                    //if(m!=-1)
                    //{       printf("mmmmm");    
                         fprintf(MemoryLog, "at time x allocated y bytes for Processs z from i to j\n");
                    countProcess++;
                    processRecv.remningTime = processRecv.runTime;
                    printf("%d %d %d %d RecvTime:%d\n", processRecv.id, processRecv.arrivalTime, processRecv.runTime, processRecv.priority, processRecv.sendTime);
                    queuePush(q, processRecv);
                    printQueue(q);
                    //}
                   // else
                   //{
                    //printf("the is no enough memory \n");       

                   //}
                   
                }
                else
                {
                    printf("Receiving process fake Process \n");//that happen when we don't have any arrived process
                    continue;
                }
            }
        }

        // terminate from the loop
        if (countProcess == numOfProcesses && queueIsEmpty(q) && !isRunning)
            break;
    }
    printf("Schuder: RR had finished...\n");
}
void HPF()
{
    printf("Schuder: hello i started HPF...\n");
    pq = (priorityQueue*)malloc(sizeof(priorityQueue));
     pq1 = (priorityQueue*)malloc(sizeof(priorityQueue));
    priorityQueueConstructor(pq);
    
    Process ProcessinCPU;
    *shmId = -1; // that is for the remning time
     isRunning = 0;
    while (1)
    {

        processRecv.valid = 0; //clear prev recv mess
        processRecv.mtype = 1;
        printf(" num of processses equal %d",numOfProcesses);
        if (countProcess < numOfProcesses)
        {

            printf("HPF : i will receive now\n");
            int val = msgrcv(msgQ, &processRecv, 100 * sizeof(processRecv), 0, !IPC_NOWAIT);
            if (val == -1)
            {
                perror("Error in Receiving");
                break;
            }
            else
            {
                if (processRecv.valid == 1)
                {
                    countProcess++;
                    processRecv.remningTime = processRecv.runTime;
                    printf("%d %d %d %d RecvTime:%d\n", processRecv.id, processRecv.arrivalTime, processRecv.runTime, processRecv.priority, getClk());
                    priorityQueuePush(pq, processRecv, processRecv.priority);
                    // printPriorityQueue(pq);
                }
                else
                {
                    printf("Receiving process fake Process \n");//that happen when we don't have any arrived process
                }
            }
        }
        

        if (*shmId <= 0 && isRunning) //running process is finish(as if *shmId was 0 so the remningTime of that process is 0 )
        {
            printf("HPF :  end current process now");
            finishcurrentProcess(ProcessinCPU);
            isRunning = 0;
        }  

          while(!priorityQueueIsEmpty(pq))
          {
             struct Process front1=priorityQueuePop(pq);
              printf("Check process \n");
                printf("Process memory %d\n",front1.memSize);
              printf("Done Check process \n");
                           
              block_start_address* temp =allocate(front1.memSize);
              
                if(temp==NULL)
                {

                break;
                } 
                else
                {

                    printf("memory allocated from %d to %d \n",temp->start_address,temp->end_address);
                     priorityQueuePush(pq1, front1, front1.priority);
                }

                             
                  
          }
          
         if (!isRunning && !priorityQueueIsEmpty(pq1))
        {
                //struct Process front1=priorityQueueTop(pq1);
                
                
                
                printf("HPF : i will pick process now\n");
                struct Process front = priorityQueuePop(pq1);
                *shmId = front.remningTime + 1;
                ProcessinCPU = front;
                isRunning = 1;
                
                ProcessinCPU.pid = startnewProcess(ProcessinCPU);

               //printf("memory allocated from %d to %d \n",temp->start_address,temp->end_address);        
                
            
        }
       
        if (isRunning)
        {
            ProcessinCPU.remningTime = *shmId;
        }

        // if it last process or the algorithm finish we will out from this loop done
        if (countProcess == numOfProcesses && priorityQueueIsEmpty(pq) && !isRunning)
            break;
    }
    printf("Schuder: HPF had finished...\n");
}
void SRTN()
{
    printf("Schuder: hello i started STRN...\n");
    pq = (priorityQueue*)malloc(sizeof(priorityQueue));
    priorityQueueConstructor(pq);

    Process ProcessinCPU;
    *shmId = -1; // that is for the remaning time
     isRunning = 0;
    while (1)
    {
        processRecv.valid = 0; //clear prev recv mess
        processRecv.mtype = 1;
        if (countProcess < numOfProcesses)
        {
            printf("STRN : i will receive now\n");
            int val = msgrcv(msgQ, &processRecv, 100 * sizeof(processRecv), 0, !IPC_NOWAIT);
            if (val == -1)
            {
                perror("Error in Receiving");
                break;
            }
            else
            {
                if (processRecv.valid == 1)
                {
                    countProcess++;
                    processRecv.remningTime = processRecv.runTime;
                    printf("%d %d %d %d RecvTime:%d\n", processRecv.id, processRecv.arrivalTime, processRecv.runTime, processRecv.priority, getClk());
                    priorityQueuePush(pq, processRecv, processRecv.runTime);
                    // printPriorityQueue(pq);
                }
                else
                {
                    printf("Receiving process fake Process \n");//that happen when we don't have any arrived process
                }
            }
        }

        if (*shmId <= 0 && isRunning) //running process is finish(as if *shmId was 0 so the remningTime of that process is 0 )
        {
            printf("STRN :  end current process now");
            finishcurrentProcess(ProcessinCPU);
            isRunning = 0;
        }

        if (!priorityQueueIsEmpty(pq) && isRunning && *shmId > pq->head->data.remningTime)
        {
            ProcessinCPU.remningTime = *shmId;
            priorityQueuePush(pq, ProcessinCPU, ProcessinCPU.remningTime);
            stopcurrentProcess(ProcessinCPU);
            isRunning = 0;

        }

        if (!isRunning && !priorityQueueIsEmpty(pq))
        {
            if (pq->head->data.remningTime < pq->head->data.runTime)
            {
                //continue
                struct Process p_strn = priorityQueuePop(pq);
                *shmId = p_strn.remningTime+1;
                ProcessinCPU = p_strn;
                isRunning = 1;
                continueProcess(ProcessinCPU);
            }
            else
            {
                //start 
                struct Process p_strn = priorityQueuePop(pq);
                *shmId = p_strn.remningTime+1;
                ProcessinCPU = p_strn;
                isRunning = 1;
                ProcessinCPU.pid = startnewProcess(ProcessinCPU);
            }
        }

        if (isRunning)
        {
            ProcessinCPU.remningTime = *shmId;
        }

        // if it is the last process or the algorithm finish we will get out from this loop done
        if (countProcess == numOfProcesses && priorityQueueIsEmpty(pq) && !isRunning)
            break;
    }
    printf("Schuder: STRN had finished ...\n");
}
void cleaningofsharedmemory(int signum)
{
    printf("Terminate shared memory from scheduler\n");
    shmctl(shmid, IPC_RMID, NULL);
    msgctl(msgQ, IPC_RMID, (struct msqid_ds*)0);
    kill(getpid(), SIGKILL);
}

////////////////////////////

// node1* split_mem(node1 *root)
// {
//     //[512][256][256]
//     root->size = root->size / 2;
//     insertAfter(root, root->size);
//     return root;
// }
// void split_until_suitable(node1 *root, int size)
// {       //    1024
//     //  [512]      //512
//     //[256][256]
//    //????????????????? while (root->size / 2 <= size/255/)
//         //root = split_mem(root);
//                 //////////////////////////////< Al-Rayis Omar >
//    // while (root->size / 2 > size/255/)
//         root = split_mem(root);
//      //////////////////////////////</ Al-Rayis Omar >
// }

// // request access

//  bool alloc( node1* root, int size)
//  {
// //     // root
// //     // traverse
//     node* tmp = root;
//      node*  min = NULL;
// //     // 32 16|16
//      while (tmp != NULL)
//      {
//          if (!tmp->data.allocated && tmp->data.memSize >= size)
//          {
//             if (min == NULL || min->data.memSize >= tmp->data.memSize)
//                  min = tmp;
//          }
//          tmp = tmp->next;
//      }
     
// //     //min is null
// //     if (min == NULL)
// //     {
// //         printf("not enough memory");
// //         return NULL;
// //     }
// //     // we have min
// //         tmp = min;
// //         split_until_suitable(tmp, size);
// //         tmp->data.allocated = true;
// //         return tmp;

//      //////////////////////////////</ Al-Rayis Omar >
//     //????????????????????????????
//     // tmp is null
//     // if (tmp == NULL)
//     // {
//     //     printf("not enough memory");
//     //     return NULL;
//     // }

//     // // we have min

//      if (min != NULL)
//      {
//          tmp = min;
//       //   split_until_suitable(tmp, size);
//         //tmp->used = true;
//         //return tmp;
//      }
//     // else
//     //     return NULL;
// }

void Buddy(int s)
{
     
    // Maximum number of powers of 2 possible
    int n = ceil(log(s) / log(2));
     
    size = n + 1;
 
    // Initially whole block of specified
    // size is available
    block_start_address* totalMemory = malloc(sizeof(block_start_address));
    totalMemory->start_address = 0;
    arr[n] = totalMemory;
}
 
block_start_address* allocate(int s)
{
     
    // Calculate index in free list
    // to search for block if available
    int x = ceil(log(s) / log(2));
     
    // Block available
    if (arr[x] != NULL)
    {
        block_start_address* temp = arr[x];
 
        // Remove block from free list
        arr[x] = arr[x]->next;
        return temp;
    }
    else
    {
        int i;
         
        // If not, search for a larger block
        for(i = x + 1; i < size; i++)
        {
             
            // Find block size greater
            // than request
            if (arr[i] != NULL)
                break;
        }
 
        // If no such block is found
        // i.e., no memory block available
        if (i == size)
        {
            return NULL;
        }
         
        // If found
        else
        {
            block_start_address* temp;
            temp = arr[i];
 
            // Remove first block to split
            // it into halves
            arr[i] = arr[i]->next;
            i--;
             
            for(;i >= x; i--)
            {
                 
                // Divide block into two halves
                block_start_address* pair1, *pair2;
                pair1->start_address = temp->start_address;
                pair2->start_address = temp->start_address + pow(2, i);
                temp->end_address=temp->start_address + pow(2, i);


                block_start_address* traversalTemp = arr[i];
                if(traversalTemp == NULL){
                    arr[i] = pair1;
                } else{
                    while (traversalTemp->next) {
                        traversalTemp = traversalTemp->next;
                    }
                    traversalTemp->next = pair1;
                    pair1->next = pair2;
                }
                
                temp = arr[i];
 
                // Remove first free block to
                // further split
                arr[i] = arr[i]->next;
            }
            return temp;
        }
    }
}
 
void deallocate(int id, int endMemoryAddress)
{
     
    // Size of block to be searched
    int n = ceil(log(endMemoryAddress / log(2)));
     
    int i, buddyNumber, buddyAddress;
 
    // Add the block in free list
    block_start_address* newFreeMemory = malloc(sizeof(block_start_address));
    newFreeMemory->start_address = id;
    arr[n] = newFreeMemory;
 
    // Calculate buddy number
    buddyNumber = id / endMemoryAddress;
 
    if (buddyNumber % 2 != 0)
        buddyAddress = id - pow(2, n);
    else
        buddyAddress = id + pow(2, n);
         
    // Search in free list to find it's buddy
    block_start_address* element = arr[n];
    while (element)
    {
        // If buddy found and is also free
        if (element->start_address == buddyAddress)
        {
             
            // Now merge the buddies to make
            // them one large free memory block
            if (buddyNumber % 2 == 0)
            {
                block_start_address* newFreeMemory = malloc(sizeof(block_start_address));
                newFreeMemory->start_address = id;
                newFreeMemory->next = arr[n+1];
                arr[n + 1] = newFreeMemory;
            }
            else
            {
                block_start_address* newFreeMemory = malloc(sizeof(block_start_address));
                newFreeMemory->start_address = buddyAddress;
                newFreeMemory->next = arr[n+1];
                arr[n + 1] = newFreeMemory;
            }

            block_start_address* elementRemoveI = arr[i];
            while (elementRemoveI->next != element)
            {
                elementRemoveI = elementRemoveI->next;
            }
            elementRemoveI->next = elementRemoveI->next->next;

            elementRemoveI = arr[i];
            while (elementRemoveI->next != NULL)
            {
                elementRemoveI = elementRemoveI->next;
            }
            elementRemoveI->next = elementRemoveI->next->next;
            break;
        }
    }
}