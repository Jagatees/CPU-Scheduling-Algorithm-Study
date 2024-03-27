// Paper 4: An Improved Round Robin Algorithm for an Efficient CPU Scheduling
// Chin Hui Min 2301941
// Lab P2 Team 3

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    int AT; // arrival time
    float FBT; // first burst time (user input)
    float BT; // burst time (changes after rounds)
    float TQ; // time quantum
    int TAT; // Turnaround Time
    int status; // when (status == 1) = task is completed, when (status == 0) = task is not completed 
    int exitTime; // time when task finishes
} Task;

// stores all tasks
Task* allQueue = NULL; 
int allSize = 0;

// stores ready tasks
Task* readyQueue = NULL; 
int queueSize = 0;

// for printing
Task* finishQueue = NULL; 
int finishSize = 0;

int contextSwitch = -1;

// round down BT for QT calculation (following XSITE example)
double roundDown(double num) {
    if (num >= 0.0) {
        return (double) ((int)(num + 0.5));
    } else {
        return (double) ((int)(num - 0.5));
    }
}
// add task to queue for execution
void addAllTask(Task task) {
    allQueue = realloc(allQueue, sizeof(Task) * (allSize + 1));
    allQueue[allSize] = task;
    allSize++;
}

// add task to queue for execution
void addTask(Task task) {
    readyQueue = realloc(readyQueue, sizeof(Task) * (queueSize + 1));
    readyQueue[queueSize] = task;
    queueSize++;
}

// add task to finished queue
void addFinishedTask(Task task) {
    finishQueue = realloc(finishQueue, sizeof(Task) * (finishSize + 1));
    finishQueue[finishSize] = task;
    finishSize++;
}

