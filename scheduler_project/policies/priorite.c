#include "../process.h"
#include <stdio.h>
#include "../colors.h"
#include "../graphics.h"

void priorite_schedule(Process processes[], int count) {
    Process queue[10];
    for (int i = 0; i < count; i++) {
        queue[i] = processes[i];
    }

    // Sort by priority (lower number = higher priority)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (queue[i].priorite > queue[j].priorite) {
                Process temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            }
        }
    }

    int current_time = 0;
    float avg_waiting = 0, avg_turnaround = 0, avg_response = 0;

    printf("\n--- PRIORITY SCHEDULING (Lower number = Higher priority) ---\n");

    for (int i = 0; i < count; i++) {
        Process* p = &queue[i];

        if (p->arrival_time > current_time) {
            current_time = p->arrival_time;
        }

        p->response_time = current_time - p->arrival_time;
        p->waiting_time = current_time - p->arrival_time;

        int total = 0;
        for (int j = 0; j < p->burst_count; j++) {
            total += p->bursts[j];
        }

        current_time += total;
        p->turnaround_time = current_time - p->arrival_time;

        printf("%s (Priority=%d): Response=%d, Waiting=%d, Turnaround=%d\n",
               p->name, p->priorite, p->response_time, p->waiting_time, p->turnaround_time);

        avg_waiting += p->waiting_time;
        avg_turnaround += p->turnaround_time;
        avg_response += p->response_time;
    }

    avg_waiting /= count;
    avg_turnaround /= count;
    avg_response /= count;

    printf("\n--- AVERAGES ---\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround);
    printf("Average Response Time: %.2f\n", avg_response);
    draw_gantt(queue, count);
draw_timeline(queue, count);
}
