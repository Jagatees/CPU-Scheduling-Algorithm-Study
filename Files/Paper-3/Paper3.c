// Paper 3: Improved Round Robin Scheduling Algorithm WIth Varying Time Quantum
// See Xue Kai 2301818
// Lab P2

#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 20

// Structure to represent a process
struct Process {
    int id;                 // Process ID
    int burst_time;         // Burst time required by the process for execution
    int arrival_time;       // Arrival time of the process in the scheduling queue
    int remaining_time;     // Remaining time for the process to complete execution
    int waiting_time;       // Waiting time of the process in the scheduling queue
    int turnaround_time;    // Turnaround time of the process (waiting time + burst time)
};

// Global variables
int current_time = 0;               // Current time during execution
int processes_executed = 0;         // Number of processes executed
int context_switch = -1;            // Counter for context switches during execution

// Function to calculate waiting time for each process
void calculateWaitingTime(struct Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
    }
}

// Function to calculate turnaround time for each process
void calculateTurnaroundTime(struct Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
    }
}

// Function to calculate and print average waiting and turnaround times
void calculateAverageTimes(struct Process processes[], int n) {
    float total_waiting_time = 0, total_turnaround_time = 0;
    printf("\nProcess\tBurst Time\tArrival Time\tWaiting Time\tTurnaround Time\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].burst_time,
               processes[i].arrival_time, processes[i].waiting_time, processes[i].turnaround_time);
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }
    printf("Average Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
    printf("Context Switch: %d\n", context_switch);
}

