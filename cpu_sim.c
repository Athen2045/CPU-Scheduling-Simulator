#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 50

/* Stores details and final times for one process. ITS EASIER TO USE A STRUCTURE*/
typedef struct {
    int id;
    int arrival;
    int burst;
    int start;
    int finish;
} Process;

/* Circular queue that stores pointers to processes. So that we can manage the order of execution*/
typedef struct {
    Process **data;
    int front;
    int rear;
    int n;
    int cap;
} Queue;

/* Created an empty queue and allocate space for its process pointers */
void initQ(Queue *q, int cap) {
    q->data = (Process **)malloc(sizeof(Process *) * cap);
    if (q->data == NULL) {
        printf("Queue memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    q->front = 0;
    q->rear = 0;
    q->n = 0;
    q->cap = cap;
}

/* FREEE memory used by the queue, "LET IT GO, LEt it go"*/
void freeQ(Queue *q) {
    free(q->data);
    q->data = NULL;
}

/* Returns 1 when the queue has no processes*/
int emptyQ(Queue *q) {
    return q->n == 0;
}

/* Add a process pointer at the rear of the circular queue*/
void enQ(Queue *q, Process *p) {
    if (q->n == q->cap) {
        printf("Queue overflow.\n");
        exit(EXIT_FAILURE);
    }

    q->data[q->rear] = p;
    q->rear = (q->rear + 1) % q->cap;
    q->n++;
}

Process *deQ(Queue *q) {
    Process *p;

    if (emptyQ(q)) {
        return NULL;
    }

    p = q->data[q->front];
    q->front = (q->front + 1) % q->cap;
    q->n--;
    return p;
}

void sortP(Process p[], int n) {
    int i;
    int j;

    for (i = 0; i < n - 1; i++) {
        int first = i;

        for (j = i + 1; j < n; j++) {
            if (p[j].arrival < p[first].arrival) {
                first = j;
            }
        }

        if (first != i) {
            Process tmp = p[i];
            p[i] = p[first];
            p[first] = tmp;
        }
    }
}

/* Add all processes that have arrived by the current time */
void addArr(Process p[], int n, int t, int *nx, Queue *q) {
    while (*nx < n && p[*nx].arrival <= t) {
        enQ(q, &p[*nx]);
        (*nx)++;
    }
}

/* Prints one part of the CPU execution timeline*/
void showRun(Process *p, int st, int end) {
    printf("  time %d -> %d : P%d\n", st, end, p->id);
}

/* Runs FCFS: the first process in the queue runs until completions*/
void fcfs(Process p[], int n) {
    Queue q;
    int t = 0;
    int nx = 0;
    int done = 0;

    initQ(&q, n + 1);
    printf("\nFCFS schedule (First Come, First Served)\n");

    while (done < n) {
        addArr(p, n, t, &nx, &q);

        if (emptyQ(&q)) {
            if (nx < n) {
                printf("  time %d -> %d : IDLE\n", t, p[nx].arrival);
                t = p[nx].arrival;
            }
            continue;
        }

        {
            Process *cur = deQ(&q);
            int st = t;

            cur->start = t;
            t += cur->burst;
            cur->finish = t;
            done++;
            showRun(cur, st, t);
        }
    }

    freeQ(&q);
}

/* Calculates and displays the scheduling metrics*/
void showRes(Process p[], int n) {
    int i;
    double totalWait = 0.0;
    double totalTurn = 0.0;
    double totalResp = 0.0;

    printf("\nResults\n");
    printf("PID  Arrival  Burst  Start  Finish  Turnaround  Waiting  Response\n");

    for (i = 0; i < n; i++) {
        /* Calculates metrics for the current proce*/
        int turn = p[i].finish - p[i].arrival;
        int wait = turn - p[i].burst;
        int resp = p[i].start - p[i].arrival;

        printf("P%-3d %-8d %-6d %-6d %-7d %-11d %-8d %d\n",
               p[i].id, p[i].arrival, p[i].burst, p[i].start,
               p[i].finish, turn, wait, resp);

        totalWait += wait;
        totalTurn += turn;
        totalResp += resp;
    }

    printf("\nAverage waiting time    : %.2f\n", totalWait / n);
    printf("Average turnaround time : %.2f\n", totalTurn / n);
    printf("Average response time   : %.2f\n", totalResp / n);
}

/* Reads input and runs the simulator, and releases allocated memory */
int main(void) {
    Process *p;
    int n;
    int i;

    printf("CPU Scheduling Simulator\n");
    printf("This version demonstrates FCFS using a circular queue.\n");
    printf("Enter number of processes (1-%d): ", MAX_PROCESSES);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_PROCESSES) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    p = (Process *)malloc(sizeof(Process) * n);
    if (p == NULL) {
        printf("Process memory allocation failed.\n");
        return 1;
    }

    printf("\nFor each process enter: arrival_time burst_time\n");
    printf("Example: 0 5 means arrival=0 and burst=5.\n");

    for (i = 0; i < n; i++) {
        p[i].id = i + 1;
        printf("P%d (arrival burst): ", i + 1);

        if (scanf("%d %d", &p[i].arrival, &p[i].burst) != 2 ||
            p[i].arrival < 0 || p[i].burst <= 0) {
            printf("Invalid process input.\n");
            free(p);
            return 1;
        }
    }

    printf("\nTimeline format: start time -> end time : process using the CPU.\n");
    sortP(p, n);
    fcfs(p, n);
    showRes(p, n);

    printf("\nHow to read the results:\n");
    printf("Turnaround = finish - arrival\n");
    printf("Waiting    = turnaround - burst\n");
    printf("Response   = first CPU start - arrival\n");

    free(p);
    return 0;
}
