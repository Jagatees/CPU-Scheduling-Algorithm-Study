// Paper 1
// Chua Shing Ying 2301932

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define MAX_PROCESS 100

typedef struct {
    int id; // process id
    int bt; // burst time
    int at; // arrival time
    int wt; // waiting time
    int tat; // turnaround time
    int rt; // remaining burst time
    int p; // priority
    int tq; // time quantum
    bool flag; // flag
} Process;

void roundRobin(Process processes[], int n)
{
    int currentTime = 0;
    int contextSwitches = 0;

    int gantChart[MAX_PROCESS][2];

    // Sort the queue by arrival time 
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (processes[j].at > processes[j+1].at) {

                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }

    // Traverse the queue until all processes is completed
    while(1)
    {
        bool done = true;

        for (int i = 0; i < n; i++)
        {
            // Check if the process is not completed and has arrived
            if (processes[i].flag == false && processes[i].at <= currentTime)
            {
                done = false;

                // Calculate the individual waiting time and turnaround time
                if (processes[i].rt <= processes[i].tq) 
                {
                    processes[i].flag = true;

                    currentTime += processes[i].rt;

                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    
                    processes[i].rt = 0;
                } 
                else if (processes[i].rt > processes[i].tq && processes[i].rt <= (processes[i].tq + (30 * processes[i].tq/100)) && processes[i].p == 3) 
                {
                    
                    processes[i].flag = true;
                    
                    currentTime += processes[i].rt;

                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    processes[i].rt = 0;
                } 
                else if (processes[i].rt > processes[i].tq && processes[i].rt <= (processes[i].tq + (20 * processes[i].tq/100)) && (processes[i].p == 2 || processes[i].p == 1)) 
                {
                    
                    processes[i].flag = true;

                    currentTime += processes[i].rt;
                    
                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    
                    processes[i].rt = 0;
                } 
                else 
                {
                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    processes[i].rt -= processes[i].tq;
                    currentTime += processes[i].tq;
                }

                contextSwitches++;
            }
        }

        if (done)
        {
            break;
        }
    }

    // Sort the array by process id 
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (processes[j].id > processes[j+1].id) {

                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }

    // Calculate total waiting time and turnaround time
    double totalWT = 0.0;
    double totalTat = 0.0; 

    for (int i = 0; i < n; i++) {
        totalWT += processes[i].wt;
        totalTat += processes[i].tat;
    }

    // Calculate averages
    double avgWT = totalWT/n;
    double avgTat = totalTat/n;

    // Display the results
    printf("\nProcess\tBurst Time\tArrival Time\tPriority\tWaiting Time\tTurnaround Time\tTime Quantum\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].bt, processes[i].at, processes[i].p, processes[i].wt, processes[i].tat, processes[i].tq);
    }

    printf("\nAverage Waiting Time: %.2f", avgWT);
    printf("\nAverage Turnaround Time: %.2f", avgTat);
    printf("\n"); // New line

    printf("\nNumber of context switches: %d", contextSwitches-1);
    printf("\n"); // New line
}

int main() {
    int noOfProcesses, timeQuantum;

    // Enter the number of processes
    printf("\nEnter the number of processes: ");
    scanf("%d", &noOfProcesses);

    Process processes[MAX_PROCESS];

    // Enter burst time, arrival time, and priority for each process
    for (int i = 0; i < noOfProcesses; i++) 
    {
        printf("\nEnter Burst Time, Arrival Time and Priority for process %d: ", i + 1);
        scanf("%d %d %d", &processes[i].bt, &processes[i].at, &processes[i].p);

        // Set process id
        processes[i].id = i + 1;

        // Set process flag
        processes[i].flag = false;

        // Set remaining time initially equal to burst time
        processes[i].rt = processes[i].bt;
    }

    // Enter the time quantum
    printf("\nEnter Time Quantum: ");
    scanf("%d", &timeQuantum);

    // Assign the appropriate time quantum based on priority
    for (int i = 0; i < noOfProcesses; i++) 
    {
        if (processes[i].p == 3)
        {
            processes[i].tq = timeQuantum + (20 * timeQuantum/100);
        }
        else if (processes[i].p == 2)
        {
            processes[i].tq = timeQuantum;
        }
        else if (processes[i].p == 1)
        {
            processes[i].tq = timeQuantum - (20 * timeQuantum/100);
        }
    }

    roundRobin(processes, noOfProcesses);

    return 0;
}
