#include <stdio.h>
#include <stdlib.h>
#include "../colors.h"
#define MAX_NAME 20
#include "../process.h"
#include "../graphics.h"

void sjf_schedule(Process processes[], int count) {
    Process queue[10];
    for (int i = 0; i < count; i++) {
        queue[i] = processes[i];
    }

    int cpu_total[10];

    for (int i = 0; i < count; i++) {
        cpu_total[i] = 0;
        for (int j = 0; j < queue[i].burst_count; j++) {
            if (j % 2 == 0) {
                cpu_total[i] += queue[i].bursts[j];
            }
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (cpu_total[j] > cpu_total[j + 1]) {
                Process temp_p = queue[j];
                queue[j] = queue[j + 1];
                queue[j + 1] = temp_p;
                
                int temp_c = cpu_total[j];
                cpu_total[j] = cpu_total[j + 1];
                cpu_total[j + 1] = temp_c;
            }
        }
    }

    int current_time = 0;
    float avg_waiting = 0, avg_turnaround = 0, avg_response = 0;

    printf("\n--- SJF SCHEDULING ---\n");

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

        printf("%s: Response=%d, Waiting=%d, Turnaround=%d\n",
               p->name, p->response_time, p->waiting_time, p->turnaround_time);

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
