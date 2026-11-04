# Group 12: CPU Scheduling Simulator

## What is it?

This program is a small command-line simulator for **First Come, First Served (FCFS)** CPU scheduling. FCFS is also called FIFO scheduling because the first process to enter the ready queue is the first process to receive the CPU.

The program focuses on the C and data-structure concepts covered in Modules 1 and 2: variables, conditions, loops, arrays, pointers, dynamic memory allocation, structures, functions, and circular queues.

## How the program works

Each process is represented by a `Process` structure containing:

- `id`: process number
- `arrival`: time at which the process becomes ready
- `burst`: CPU time required by the process
- `start`: first time the process receives the CPU
- `finish`: time at which the process completes

The ready queue is represented by a `Queue` structure. It stores pointers to `Process` structures in a dynamically allocated circular array. The queue uses `front`, `rear`, `n`, and `cap` to track its state.

Before scheduling, `sortP()` orders the process array by arrival time. During the simulation, `addArr()` places every process that has arrived into the ready queue. `fcfs()` removes processes from the front of the queue and runs each one until its burst is complete. FCFS is non-preemptive, so a running process is never interrupted.

If the queue is empty, the CPU is idle. The simulated time then moves forward to the next process arrival.

## Main functions

### Queue functions

- `initQ()` allocates the queue array and initializes its indexes.
- `enQ()` inserts a process pointer at the rear.
- `deQ()` removes a process pointer from the front.
- `emptyQ()` checks whether the queue contains no processes.
- `freeQ()` releases the queue memory.

The circular behavior is created with modulo arithmetic:

```c
(index + 1) % capacity
```

When an index reaches the end of the array, it wraps back to position zero. This demonstrates the circular queue implementation from Module 2.

### `sortP()`

`sortP()` uses a simple selection sort to arrange processes by arrival time. This allows processes to be entered in any order while preserving correct FCFS behavior.

### `addArr()`

`addArr()` scans the sorted process array. Every process whose arrival time is less than or equal to the current simulated time is inserted into the ready queue. The `nx` parameter is passed by pointer so the function can update which process should be checked next.

### `fcfs()`

This is the main scheduling function. It repeatedly:

1. Adds newly arrived processes to the queue.
2. Checks whether the queue is empty.
3. Removes the process at the front of the queue.
4. Advances the CPU clock by that process's burst time.
5. Records the process's start and finish times.

The function uses a pointer, `cur`, to modify the selected process structure directly.

### `showRun()`

`showRun()` prints each execution interval in the form:

```text
time start -> time end : process
```

This makes the simulated CPU timeline easy to follow.

### `showRes()`

`showRes()` calculates and displays the scheduling metrics:

```text
turnaround = finish - arrival
waiting    = turnaround - burst
response   = start - arrival
```

It also calculates the average waiting, turnaround, and response times using loops and `double` variables.

### `main()`

`main()` handles input and program flow. It validates the number of processes, dynamically allocates the process array, reads arrival and burst times, sorts the processes, calls `fcfs()`, displays the results, and finally releases the allocated memory.

## Expected behavior

For example, the input:

```text
3
0 5
1 3
2 4
```

means that P1 arrives at time 0 and needs 5 CPU units, P2 arrives at time 1 and needs 3 CPU units, and P3 arrives at time 2 and needs 4 CPU units.

Since FCFS is non-preemptive, P1 runs from time 0 to 5. P2 and P3 wait in arrival order, so P2 runs from 5 to 8 and P3 runs from 8 to 12.

The simulator does not model I/O blocking, context-switch overhead, multiple CPUs, or real operating-system processes. Its purpose is to make FIFO queue behavior and scheduling metrics visible through a simple discrete-time model.

## Example Output

The following image shows example input, the FCFS execution timeline, and the calculated metrics.

![Example output from the CPU Scheduling Simulator](assets/Output.png)

## Sources

- [Source Code Inspiration](https://github.com/yousefkotp/CPU-Scheduling-Algorithms)
- [Creating a CPU Scheduling Simulator](https://www.scribd.com/document/977387928/raju-os)
- [CPU Scheduling in Operating Systems](https://www.geeksforgeeks.org/operating-systems/cpu-scheduling-in-operating-systems/)
