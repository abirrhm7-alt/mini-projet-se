#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 10
#define MAX_NAME 20

typedef struct
{
    char name[MAX_NAME];
    int arrival_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int completed;
    int bursts[10];
    int burst_count;
    int priorite;
} Process;

typedef struct {
    int start;
    int end;
    int process_id;
} Interval;

#endif