// remove task from queue
void removeTask(Task taskRemove) {
    // if no more task
    if (queueSize == 0) {
        return;
    }

    int index = -1;
    for (int i = 0; i < queueSize; i++) {
        if (readyQueue[i].id == taskRemove.id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Task with id %d not found in the queue\n", taskRemove.id);
        return;
    }

    for (int i = index; i < queueSize - 1; i++) {
        readyQueue[i] = readyQueue[i + 1];
    }

    queueSize--;
    readyQueue = realloc(readyQueue, sizeof(Task) * queueSize);
}

// Step 1: compare BT
int compareTasks(const void* a, const void* b) {
    Task* taskA = (Task*)a;
    Task* taskB = (Task*)b;
    return taskA->BT - taskB->BT;
}

// Step 1: sort based on shortest BT (SJF)
void sortReadyQueue() {
    qsort(readyQueue, queueSize, sizeof(Task), compareTasks);
}

// Step 2: calculate mean
float calcMean() {
    // if queue empty
    if (queueSize == 0) {
        return 0.0;
    }

    float totalBT = 0.0;
    for (int i = 0; i < queueSize; i++) {
        totalBT += readyQueue[i].BT;
    }

    return totalBT / queueSize;
}

// Step 3: calculate TQij
void calcTQij(Task* task, float mean) {
    if (task->BT != 0) {
        task->TQ = (mean / 2.0) + ((mean / 2.0) / task->BT); // paper 4's formula
        printf("Calculated Time Quantum: %f\n", task->TQ);
    }
}

void executeTasks() {
    int i = 0;
    int taskCounter = 0;
    int roundCount = 1;
    int lastTaskId = -1;
    int initialQueueSize = queueSize;
    float mean = calcMean();
    int taskChangedStatus = 0;

    float time = 0;

    for (int i = 0; i < queueSize; i++) {
        readyQueue[i].FBT = 0.0;
    }

    // based on paper 4's flow chart
    while (queueSize > 0) {
        if (readyQueue[i].FBT == 0.0) {
            readyQueue[i].FBT = readyQueue[i].BT;
        }
        contextSwitch++;
        calcTQij(&readyQueue[i], mean);
        // check for tasks whose arrival time is <= time

        printf("\n----- ROUND %d -----\n", roundCount);

        // if BT is larger than TQ
        if (readyQueue[i].BT > readyQueue[i].TQ) {
            printf("Task ID %d, Time Quantum: %f\n", readyQueue[i].id, readyQueue[i].TQ);
            time += readyQueue[i].TQ;
            readyQueue[i].BT -= readyQueue[i].TQ;

            // after subtracting BT, if BT is less than TQ, task is completed
            if (readyQueue[i].BT <= readyQueue[i].TQ) {
                printf("(Completed) Burst Time: %f\n", readyQueue[i].BT);
                time += readyQueue[i].BT;

                readyQueue[i].BT = 0;
                readyQueue[i].status = 1;
                
                readyQueue[i].exitTime = roundDown(time);
                readyQueue[i].TAT = readyQueue[i].exitTime - readyQueue[i].AT;

                // add finished task to finished queue and remove from ready queue
                addFinishedTask(readyQueue[i]);
                removeTask(readyQueue[i]);
                taskChangedStatus = 1; 

            } else {
                // task did not complete, store task in temp, remove task from queue, add task to the back of queue
                Task temp = readyQueue[i];
                removeTask(readyQueue[i]); 
                addTask(temp); 
            }
        } else {
            // TQ is larger than BT, task is completed
            printf("Task ID %d, Time Quantum: %f\n", readyQueue[i].id, readyQueue[i].TQ);
            printf("(Completed) Burst Time: %f\n", readyQueue[i].BT);
            time += readyQueue[i].BT;

            readyQueue[i].BT = 0;
            readyQueue[i].status = 1;

            readyQueue[i].exitTime = roundDown(time);
            readyQueue[i].TAT = readyQueue[i].exitTime - readyQueue[i].AT;
            
            // add finished task to finished queue and remove from ready queue
            addFinishedTask(readyQueue[i]);
            removeTask(readyQueue[i]);
            taskChangedStatus = 1;
        }
        
        taskCounter++;

        // if all tasks have been executed in the round
        if (taskCounter == initialQueueSize) {
            printf("Completed Round %d\n\n", roundCount);
            roundCount++;
            taskCounter = 0;
            initialQueueSize = queueSize;

            // check for tasks whose arrival time is <= time
            for (int i = 0; i < allSize; i++) {
                if (allQueue[i].AT <= time && allQueue[i].status == 0) {
                    addTask(allQueue[i]);
                    allQueue[i].status = 2; // Mark as in ready queue
                    taskChangedStatus = 1;
                }
            }
            if (taskChangedStatus) { // If a task has ended or a new task has been inserted, recompute the mean
                mean = calcMean();
                sortReadyQueue();
                taskChangedStatus = 0; 
            }
        }
    }
}

// Step 5: When a new task enters
void handleNewTask(Task newTask, float mean) {
    addTask(newTask);
    sortReadyQueue();
    executeTasks(mean);
}

// SJF sort
void printReadyQueue() {
    printf("\nReady Queue (Sorted by BT): \n");
    for (int i = 0; i < queueSize; i++) {
        printf("Task ID: %d, Burst Time: %f\n", readyQueue[i].id, readyQueue[i].BT);
    }
    printf("\n");
}

int main() {
    // get user inputs
    int numOfTasks;
    printf("No. of tasks: ");
    scanf("%d", &numOfTasks);

    for (int i = 0; i < numOfTasks; i++) {
        Task task;
        printf("\nTask %d Arrival Time: ", i + 1);
        scanf("%d", &task.AT);
        printf("Task %d Burst Time: ", i + 1);
        scanf("%f", &task.BT);
        task.id = i + 1;
        task.status = 0;
        task.TAT = 0;
        task.FBT = 0.0;
        task.TQ = 0;
        task.exitTime = 0;

        // add task to all task queue
        addAllTask(task);
    }
    
    // loop through all task queue and add tasks with AT = 0 to ready queue
    for (int i = 0; i < allSize; i++) {
        if (allQueue[i].AT <= 0) {
            addTask(allQueue[i]);
            allQueue[i].status = 2;
        }
    }
    
    sortReadyQueue();
    printReadyQueue();
    executeTasks();

    printf("\nProcess\t\tBurst Time\tArrival Time\tWaiting Time\tTurnaround Time\n");
    
    float totalTAT = 0.0;
    float totalWT = 0.0;

    // print finished queue
    for (int i = 0; i < finishSize; i++) {
        printf("P%d\t\t%d\t\t%d\t\t%d\t\t%d\n", finishQueue[i].id, (int) finishQueue[i].FBT, finishQueue[i].AT, (int) (finishQueue[i].TAT - finishQueue[i].FBT), finishQueue[i].TAT);

        totalTAT += finishQueue[i].TAT;
        totalWT += (finishQueue[i].TAT - finishQueue[i].FBT);
    }

    // print context switches, ATAT and AWT
    printf("\nContext Switches: %d\n", contextSwitch);
    printf("Average Turnaround Time: %.2f\n", (float)totalTAT / finishSize);
    printf("Average Waiting Time: %.2f\n", (float)totalWT / finishSize);

    return 0;
}
