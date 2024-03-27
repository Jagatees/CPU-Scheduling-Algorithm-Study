// Paper 1: Improved Round Robin CPU Scheduling Algorithm using the combination of Priority, Shortest Job First and Round Robin algorithm
// Chua Shing Ying 2301932
// Lab P2 Team 3

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

typedef struct {
    int id; // process id
    int ct; // current time
} gantChart;

int compare(const void *a, const void *b)
{
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;

    // Sort by arrival time
    if (p1->at != p2->at)
        return p1->at - p2->at;

    // If arrival times are the same, sort by burst time < 20% of time quantum
    if (p1->bt < 0.2 * p1->tq && p2->bt >= 0.2 * p2->tq)
        return -1;
    if (p1->bt >= 0.2 * p1->tq && p2->bt < 0.2 * p2->tq)
        return 1;

    // If both burst times are less than 20% of the time quantum or neither is, sort by process ID
    return p1->id - p2->id;
}

void roundRobin(Process processes[], int n)
{
    int currentTime = 0;
    int contextSwitches = 0;

    gantChart gantchart[MAX_PROCESS];

    // Sort the current array by arrival time and burst time
    qsort(processes, n, sizeof(Process), compare);

    // Traverse the queue until all processes is completed
    while(1)
    {
        // Assume all processes are done 
        bool done = true;

        for (int i = 0; i < n; i++)
        {
            // Check if the process is not completed and has arrived
            if (processes[i].flag == false && processes[i].at <= currentTime)
            {
                // There is a process that is not done
                done = false;
                
                // Check if remaining time is lesser or equal to time quantum
                if (processes[i].rt <= processes[i].tq) 
                {
                    processes[i].flag = true;
                    
                    currentTime += processes[i].rt;

                    // Calculate the individual waiting time and turnaround time
                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    
                    processes[i].rt = 0;

                } 
                else if (processes[i].rt > processes[i].tq && processes[i].rt <= (processes[i].tq + (30 * processes[i].tq/100)) && processes[i].p == 3) 
                {
                    
                    processes[i].flag = true;
                    
                    currentTime += processes[i].rt;

                    // Calculate the individual waiting time and turnaround time
                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;

                    processes[i].rt = 0;
                } 
                else if (processes[i].rt > processes[i].tq && processes[i].rt <= (processes[i].tq + (20 * processes[i].tq/100)) && (processes[i].p == 2 || processes[i].p == 1)) 
                {
                    
                    processes[i].flag = true;

                    currentTime += processes[i].rt;

                    // Calculate the individual waiting time and turnaround time
                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    
                    processes[i].rt = 0;
                } 
                else 
                {
                    // Calculate the individual waiting time and turnaround time
                    processes[i].wt = currentTime - processes[i].bt - processes[i].at;
                    processes[i].tat = currentTime - processes[i].at;
                    processes[i].rt -= processes[i].tq;
                    currentTime += processes[i].tq;
                }

                gantchart[contextSwitches].id =  processes[i].id;
                gantchart[contextSwitches].ct =  currentTime;
                
                // Record each context switch
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
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].bt, 
        processes[i].at, processes[i].p, processes[i].wt, processes[i].tat, processes[i].tq);
    }

    // Display average waiting time and turnaround time
    printf("\nAverage Waiting Time: %.2f", avgWT);
    printf("\nAverage Turnaround Time: %.2f", avgTat);
    printf("\n"); 

    // Display number of context switches
    printf("\nNumber of context switches: %d", contextSwitches-1);
    printf("\n"); 
    printf("\n"); 

    // Show the gant chart of the process 
    for (int i = 0; i < contextSwitches; i++)
    {
        printf("| %d (%d)| ", gantchart[i].id, gantchart[i].ct);
    }

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
        // High priority
        if (processes[i].p == 3)
        {
            processes[i].tq = timeQuantum + (20 * timeQuantum/100);
        }
        // Medium priority
        else if (processes[i].p == 2)
        {
            processes[i].tq = timeQuantum;
        }
        // Low priority 
        else if (processes[i].p == 1)
        {
            processes[i].tq = timeQuantum - (20 * timeQuantum/100);
        }
    }

    // Start the algorithm
    roundRobin(processes, noOfProcesses);

    return 0;
}
