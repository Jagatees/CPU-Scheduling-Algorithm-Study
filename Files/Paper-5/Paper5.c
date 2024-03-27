// Paper 5: Adaptive Round Robin Scheduling using Shortest Burst Approach Based on Smart Time Slice
// Tan Yan An 2301908
// Lab P2 Team 3

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float getTimeSlice(int numberOfProcesses, int remainingBurstTime[], int arrivalTimeArray[], int currentTime){
    float totalBurstTime = 0.0;
    int numberOfProcessesInQueue = 0;
    int tempList[numberOfProcesses];
    int tempListCounter = 0;
    float timeSlice = 0.0;

    // To check how many processes are in the queue based on the arrival time and remaining burst time
    for (int i = 0; i < numberOfProcesses; i++){
        if (arrivalTimeArray[i] <= currentTime && remainingBurstTime[i] > 0){  // processes are still in queue if it has arrived and has remaining burst time
            totalBurstTime += remainingBurstTime[i];
            tempList[tempListCounter] = remainingBurstTime[i];
            numberOfProcessesInQueue++;
            tempListCounter++;
        }
    }
    if (numberOfProcessesInQueue % 2 == 0){  // if the number of processes in the queue is even, use the mean remaining burst time as the time slice
        timeSlice = ceil(totalBurstTime / numberOfProcessesInQueue); // round up the time slice
    }
    else{
        int median = numberOfProcessesInQueue / 2; // if the number of processes in the queue is odd, use the median remaining burst time as the time slice
        timeSlice = tempList[median];

    }

    return timeSlice;
}


void executeRoundRobin(int numberOfProcesses, int burstTimeArray[], int arrivalTimeArray[], int processNumberArray[], int waitingTime[], int turnaroundTime[], int responseTime[], int *contextSwitch){
    int remainingBurstTime[numberOfProcesses];

    // copy burst time array to remaining burst time array, to preserve original burst time array values
    for (int i = 0; i < numberOfProcesses; i++){
        remainingBurstTime[i] = burstTimeArray[i];
        //initialize response time to -1, reason being that if the response time is still -1, it means the process has not been executed. 
        //This prevents it from counting twice if response time was 0.
        responseTime[i] = -1; 
    }

    int currentTime = 0;
    int startCycleTime = 0;
    int numberOfCompletedProcesses = 0;
    int prevProcess = -1;

    
    while(numberOfCompletedProcesses < numberOfProcesses){
        int done = 1; //initialize done to 1, meaning all processes are done or no processes are in the queue

        float smartTimeSlice = getTimeSlice(numberOfProcesses, remainingBurstTime, arrivalTimeArray, currentTime); // recalculate smart time slice every cycle

        for (int i = 0; i < numberOfProcesses; i++){
            // if the process has arrived and has remaining burst time, execute the process
            // note that arrival time of process are compared against startCycleTime to ensure the time slice is used for one full cycle
            // this is because the time slice is calculated based on the remaining burst time of the processes in the queue at startCycleTime, if the process arrives after the startCycleTime, 
            // it will be executed in the next cycle
            // instead of using the time slice for the current cycle

            if (arrivalTimeArray[i] <= startCycleTime && remainingBurstTime[i] > 0 ){ 
                *contextSwitch +=1;
                
                done = 0;
                if (responseTime[i] == -1){ // if the response time is still -1, set the response time to the current time - arrival time
                    responseTime[i] = currentTime - arrivalTimeArray[i];
                }
                if (remainingBurstTime[i] > smartTimeSlice){ // if the remaining burst time of the process is more than the time slices, execute the process for the time slice
                    currentTime += smartTimeSlice; // increase the current time by the time slices
                    remainingBurstTime[i] -= smartTimeSlice;
                }
                else{ // if the remaining burst time of the process is less than or equal to the time slices, execute the process for the remaining burst time
                    
                    currentTime += remainingBurstTime[i]; // increase the current time by the remaining burst time
                    waitingTime[i] = currentTime - arrivalTimeArray[i] - burstTimeArray[i]; // waiting time is the current time of completion - the burst time - the arrival time
                    turnaroundTime[i] = currentTime - arrivalTimeArray[i]; // turnaround time is the time of completion of hte process - the arrival time
                    remainingBurstTime[i] = 0;
                    numberOfCompletedProcesses++;
                }
            }
        }
        
        if (done == 1){ // if no process to execute, increase the current time by 1, this is to handle the case where there are no processes in the queue at time 0 etc.
            currentTime++;
            
        }
        startCycleTime = currentTime; // set the start cycle time to the current time
    }

}

