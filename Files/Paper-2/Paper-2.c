#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define QUANTUM_FACTOR 0.8

// Define the structure for a process
typedef struct Process {
    int burst_time;             // Time required for execution
    int remaining_time;         // Remaining time for execution
    int waiting_time;           // Time spent waiting in the ready queue
    int turnaround_time;        // Total time from arrival to completion
    int response_time;          // Time taken from arrival until the first response
    int process_id;             // ID of the process
    int arrival_time;           // Time at which process arrives
    bool added_to_ready_queue;  // Flag to indicate if the process has been added to the ready queue
} Process;

// Define a structure for the ready queue
typedef struct ReadyQueueNode {
    Process *process;           // Pointer to the process in the ready queue
    struct ReadyQueueNode *next;    // Pointer to the next process in the ready queue
} ReadyQueueNode;

// Global variables for ready queue management
ReadyQueueNode *head = NULL;        // Pointer to the head of the ready queue
ReadyQueueNode *tail = NULL;        // Pointer to the tail of the ready queue

// Function to check if the ready queue is empty
bool is_ready_queue_empty() {
    return head == NULL;
}

// Function to free memory allocated for the ready queue
void free_ready_queue() {
    ReadyQueueNode *current = head;
    ReadyQueueNode *next_node;

    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    head = NULL;
    tail = NULL;
}

// Function to sort processes by arrival time
void sort_processes_by_arrival_time(Process *process_list, int num_processes) {
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = 0; j < num_processes - i - 1; j++) {
            if (process_list[j].arrival_time > process_list[j + 1].arrival_time) {
                Process temp = process_list[j];
                process_list[j] = process_list[j + 1];
                process_list[j + 1] = temp;
            }
        }
    }
}

