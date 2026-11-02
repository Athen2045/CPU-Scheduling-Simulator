#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 50

typedef struct {
    int id;
    int arrival;
    int burst;
    int priority;
    int remaining;
    int first_start;
    int completion;
} Process;

typedef struct {
    Process **items;
    int front;
    int rear;
    int count;
    int capacity;
} Queue;

void initQ(Queue *q, int cap) {
    q->items = (Process **)malloc(sizeof(Process *) * cap);
    if (q->items == NULL) {
        printf("Queue memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->capacity = cap;
}

void freeQ(Queue *q) {
    free(q->items);
    q->items = NULL;
}

int isEmpty(Queue *q) {
    return q->count == 0;
}

void enQ(Queue *q, Process *p) {
    if (q->count == q->capacity) {
        printf("Queue overflow. Increase the queue capacity.\n");
        exit(EXIT_FAILURE);
    }

    q->items[q->rear] = p;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
}

Process *deQ(Queue *q) {
    Process *p;

    if (isEmpty(q)) {
        return NULL;
    }

    p = q->items[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return p;
}

void resetP(Process p[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        p[i].remaining = p[i].burst;
        p[i].first_start = -1;
        p[i].completion = 0;
    }
}

void sortP(Process p[], int n) {
    int i;
    int j;

    /* Simple selection sort: useful here because it exposes the array logic. I Think????? */
    for (i = 0; i < n - 1; i++) {
        int earliest = i;
        for (j = i + 1; j < n; j++) {
            if (p[j].arrival < p[earliest].arrival) {
                earliest = j;
            }
        }
        if (earliest != i) {
            Process tmp = p[i];
            p[i] = p[earliest];
            p[earliest] = tmp;
        }
    }
}

void addArr(Process p[], int n, int t, int *nx, Queue *q) {
    while (*nx < n && p[*nx].arrival <= t) {
        enQ(q, &p[*nx]);
        (*nx)++;
    }
}

void showRun(Process *p, int st, int end) {
    printf("  time %d -> %d : P%d\n", st, end, p->id);
}

void fcfs(Process p[], int n) {
    Queue ready;
    int t = 0;
    int nx = 0;
    int done = 0;

    initQ(&ready, n + 1);
    printf("\nFCFS schedule (First Come, First Served)\n");

    while (done < n) {
        addArr(p, n, t, &nx, &ready);

        if (isEmpty(&ready)) {
            if (nx < n) {
                printf("  time %d -> %d : IDLE\n", t, p[nx].arrival);
                t = p[nx].arrival;
            }
            continue;
        }

        {
            Process *cur = deQ(&ready);
            int st = t;

            cur->first_start = t;
            t += cur->remaining;
            cur->remaining = 0;
            cur->completion = t;
            done++;
            showRun(cur, st, t);
        }
    }

    freeQ(&ready);
}

void rr(Process p[], int n, int quantum) {
    Queue ready;
    int t = 0;
    int nx = 0;
    int done = 0;

    initQ(&ready, n + 1);
    printf("\nRound Robin schedule (quantum = %d)\n", quantum);

    while (done < n) {
        Process *cur;
        int run;
        int st;

        addArr(p, n, t, &nx, &ready);
        if (isEmpty(&ready)) {
            if (nx < n) {
                printf("  time %d -> %d : IDLE\n", t, p[nx].arrival);
                t = p[nx].arrival;
            }
            continue;
        }

        cur = deQ(&ready);
        if (cur->first_start == -1) {
            cur->first_start = t;
        }

        run = cur->remaining < quantum ? cur->remaining : quantum;
        st = t;
        t += run;
        cur->remaining -= run;
        showRun(cur, st, t);

        addArr(p, n, t, &nx, &ready);
        if (cur->remaining > 0) {
            enQ(&ready, cur);
        } else {
            cur->completion = t;
            done++;
        }
    }

    freeQ(&ready);
}

Process *pickPri(Queue *ready) {
    int i;
    int best = ready->front;
    int position = ready->front;

    for (i = 0; i < ready->count; i++) {
        Process *candidate = ready->items[position];
        Process *selected = ready->items[best];

        if (candidate->priority < selected->priority ||
            (candidate->priority == selected->priority && candidate->arrival < selected->arrival)) {
            best = position;
        }
        position = (position + 1) % ready->capacity;
    }

    /* Remove the chosen item while preserving the order of the other items. Its position is stored in 'best'. I believe this is the correct approach. */
    {
        Process *chosen = ready->items[best];
        position = best;
        while (position != ready->rear) {
            int next_position = (position + 1) % ready->capacity;
            if (next_position == ready->rear) {
                break;
            }
            ready->items[position] = ready->items[next_position];
            position = next_position;
        }
        ready->rear = (ready->rear - 1 + ready->capacity) % ready->capacity;
        ready->count--;
        return chosen;
    }
}

void pri(Process p[], int n) {
    Queue ready;
    int t = 0;
    int nx = 0;
    int done = 0;

    initQ(&ready, n + 1);
    printf("\nPriority schedule (non-preemptive; smaller number = higher priority)\n");

    while (done < n) {
        Process *cur;
        int st;

        addArr(p, n, t, &nx, &ready);
        if (isEmpty(&ready)) {
            if (nx < n) {
                printf("  time %d -> %d : IDLE\n", t, p[nx].arrival);
                t = p[nx].arrival;
            }
            continue;
        }

        cur = pickPri(&ready);
        cur->first_start = t;
        st = t;
        t += cur->remaining;
        cur->remaining = 0;
        cur->completion = t;
        done++;
        showRun(cur, st, t);
    }

    freeQ(&ready);
}

void showRes(Process processes[], int count) {
    int i;
    double total_waiting = 0.0;
    double total_turnaround = 0.0;
    double total_response = 0.0;

    printf("\nResults\n");
    printf("PID  Arrival  Burst  Priority  Completion  Turnaround  Waiting  Response\n");
    for (i = 0; i < count; i++) {
        int turnaround = processes[i].completion - processes[i].arrival;
        int waiting = turnaround - processes[i].burst;
        int response = processes[i].first_start - processes[i].arrival;

        printf("P%-3d %-8d %-6d %-9d %-11d %-11d %-8d %d\n",
               processes[i].id, processes[i].arrival, processes[i].burst,
               processes[i].priority, processes[i].completion, turnaround,
               waiting, response);
        total_waiting += waiting;
        total_turnaround += turnaround;
        total_response += response;
    }

    printf("\nAverage waiting time    : %.2f\n", total_waiting / count);
    printf("Average turnaround time : %.2f\n", total_turnaround / count);
    printf("Average response time   : %.2f\n", total_response / count);
}

int main(void) {
    Process *processes;
    int count;
    int choice;
    int quantum = 0;

    printf("CPU Scheduling Simulator\n");
    printf("Enter number of processes (1-%d): ", MAX_PROCESSES);
    if (scanf("%d", &count) != 1 || count < 1 || count > MAX_PROCESSES) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    processes = (Process *)malloc(sizeof(Process) * count);
    if (processes == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("\nFor each process enter: arrival_time burst_time priority\n");
    printf("Priority uses smaller numbers first (1 is highest).\n");
    printf("Example: 0 5 2 means arrival=0, burst=5, priority=2.\n");
    {
        int i;
        for (i = 0; i < count; i++) {
            processes[i].id = i + 1;
            printf("P%d (arrival burst priority): ", i + 1);
            if (scanf("%d %d %d", &processes[i].arrival,
                      &processes[i].burst, &processes[i].priority) != 3 ||
                processes[i].arrival < 0 || processes[i].burst <= 0 ||
                processes[i].priority <= 0) {
                printf("Invalid process input.\n");
                free(processes);
                return 1;
            }
        }
    }

    printf("\nChoose an algorithm:\n");
    printf("1. First Come, First Served (FCFS)\n");
    printf("2. Round Robin\n");
    printf("3. Priority (non-preemptive)\n");
    printf("Choice: ");
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("Invalid choice.\n");
        free(processes);
        return 1;
    }

    if (choice == 2) {
        printf("Enter time quantum (> 0): ");
        if (scanf("%d", &quantum) != 1 || quantum <= 0) {
            printf("Invalid time quantum.\n");
            free(processes);
            return 1;
        }
    }

    printf("\nTimeline format: start time -> end time : process using the CPU.\n");
    sortP(processes, count);
    resetP(processes, count);
    if (choice == 1) {
        fcfs(processes, count);
    } else if (choice == 2) {
        rr(processes, count, quantum);
    } else {
        pri(processes, count);
    }
    showRes(processes, count);

    printf("\nHow to read the results:\n");
    printf("Turnaround = completion - arrival\n");
    printf("Waiting    = turnaround - burst\n");
    printf("Response   = first CPU start - arrival\n");

    free(processes);
    return 0;
}
