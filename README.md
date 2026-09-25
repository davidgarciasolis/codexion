*This project has been created as part of the 42 curriculum by davgarc4.*

# Codexion

Codexion is a concurrent C simulation inspired by the dining philosophers problem. Several coders share USB dongles arranged in a circle: every coder needs both adjacent dongles to compile, then alternates between compiling, debugging, and refactoring. The goal is to coordinate access to these limited resources without deadlocks, while respecting their cooldown and preventing a coder from burning out.

The project uses POSIX threads, mutexes, condition variables, and a priority queue implemented as a heap. Pending requests can be arbitrated with FIFO or EDF (*Earliest Deadline First*) scheduling.

## Description

Each coder runs in its own thread. After refactoring, it requests its left and right dongles before starting another compilation. A released dongle remains unavailable for the configured cooldown period.

A dedicated monitor thread periodically checks every coder's deadline. The simulation ends when a coder burns out because it did not start compiling in time, or when every coder has completed the required number of compilations.

## Instructions

### Build

```sh
make
```

This produces the `codexion` executable. The project is compiled with `cc -Wall -Wextra -Werror -pthread`.

The usual Makefile rules are also available:

```sh
make clean
make fclean
make re
```

### Run

```text
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All times are in milliseconds. The seven numeric parameters must be positive integers, and `scheduler` must be either `fifo` or `edf`.

| Argument | Description |
| --- | --- |
| `number_of_coders` | Number of coders and available dongles. |
| `time_to_burnout` | Maximum time without starting a compilation before burning out. |
| `time_to_compile` | Duration of compilation, during which both dongles are held. |
| `time_to_debug` | Duration of the debugging phase. |
| `time_to_refactor` | Duration of the refactoring phase. |
| `number_of_compiles_required` | Number of compilations every coder must finish for a successful simulation. |
| `dongle_cooldown` | Time a dongle remains unavailable after it is released. |
| `scheduler` | Scheduling policy: `fifo` or `edf`. |

Example:

```sh
./codexion 4 800 200 200 200 3 50 fifo
```

Events are printed as `timestamp_in_ms coder_id state`. The program reports dongle acquisition, compilation, debugging, refactoring, burnout, and simulation completion.

## Scheduling

Requests are stored in a priority heap:

- With `fifo`, the earliest request wins.
- With `edf`, the coder with the closest deadline wins: `last_compile_start + time_to_burnout`. The lower coder ID breaks ties.

The scheduler selects the highest-priority request that can acquire both dongles at once. Therefore, a request blocked by a neighbouring dongle does not prevent another compatible request from being granted.

## Blocking cases handled

- **Deadlock:** no thread acquires one dongle and waits for the other. The scheduler grants both dongles as one protected logical operation, removing circular wait and the Coffman *hold and wait* condition.
- **Starvation:** pending requests are ordered in a heap according to the chosen policy. FIFO preserves arrival order; EDF deterministically serves the most urgent deadline and uses the lower coder ID as a tie-breaker.
- **Resource contention:** all dongle, queue, and grant state changes occur while holding the same mutex. A request is granted only when both adjacent dongles are available.
- **Cooldown:** releasing a dongle records its `lista_en` availability time. The scheduler cannot allocate it before that time, and timed waits let pending coders retry when a cooldown expires.
- **Burnout:** a dedicated monitor thread checks deadlines and stops the simulation when one is exceeded. It wakes waiting threads so they can exit cleanly.
- **Interleaved output:** a dedicated output mutex serializes every log line, including burnout and completion messages.
- **Coordinated completion:** when all coders reach the compilation target, shared state is marked as stopped and all waiting threads are notified.

## Thread synchronization mechanisms

The simulation uses two mutexes and one POSIX condition variable:

| Mechanism | Purpose |
| --- | --- |
| `pthread_mutex_t cerrojo` | Protects dongles, the waiting heap, turns, deadlines, counters, and the global stop flag. |
| `pthread_mutex_t cerrojo_impresion` | Ensures that one complete log line is printed before another thread writes. |
| `pthread_cond_t cambio` | Wakes coders when resource availability changes, a cooldown expires, or the simulation stops. |

When requesting dongles, a coder locks `cerrojo`, enters the heap, and waits on `cambio` until it receives a grant. Waiting releases the mutex atomically and reacquires it before checking the predicate again. As a result, queue and dongle state are never read or updated without protection. Releases, grants, and shutdown use `broadcast` so affected requests can reevaluate their state.

The monitor also checks and updates the stop state while holding `cerrojo`. Logging checks this state while locking mutexes in a consistent order (`cerrojo_impresion`, then `cerrojo`), preventing race conditions in output and messages after shutdown.

## Project structure

| File | Responsibility |
| --- | --- |
| `main.c` | Creates and joins coder and monitor threads. |
| `parseo.c` | Argument validation and configuration. |
| `simulation.c`, `cleanup.c` | Shared-state initialization and resource cleanup. |
| `developer.c` | Each coder's compile, debug, and refactor cycle. |
| `dongle.c`, `planificador.c` | Dongle requests, cooldown, allocation, and release. |
| `cola.c` | FIFO/EDF priority heap. |
| `monitor.c`, `state.c`, `log.c`, `time.c` | Burnout monitor, state, logging, and timing. |

## Resources

- [POSIX Threads Programming](https://hpc-tutorials.llnl.gov/posix/): introduction to threads, mutexes, and condition variables.
- [POSIX `pthread_cond_timedwait`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_cond_timedwait.html): timed-wait and condition-variable semantics.
- [POSIX `pthread_mutex_lock`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_mutex_lock.html): mutual exclusion and usage requirements.
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem): the classic concurrency problem that inspired dongle coordination.
- [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling): the EDF policy used by the project.

### AI usage

AI was used to review the repository and draft this documentation from the subject and existing source code. It was not used as a substitute for manually reviewing the concurrent logic; the README content was checked against the project files.