int main() {
    int n;
    printf("Enter the number of processes (up to %d): ", MAX_PROCESSES);
    scanf("%d", &n);

    if (n > MAX_PROCESSES) {
        printf("Error: Maximum number of processes exceeded.\n");
        return 1;
    }

    struct Process processes[MAX_PROCESSES];   // Array to store processes

    // Input burst time and arrival time for each process
    printf("Enter burst time and arrival time for each process:\n");
    for (int i = 0; i < n; i++) {
        printf("Process %d:\n", i + 1);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burst_time);
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrival_time);
        processes[i].id = i + 1;
        processes[i].waiting_time = 0;
        processes[i].remaining_time = processes[i].burst_time;
    }

    // Sort processes based on arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time == processes[j + 1].arrival_time) {
                if (processes[j].burst_time > processes[j + 1].burst_time) {
                    struct Process temp = processes[j];
                    processes[j] = processes[j + 1];
                    processes[j + 1] = temp;
                }
            }
            else if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }   
    }

    // Main scheduling algorithm begins here
    while (1) {
        struct Process light_task_queue[MAX_PROCESSES];   // Queue for light tasks
        struct Process heavy_task_queue[MAX_PROCESSES];   // Queue for heavy tasks
        int light_task_count = 0;       // Count of light tasks
        int heavy_task_count = 0;       // Count of heavy tasks
        int total_burst_time = 0;       // Total burst time of processes in the scheduling queue
        float median_burst_time = 0;    // Median burst time of processes in the scheduling queue
        float median_id = 0;            // Median ID of processes
        int trials = 0;                 // Number of trials for finding median burst time

        // Sorting tasks into light and heavy queues based on burst time & arrival time
        for (int i = 0 + processes_executed; i < n; i++) {
            if (processes[i].arrival_time <= current_time) {
                for (int j = i + 1; j < n; j++) {
                    if (processes[j].arrival_time <= current_time && processes[j].burst_time < processes[i].burst_time) {
                        // Swap the processes
                        struct Process temp = processes[i];
                        processes[i] = processes[j];
                        processes[j] = temp;
                    }
                }
                total_burst_time += processes[i].burst_time;
                trials += 1;
            }
        }
        if (trials % 2 == 0) {
            median_burst_time = (processes[(trials / 2) + processes_executed].burst_time + processes[(trials / 2) + 
            processes_executed-1].burst_time) / 2;
            median_id = trials / 2 + processes_executed - 1;
        } else {
            median_burst_time = processes[(trials / 2) + processes_executed].burst_time;
            median_id = trials / 2 + processes_executed;
        }

        // Distributing tasks into light and heavy queues based on median burst time
        for (int i = 0 + processes_executed; i < n; i++) {
            if (processes[i].arrival_time <= current_time) {
                if (processes[i].burst_time <= median_burst_time && i <= median_id) {
                    light_task_queue[light_task_count++] = processes[i];
                } else {
                    heavy_task_queue[heavy_task_count++] = processes[i];
                }
            }
        }

        // Start of Task execution
        int light_counter = 0;
        int heavy_counter = 0;
        float light_task_median = 0;
        float heavy_task_median = 0;
        int light_settle = 0;
        int heavy_settle = 0;
        int done = 0;



        // Light task execution algorithm
        while (light_settle == 0) {
            // Calculating median burst time for light tasks
            if (light_task_count > 0) {
                if (light_task_count - light_counter == 1) {
                    light_task_median = light_task_queue[light_counter].remaining_time;
                }
                else if (light_task_count % 2 == 0) {
                    light_task_median = ((light_task_queue[((light_task_count-light_counter) / 2) + 
                    light_counter].remaining_time) + (light_task_queue[((light_task_count-light_counter) / 2) + 
                    light_counter - 1].remaining_time)) / 2;
                } else {
                    light_task_median = light_task_queue[((light_task_count-light_counter) / 2) + 
                    light_counter].remaining_time;
                }
            }

            float current_quantum = light_task_median;   // Current quantum for execution

            // Execution loop for light tasks
            while (light_counter < light_task_count) {

                if (light_task_queue[light_counter].remaining_time > light_task_median) {
                    current_time += light_task_median;
                    light_task_queue[light_counter].remaining_time -= light_task_median;
                    light_counter += 1;
                    context_switch += 1;    // Increment context switch count
                } else {
                    current_time += light_task_queue[light_counter].remaining_time;
                    light_task_queue[light_counter].waiting_time = current_time - light_task_queue[light_counter].burst_time;
                    processes[processes_executed].waiting_time = light_task_queue[light_counter].waiting_time - 
                    processes[processes_executed].arrival_time;
                    light_task_queue[light_counter].remaining_time = 0;
                    light_counter += 1;
                    processes_executed += 1;
                    context_switch += 1;    // Increment context switch count
                }
                
            }

            int light_leftover = 0;
            light_settle = 1;

            // Checking if all light tasks are completed
            for (int i=0; i < light_task_count; i++) {
                light_leftover += light_task_queue[i].remaining_time;
                if (light_task_queue[i].remaining_time > 0) {
                    light_counter -= 1;
                }
            }

            // Determining completion status of light tasks
            if (light_counter == light_task_count && light_leftover == 0) {
                done += 1;
            } else {
                light_settle = 0;
            }
        }



        // Heavy Task Execution Algorithm
        while (heavy_settle == 0) {
            // Calculating median burst time for heavy tasks
            if (heavy_task_count > 0) {
                if (heavy_task_count - heavy_counter == 1) {
                    heavy_task_median = heavy_task_queue[heavy_counter].remaining_time;
                }
                else if (heavy_task_count % 2 == 0) {
                    heavy_task_median = ((heavy_task_queue[((heavy_task_count-heavy_counter) / 2) + 
                    heavy_counter].remaining_time) + (heavy_task_queue[((heavy_task_count-heavy_counter) / 2) + 
                    heavy_counter - 1].remaining_time)) / 2;
                } else {
                    heavy_task_median = heavy_task_queue[((heavy_task_count-heavy_counter) / 2) + 
                    heavy_counter].remaining_time;
                }
            }

            float current_quantum = heavy_task_median;   // Current quantum for execution

            // Execution loop for heavy tasks
            while (heavy_counter < heavy_task_count) {

                if (heavy_task_queue[heavy_counter].remaining_time > heavy_task_median) {
                    current_time += heavy_task_median;
                    heavy_task_queue[heavy_counter].remaining_time -= heavy_task_median;
                    heavy_counter += 1;
                    context_switch += 1;    // Increment context switch count
                } else {
                    current_time += heavy_task_queue[heavy_counter].remaining_time;
                    heavy_task_queue[heavy_counter].waiting_time = current_time - heavy_task_queue[heavy_counter].burst_time;
                    processes[processes_executed].waiting_time = heavy_task_queue[heavy_counter].waiting_time - 
                    processes[processes_executed].arrival_time;
                    heavy_task_queue[heavy_counter].remaining_time = 0;
                    heavy_counter += 1;
                    processes_executed += 1;
                    context_switch += 1;    // Increment context switch count
                }
                
            }

            int heavy_leftover = 0;
            heavy_settle = 1;

            // Checking if all heavy tasks are completed
            for (int i=0; i < heavy_task_count; i++) {
                heavy_leftover += heavy_task_queue[i].remaining_time;
                if (heavy_task_queue[i].remaining_time > 0) {
                    heavy_counter -= 1;
                }
            }

            // Determining completion status of heavy tasks
            if (heavy_counter == heavy_task_count && heavy_leftover == 0) {
                done += 1;
            } else {
                heavy_settle = 0;
            }
        }


        // Check if both light and heavy tasks are done and all processes executed
        if (done == 2 && processes_executed == n) {
            break;
        }
        else if (processes_executed == 0) {
            current_time += 1;   // Increment time if no processes executed
        }
    }

    // Calculate waiting time and turnaround time for each process
    calculateTurnaroundTime(processes, n);
    calculateWaitingTime(processes, n);

    // Calculate and print average waiting and turnaround times
    calculateAverageTimes(processes, n);

    return 0;
}
