// #include<bits/stdc++.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
 
// Size of vector of pairs
int size;
typedef struct block_start_address
{
    int start_address;
    struct block_start_address* next;
} block_start_address;

// Global vector of pairs to track all
// the free nodes of various sizes
block_start_address* arr[100];
 
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
 
int allocate(int s)
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
        return temp->start_address;
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
            return -1;
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
            return temp->start_address;
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
 
// Driver code
int main()
{
     
    Buddy(128);
    allocate(16);
    allocate(16);
    allocate(16);
    allocate(16);
    deallocate(0, 15);
    // deallocate(9);
    deallocate(32, 47);
    deallocate(16, 31);
 
    return 0;
}