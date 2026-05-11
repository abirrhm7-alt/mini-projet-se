#include "graphics.h"
#include <stdio.h>

void draw_gantt(Process processes[], int count) {
    printf("\n");
    printf("============================================\n");
    printf("         PROCESS EXECUTION CHART\n");
    printf("============================================\n\n");
    
    int max_time = 0;
    for (int i = 0; i < count; i++) {
        if (processes[i].turnaround_time > max_time) {
            max_time = processes[i].turnaround_time;
        }
    }
    
    int chart_width = 50;
    
    for (int i = 0; i < count; i++) {
        Process p = processes[i];
        
        int exec_len = (p.turnaround_time - p.waiting_time) * chart_width / max_time;
        int wait_len = p.waiting_time * chart_width / max_time;
        
        printf("%s |", p.name);
        
        for (int w = 0; w < wait_len; w++) {
            printf(".");
        }
        
        for (int e = 0; e < exec_len; e++) {
            printf("#");
        }
        
        int remaining = chart_width - wait_len - exec_len;
        for (int r = 0; r < remaining; r++) {
            printf(" ");
        }
        
        printf("| %d ms\n", p.turnaround_time);
    }
    
    printf("\nLegend: . = Waiting    # = Executing\n");
    printf("============================================\n");
}

void draw_timeline(Process processes[], int count) {
    printf("\n=== TIMELINE ===\n\n");
    
    int max_time = 0;
    for (int i = 0; i < count; i++) {
        if (processes[i].turnaround_time > max_time) {
            max_time = processes[i].turnaround_time;
        }
    }
    
    int chart_width = 50;
    
    for (int i = 0; i < count; i++) {
        Process p = processes[i];
        
        int exec_len = (p.turnaround_time - p.waiting_time) * chart_width / max_time;
        int wait_len = p.waiting_time * chart_width / max_time;
        
        printf("%s    ", p.name);
        
        for (int w = 0; w < wait_len; w++) {
            printf(".");
        }
        
        for (int e = 0; e < exec_len; e++) {
            printf("#");
        }
        
        printf(" %d ms\n", p.turnaround_time);
    }
    
    printf("\nLegend: . = Waiting    # = Executing\n");
}
