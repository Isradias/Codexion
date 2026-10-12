*This project has been created as part of the 42 curriculum by icaldas-.*

# Codexion

## Description

Codexion is a POSIX-thread simulation of coders sharing USB dongles in a circular
workspace. Each coder needs two adjacent dongles to compile, then alternates between
debugging and refactoring. A coder burns out if they do not start a new compilation
before their deadline.

The program supports FIFO and Earliest Deadline First (EDF) arbitration. It is being
implemented incrementally with local Git commits and is never pushed from this
repository.

## Instructions

Build the project:

```sh
make
```

Run it with eight mandatory arguments:

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
  time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Example:

```sh
./codexion 4 800 200 100 100 3 20 edf
```

`scheduler` must be either `fifo` or `edf`. All time values are expressed in
milliseconds.

## Architecture

The simulation is organized around a shared state object. It owns coder and dongle
arrays, synchronization primitives, the scheduler queue, the simulation start time,
and shutdown state. Coders are threads; the monitor is a separate thread.

The intended module boundaries are parsing, initialization and cleanup, time helpers,
logging, priority heap, arbitration, coder lifecycle, and monitoring. The source is
being refactored to keep every module compliant with the 42 Norm.

## Thread flow

1. A coder requests its two adjacent dongles.
2. The scheduler selects an eligible request.
3. The coder acquires both dongles, logs both acquisitions, and starts compiling.
4. It releases both dongles and starts their cooldown.
5. It logs and performs debugging, then refactoring, before requesting dongles again.
6. The monitor ends the simulation on burnout or after every coder meets the required
   compilation count.

## Scheduling policies

### FIFO

FIFO orders pending requests by their monotonic arrival sequence. Requests with an
earlier sequence number are considered first.

### EDF

EDF orders pending requests by the deadline
`last_compile_start + time_to_burnout`. Equal deadlines use arrival sequence as a
deterministic tie breaker. The priority queue is implemented manually as a binary heap;
no library priority queue is used.

## Blocking cases handled

The final implementation addresses the following concurrency hazards:

- Deadlock: both dongles are granted together by one arbitration decision, so a coder
  never holds one dongle while waiting indefinitely for the other. This removes
  circular wait, one of Coffman's necessary deadlock conditions.
- Mutual exclusion: each dongle state is protected by its own mutex.
- Starvation: FIFO preserves request order; EDF orders by imminent deadline and uses a
  stable tie breaker. The scheduler re-evaluates on releases and cooldown expiration.
- Cooldown: a released dongle has an unavailable-until timestamp and cannot be granted
  before it expires.
- Burnout: the monitor waits for the nearest deadline rather than polling at a coarse
  interval, allowing the burnout message to meet the required 10 ms precision.
- Output interleaving: a dedicated log mutex serializes complete log lines.
- Shutdown: a shared stop flag and condition-variable broadcasts wake waiting coders
  before resources are destroyed.

## Thread synchronization mechanisms

`pthread_mutex_t` protects individual dongles, shared scheduler state, and output.
`pthread_cond_t` lets coders wait without busy looping when the queue head cannot yet
receive both dongles, and lets the monitor and scheduler wake waiters on shutdown,
resource release, or cooldown expiry.

The state mutex is acquired before examining or changing scheduler state. A dongle
mutex is acquired only while changing that dongle's protected fields. Logging does not
hold resource mutexes. These rules prevent conflicting lock-order paths and keep state
transitions coherent.

## Precision and cleanup

Timestamps use millisecond wall-clock time. Timed waits use the clock compatible with
their condition variable. Threads are joined before mutexes, condition variables, and
heap-backed arrays are destroyed. Allocation and initialization failures are cleaned up
only for resources that were successfully created.

## Resources

- [POSIX Threads](https://pubs.opengroup.org/onlinepubs/9799919799.2024edition/)
- [pthread mutexes](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [pthread condition variables](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

### AI use

AI assistance was used to help plan the module structure, review synchronization
invariants, draft this documentation, and propose test cases. The implementation and
all generated code are reviewed, compiled, tested, and understood by the author before
being retained.

## Current status

Argument validation and the initial concurrent lifecycle scaffold are present. The
simulation module is currently being refactored for full 42 Norm compliance before FIFO
and EDF heap arbitration are completed. Claims in this README about the final scheduler
describe the intended design, not completed validation results.
