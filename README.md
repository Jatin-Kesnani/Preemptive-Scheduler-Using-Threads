# Preemptive Scheduler Using Threads

A C++ implementation of four preemptive CPU scheduling algorithms using POSIX threads (pthreads) and semaphores for synchronization.

## Algorithms Implemented
1. **Shortest Remaining Time First (SRTF)**
2. **Longest Remaining Time First (LRTF)**
3. **Priority Scheduling**
4. **Round Robin (RR)**

## Features
- Thread-based simulation of process scheduling
- Synchronization using semaphores
- Detailed process statistics (waiting time, turnaround time)
- Gantt chart visualization
- Interactive menu-driven interface
- Colorful terminal output

## Requirements
- Linux/Unix system
- GCC compiler
- pthread library

## Installation & Usage
```bash
# Compile
g++ scheduler.cpp -o scheduler -lpthread

# Run
./scheduler
```