void swap(int *smallerValue, int *largerValue) {
    int temp = *smallerValue; //initialize temporary variable to pointer of smallerValue
    *smallerValue = *largerValue; //pointer of smallerValue = pointer of largerValue
    *largerValue = temp; //pointer of largerValue = temporary variable
}

void sortByBurstTime(int numberOfProcesses, int burstTimeArray[], int arrivalTimeArray[], int processNumberArray[]){
    for (int i = 0; i < numberOfProcesses - 1; i++) {
        for (int j = 0; j < numberOfProcesses - i - 1; j++) {
            if (burstTimeArray[j] > burstTimeArray[j + 1]) { //if the burst time of target process is larger than the burst time of the next process
                swap(&arrivalTimeArray[j], &arrivalTimeArray[j + 1]); //swap the arrival time of the processes
                swap(&burstTimeArray[j], &burstTimeArray[j + 1]); //swap the burst time of the processes
                swap(&processNumberArray[j], &processNumberArray[j + 1]); //swap the process number positions
            } 
        }
    }
    
}

void showOutput(int numberOfProcesses, int burstTimeArray[], int arrivalTimeArray[], int processNumberArray[], int waitingTime[], int turnaroundTime[], int responseTime[], int contextSwitch){
    float totalWaitingTime = 0;
    float totalTurnaboutTime = 0;
    float totalResponseTime = 0;

    printf("\n\nProcess\t\tBurst Time\tArrival Time\tWaiting Time\tTurnaround Time\tResponse Time\n");
    for (int i = 0; i < numberOfProcesses; i++){
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processNumberArray[i], burstTimeArray[i], arrivalTimeArray[i], waitingTime[i], turnaroundTime[i], responseTime[i]);
    }
    
    // calculate total waiting time, total turnaround time and total response time
    for (int i = 0; i < numberOfProcesses; i++){
        totalWaitingTime += waitingTime[i];
        totalTurnaboutTime += turnaroundTime[i];
        totalResponseTime += responseTime[i];
    }

    printf("\nThe Average Waiting time: %.2f", totalWaitingTime/numberOfProcesses);
    printf("\nThe Average Turnaround time: %.2f", totalTurnaboutTime/numberOfProcesses);
    printf("\nThe Average Response time: %.2f", totalResponseTime/numberOfProcesses);
    printf("\nTotal Context Switch: %d\n", contextSwitch);
}   

int main() {

    int numberOfProcesses;
    printf("Enter the number of processes: "); // User input for number of processes
    scanf("%d", &numberOfProcesses);

    // Check for zero input and prompt for re-entry
    while (numberOfProcesses <= 0) {
        printf("Number of processes must be greater than 0. Please re-enter: ");
        scanf("%d", &numberOfProcesses);
    }

    // Arrays and variables declaration
    int burstTimeArray[numberOfProcesses];
    int arrivalTimeArray[numberOfProcesses];
    int processNumberArray[numberOfProcesses];
    int waitingTime[numberOfProcesses];
    int turnaroundTime[numberOfProcesses];
    int responseTime[numberOfProcesses];
    int contextSwitch = -1;

    // User inputs for burst times and arrival times for each process
    for (int i = 0; i < numberOfProcesses; i++) {
        printf("\nEnter Burst Time for process %d: ", i + 1);
        scanf("%d", &burstTimeArray[i]);

        // Check for zero input in burst time and prompt for re-entry
        while (burstTimeArray[i] <= 0) {
            printf("Burst Time must be greater than 0. Please re-enter Burst Time for process %d: ", i + 1);
            scanf("%d", &burstTimeArray[i]);
        }

        processNumberArray[i] = i + 1; // Process number array initialization
        printf("Enter Arrival Time for process %d: ", i + 1);
        scanf("%d", &arrivalTimeArray[i]);
    }

    // Assuming the existence of these functions
    sortByBurstTime(numberOfProcesses, burstTimeArray, arrivalTimeArray, processNumberArray); 
    executeRoundRobin(numberOfProcesses, burstTimeArray, arrivalTimeArray, processNumberArray, waitingTime, turnaroundTime, responseTime, &contextSwitch);
    showOutput(numberOfProcesses, burstTimeArray, arrivalTimeArray, processNumberArray, waitingTime, turnaroundTime, responseTime, contextSwitch);

    return 0;
}
