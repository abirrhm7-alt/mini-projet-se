#include "../process.h"
#include "../colors.h"
#include <stdio.h>
#include <string.h>

void rr_schedule(Process processes[], int count, int quantum) {
    Process queue[10];
    int remaining[10];
    int finished[10] = {0};
    int response_set[10] = {0};
    int current_time = 0;
    int completed = 0;
    float avg_waiting = 0, avg_turnaround = 0, avg_response = 0;
    
    // Store execution intervals for timeline
    Interval intervals[100];
    int interval_count = 0;
    
    // Initialize queue with correct names
    for (int i = 0; i < count; i++) {
        strcpy(queue[i].name, processes[i].name);
        queue[i].arrival_time = processes[i].arrival_time;
        queue[i].burst_count = processes[i].burst_count;
        queue[i].priorite = processes[i].priorite;
        queue[i].waiting_time = 0;
        queue[i].turnaround_time = 0;
        queue[i].response_time = -1;
        queue[i].completed = 0;
        for (int j = 0; j < processes[i].burst_count; j++) {
            queue[i].bursts[j] = processes[i].bursts[j];
        }
        remaining[i] = queue[i].bursts[0];
    }
    
    // Round robin queue (indices into queue array)
    int rr_queue[20];
    int front = 0, rear = 0;
    
    // Add initial processes by arrival time
    for (int i = 0; i < count; i++) {
        if (queue[i].arrival_time == 0) {
            rr_queue[rear++] = i;
        }
    }
    
    printf("\n" BOLD BLUE "--- ROUND ROBIN (Quantum=%d) ---\n" RESET, quantum);
    
    while (completed < count) {
        // If no process ready, advance time
        if (front >= rear) {
            current_time++;
            // Add any newly arrived processes
            for (int i = 0; i < count; i++) {
                if (queue[i].arrival_time == current_time && !finished[i]) {
                    rr_queue[rear++] = i;
                }
            }
            continue;
        }
        
        // Get next process
        int current = rr_queue[front++];
        
        // Set response time if first time
        if (response_set[current] == 0) {
            queue[current].response_time = current_time - queue[current].arrival_time;
            response_set[current] = 1;
        }
        
        // Execute for quantum or remaining time
        int exec_time = (remaining[current] < quantum) ? remaining[current] : quantum;
        int start_time = current_time;
        
        // Record interval for timeline
        intervals[interval_count].start = start_time;
        intervals[interval_count].end = start_time + exec_time;
        intervals[interval_count].process_id = current;
        interval_count++;
        
        remaining[current] -= exec_time;
        current_time += exec_time;
        
        // Add newly arrived processes during this execution
        for (int i = 0; i < count; i++) {
            if (queue[i].arrival_time <= current_time && !finished[i] && i != current) {
                int already = 0;
                for (int k = front; k < rear; k++) {
                    if (rr_queue[k] == i) already = 1;
                }
                if (!already) {
                    rr_queue[rear++] = i;
                }
            }
        }
        
        // Check if process finished
        if (remaining[current] == 0) {
            queue[current].turnaround_time = current_time - queue[current].arrival_time;
            queue[current].waiting_time = queue[current].turnaround_time - queue[current].bursts[0];
            
            printf(GREEN "%s" RESET ": Response=%d, Waiting=%d, Turnaround=%d\n",
                   queue[current].name, queue[current].response_time,
                   queue[current].waiting_time, queue[current].turnaround_time);
            
            avg_waiting += queue[current].waiting_time;
            avg_turnaround += queue[current].turnaround_time;
            avg_response += queue[current].response_time;
            
            completed++;
            finished[current] = 1;
        } else {
            // Put back at end of queue
            rr_queue[rear++] = current;
        }
        
        // Safety limit
        if (current_time > 1000) break;
    }
    
    // Calculate averages
    avg_waiting /= count;
    avg_turnaround /= count;
    avg_response /= count;
    
    printf(BOLD CYAN "\n--- AVERAGES ---\n" RESET);
    printf(YELLOW "Average Waiting Time: %.2f\n" RESET, avg_waiting);
    printf(YELLOW "Average Turnaround Time: %.2f\n" RESET, avg_turnaround);
    printf(YELLOW "Average Response Time: %.2f\n" RESET, avg_response);
    
    // Copy results back to original processes array
    for (int i = 0; i < count; i++) {
        strcpy(processes[i].name, queue[i].name);
        processes[i].waiting_time = queue[i].waiting_time;
        processes[i].turnaround_time = queue[i].turnaround_time;
        processes[i].response_time = queue[i].response_time;
    }
    
    // Draw timeline using queue names
    printf("\n" BOLD CYAN "=== ROUND ROBIN TIMELINE ===\n" RESET);
    
    int max_time = current_time;
    int chart_width = 50;
    if (max_time == 0) max_time = 1;
    
    for (int i = 0; i < count; i++) {
        // Use queue[i].name which is the correct name
        printf("%s    ", queue[i].name);
        
        for (int t = 0; t < max_time; t++) {
            char c = '.';
            for (int k = 0; k < interval_count; k++) {
                if (intervals[k].process_id == i && t >= intervals[k].start && t < intervals[k].end) {
                    c = '#';
                    break;
                }
            }
            // Scale to chart width
            int pos = t * chart_width / max_time;
            int next_pos = (t + 1) * chart_width / max_time;
            if (pos < next_pos) {
                printf("%c", c);
            }
        }
        printf(" %d ms\n", queue[i].turnaround_time);
    }
    
    printf("\nLegend: # = Running    . = Waiting\n");
}
