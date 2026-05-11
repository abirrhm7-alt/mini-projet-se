#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"

// Function prototypes
void load_processes(const char* filename);
void fifo_schedule(Process processes[], int count);
void sjf_schedule(Process processes[], int count);
void rr_schedule(Process processes[], int count, int quantum);
void priorite_schedule(Process processes[], int count);

Process processes[MAX_PROCESSES];
int process_count = 0;

void load_processes(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    char line[200];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '#') continue;
        if (line[0] == '\n') continue;

        Process* p = &processes[process_count];
        
        // Parse: name,arrival,bursts...,priority
        int parsed = sscanf(line, "%[^,],%d,%d,%d,%d,%d", 
               p->name, &p->arrival_time, 
               &p->bursts[0], &p->bursts[1], &p->bursts[2], &p->priorite);
        
        if (parsed == 5) {
            p->priorite = 5;
            p->burst_count = 2;
        } else if (parsed == 6) {
            p->burst_count = 3;
        } else {
            parsed = sscanf(line, "%[^,],%d,%d,%d,%d,%d,%d,%d",
                   p->name, &p->arrival_time,
                   &p->bursts[0], &p->bursts[1], &p->bursts[2], 
                   &p->bursts[3], &p->bursts[4], &p->priorite);
            p->burst_count = 5;
        }

        p->waiting_time = 0;
        p->turnaround_time = 0;
        p->response_time = -1;
        p->completed = 0;

        process_count++;
    }
    fclose(file);
}

int main()
{
    load_processes("processes.txt");

    printf("\n=== LOADED PROCESSES ===\n");
    for (int i = 0; i < process_count; i++) {
        printf("%s: arrival=%d, priority=%d, bursts=%d\n",
               processes[i].name,
               processes[i].arrival_time,
               processes[i].priorite,
               processes[i].burst_count);
    }

    int choice;
    printf("\n--- SCHEDULING POLICIES ---\n");
    printf("1. FIFO (First Come First Served)\n");
    printf("2. SJF (Shortest Job First)\n");
    printf("3. Round Robin\n");
    printf("4. Priority (Higher priority first)\n");
    printf("Enter choice (1-4): ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            fifo_schedule(processes, process_count);
            break;
        case 2:
            sjf_schedule(processes, process_count);
            break;
        case 3: {
            int quantum;
            printf("Enter time quantum (ms): ");
            scanf("%d", &quantum);
            rr_schedule(processes, process_count, quantum);
            break;
        }
        case 4:
            priorite_schedule(processes, process_count);
            break;
        default:
            printf("Invalid choice. Running FIFO.\n");
            fifo_schedule(processes, process_count);
    }
    
    return 0;
}