// Function to update the ready queue based on arrival time
int update_ready_queue(Process *process_list, int current_time, int num_processes) {
    bool process_added = false;
    for (int i = 0; i < num_processes; i++) {
        if (current_time >= process_list[i].arrival_time && !process_list[i].added_to_ready_queue) {
            // Allocate memory for a new ready queue node
            ReadyQueueNode *new_node = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
            if (new_node == NULL) {
                // Handle memory allocation error
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            // Set the process pointer in the new node
            new_node->process = &process_list[i]; 
            new_node->next = NULL; 

            process_list[i].added_to_ready_queue = true;

            // Print debug message
            printf("Process ID Added to RQ: %d\n", process_list[i].process_id);

            // Add the new node to the ready queue
            if (is_ready_queue_empty()) {
                head = new_node;
                tail = new_node;
            } else {
                tail->next = new_node;
                tail = new_node;
            }

            process_added = true;
        }
    }

    return process_added ? 1 : 0;
}

// Function to get the size of the ready queue
int get_ready_queue_size() {
    int size = 0;
    ReadyQueueNode *current = head;

    while (current != NULL) {
        size++;
        current = current->next;
    }

    return size;
}

// Function to return the maximum burst time in the ready queue
int return_max_burst_time(ReadyQueueNode *head) {
    int max_burst_time = 0;
    while (head != NULL) {
        if (head->process->burst_time > max_burst_time) {
            max_burst_time = head->process->burst_time;
        }

        head = head->next;
    }

    return max_burst_time;
}

// Function to calculate the average waiting time of all processes
float calculate_average_waiting_time(Process process_list[], int num_processes) {
    float total_waiting_time = 0;
    for (int i = 0; i < num_processes; ++i) {
        total_waiting_time += process_list[i].waiting_time;
    }
    return total_waiting_time / num_processes;
}

// Function to calculate the average turnaround time of all processes
float calculate_average_turnaround_time(Process process_list[], int num_processes) {
    float total_turnaround_time = 0;
    for (int i = 0; i < num_processes; ++i) {
        total_turnaround_time += process_list[i].turnaround_time;
    }
    return total_turnaround_time / num_processes;
}

// Main function
int main() {

    int current_time = 0;
    int num_processes;
    int max_burst_time = 0;
    int context_switch_count = -1;


    // Input the number of processes
    printf("Input no. of processes: ");
    scanf("%d", &num_processes);

    // Allocate memory for the array of processes
    Process *process_list = malloc(num_processes * sizeof(Process));
    if (process_list == NULL) {
        // Handle memory allocation error
        fprintf(stderr, "Memory allocation error\n");
        return EXIT_FAILURE;
    }

    // Input burst time and arrival time for each process
    for (int i = 0; i < num_processes; i++) {
        printf("Enter process %d burst time: ", i + 1);
        scanf("%d", &process_list[i].burst_time);

        printf("Enter process %d arrival time: ", i + 1);
        scanf("%d", &process_list[i].arrival_time);

        // Assignment of process ID
        process_list[i].process_id = i + 1;
        
        // Default attributes initialization
        process_list[i].waiting_time = 0;
        process_list[i].remaining_time = process_list[i].burst_time;
        process_list[i].turnaround_time = 0;
        process_list[i].response_time = 0;
        process_list[i].added_to_ready_queue = false;
    }

    // Sort processes by arrival time
    sort_processes_by_arrival_time(process_list, num_processes);

    // Update the ready queue based on arrival time
    update_ready_queue(process_list, current_time, num_processes);

    int remaining_processes = 0;
    ReadyQueueNode *current_node = head;

    // Calculate the maximum burst time in the ready queue
    max_burst_time = return_max_burst_time(current_node);

    // Set quantum time as 80% of the maximum burst time
    int quantum_time = QUANTUM_FACTOR * max_burst_time;

    // Loop until the ready queue is empty
    while (!is_ready_queue_empty()) {
        ReadyQueueNode *process_node = head;
        Process *process = process_node->process;
        
        // If remaining time of the process is less than or equal to quantum time
        if (process->remaining_time <= quantum_time) {
            printf("Assigning CPU to Process ID: %d\n", process->process_id);
            context_switch_count++;
            process->waiting_time = current_time - process->arrival_time;
            current_time += process->remaining_time;
            process->turnaround_time = current_time - process->arrival_time;
            
            // Update remaining time of the process
            process->remaining_time = 0;

            // Remove the process from the ready queue
            head = head->next;
            free(process_node);

            // If new processes are added to the ready queue, recalculate the quantum time
            if (update_ready_queue(process_list, current_time, num_processes) == 1) {
                ReadyQueueNode *temp = head;

                // Recalculate maximum burst time in the ready queue
                max_burst_time = return_max_burst_time(temp);
                quantum_time = QUANTUM_FACTOR * max_burst_time;
                remaining_processes = 0;
            }
        } else {
            // Move the process to the tail of the ready queue
            context_switch_count++;
            printf("Moving Process ID: %d to Tail\n", head->process->process_id);
            tail->next = head;
            tail = head;
            head = head->next;
            tail->next = NULL;
            remaining_processes++;

        }
        
        // If the number of remaining processes equals the size of the ready queue, reset quantum time
        if (remaining_processes == get_ready_queue_size()) {
            max_burst_time = 0;
            ReadyQueueNode *current = head;
            max_burst_time = return_max_burst_time(current);
            quantum_time = max_burst_time;
        }
    }

    // Print process details
    printf("Process\tBurst Time\tWaiting Time\tTurnaround Time\tResponse Time\tArrival Time\n");
    for (int i = 0; i < num_processes; i++) { 
        printf("P%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", process_list[i].process_id, process_list[i].burst_time, process_list[i].waiting_time, process_list[i].turnaround_time, process_list[i].response_time, process_list[i].arrival_time);
    }

    // Print average waiting time and average turnaround time
    printf("\n");
    printf("Average Waiting Time: %.2f\n", calculate_average_waiting_time(process_list, num_processes));
    printf("Average Turnaround Time: %.2f\n", calculate_average_turnaround_time(process_list, num_processes));
    printf("Total number of context switches: %d\n", context_switch_count);

    // Free allocated memory
    free(process_list);
    free_ready_queue();

    return 0;
}
