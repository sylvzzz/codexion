*This project has been created as part of the 42 curriculum by <a href="https://github.com/sylvzzz">dbotelho</a>.*

# Codexion

A concurrent programming project in C. Coders race against burnout while fighting for a few USB dongles. It uses POSIX threads, mutexes, condition variables, and FIFO/EDF scheduling to share resources, avoid deadlocks, and keep access fair under either scheduler.

## Description

Codexion sets up a circular coworking space where several coders share one Quantum Compiler. To compile, a coder has to hold **two USB dongles at the same time**, one in each hand. Coders go through three phases: compiling, debugging, and refactoring. If a coder waits too long to compile again, it `burns out`.

Each coder is a POSIX thread. Dongles sit between pairs of coders, one per pair, so each coder has a left and a right dongle. There is a single dongle for the single-coder case. Since the dongles are scarce and shared, coders need to coordinate safely. A separate **monitor thread** watches the coders: it sees when one burns out (a coder that does not start compiling within `time_to_burnout` ms after its last compile) and when every coder has reached `number_of_compiles_required`, and then it ends the simulation.

The simulation takes 8 arguments, including `dongle_cooldown` (a dongle stays unused for a while after being released) and a scheduler policy, `fifo` (first in, first out) or `edf` (earliest deadline first). Arbitration runs through a hand-built priority queue (a binary heap).

## Instructions

### Compilation

The project uses a Makefile at the repo root:

```sh
make          # builds ./codexion
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # rebuilds from scratch
```

You need a C compiler and the `-Wall -Wextra -Werror -pthread` flags.

### Run

```sh
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

All 7 parameters plus the scheduler string are required. All times are in milliseconds. The scheduler must be exactly `fifo` or `edf`.

### Example

```sh
./codexion 3 200 10 10 10 2 0 fifo
```

```text
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
10 1 is debugging
...
```

The simulation stops when a coder burns out or when every coder has compiled at least `number_of_compiles_required` times.

## Blocking cases handled

These are the concurrency problems the code deals with:

- **Deadlocks (hold-and-wait).** The naive approach is to take the left dongle and then the right one. That can leave two neighbours sitting with one dongle each, waiting for the other to free theirs. Codexion avoids this by only letting a coder grab **both** dongles at once: `can_grab` checks that both dongles are free and out of cooldown before the grab, so a coder never holds one dongle while waiting for another. This removes the hold-and-wait condition (one of Coffman's conditions) and breaks any circular wait.
- **Atomic double-grab.** The code grabs both dongles while holding a single arbitration mutex, so the pair is reserved together. No other coder can slip in and see half a grab.
- **Cooldown handling.** After a coder releases a dongle, the code sets `available_at_ms` to `now + dongle_cooldown`, and `can_grab` refuses a dongle whose cooldown has not passed. A coder blocked on a dongle in cooldown sleeps with a timed wait and checks again, so it never gets stuck waiting for time to pass.
- **Starvation and fairness.** The `fifo`/`edf` scheduler is a binary heap priority queue. FIFO order is recorded as a monotonically increasing `request_order` the moment a coder joins the wait queue (true arrival order). With FIFO, the earlier arrival wins; with EDF, the coder with the earliest burnout deadline wins. Each coder's `heap_pos` tracks whether it is waiting, so no coder is skipped quietly. A coder only blocks a neighbour on a shared dongle if it could actually lock that dongle's pair right now (`other_can_hold_pair`) — speculative waiting never stalls a grant that another coder can take.
- **Burnout detection.** A dedicated monitor thread polls the coders and marks one burned out as soon as `now >= last_compile_start + time_to_burnout`. It prints the burnout message quickly, within the 10 ms the subject allows.
- **Log serialization.** All state messages are printed under a logging mutex (`log_lock`), so two lines never mix.
- **Impossible parameter sets.** When the workload simply cannot be met (for example, `time_to_compile` much longer than `time_to_burnout`), coders burn out and the simulation ends; it does not hang.
- **Cooldown-serialized rings (solved).** On a ring of coders with `dongle_cooldown` > 0, granting dongles strictly one coder at a time leaves the time between two compiles of the same coder at `number_of_coders × (time_to_compile + dongle_cooldown)`. For `5 × 600 ms = 3000 ms` that is exactly `time_to_burnout`, a knife edge. Codexion avoids it by granting **disjoint pairs at the same time** (e.g. coders 1 and 3 hold separate dongles simultaneously), so each coder's period stays well under `time_to_burnout` and `./codexion 5 3000 200 200 200 10 400 fifo` completes in every run. The monitor also never burns a coder that is mid-compile. With an extreme cooldown (`800` ms) FIFO's effective ring period can still exceed `time_to_burnout` — that is inherent to strict arrival order under such a cooldown (EDF, which serves the most urgent coder first, completes).
- **Invalid input.** `parse_args` rejects the wrong number of arguments, non-integers, negative numbers, and any scheduler other than `fifo`/`edf`, and exits with a clear error.

## Thread synchronization mechanisms

Codexion uses these POSIX threading primitives, all built around the shared state in `t_sim`:

- **`pthread_mutex_t`, `arbitration_lock`.** Serializes all arbitration: the wait queue (binary heap), `pq_push`/`pq_remove`, `can_grab`, and `grab_dongles`. Only one thread changes the heap and dongle state at a time, so nothing gets corrupted. It is also used in `compile_phase` to release the dongles and set cooldowns.
- **`pthread_mutex_t`, `state_lock`.** Protects per-coder fields that the monitor reads (`state`, `compiles_done`, `last_compile_start_ms`). The monitor uses these to detect burnout or completion, the coders update them, and the lock stops them from racing.
- **`pthread_mutex_t`, `log_lock`.** Keeps output from interleaving. Every state message (`has taken a dongle`, `is compiling`, and so on) is printed under this lock.
- **`pthread_mutex_t`, `stop_lock`.** Guards the `stop` flag. The monitor and the coders both read and write it (`sim_should_stop`) without racing.
- **`pthread_cond_t`, `arbitration_cond`.** With `pthread_cond_timedwait`, a waiting coder sleeps until it can try to grab again, and `pthread_cond_broadcast` wakes all waiters whenever a coder finishes compiling and releases its dongles. The 1 ms timed wait also makes sure a coder never gets stuck on a dongle that only becomes free once the cooldown passes, since releasing that dongle sends no broadcast.

### How race conditions are prevented

No shared state is ever changed outside a lock:

- Heap and dongles: `arbitration_lock`
- Coder lifecycle fields: `state_lock`
- Log output: `log_lock`
- Stop flag: `stop_lock`

Every writer holds the matching lock before touching shared memory, and the heap is only read under `arbitration_lock`, so the accesses never overlap and no data race can happen.

The code was checked with `valgrind --tool=drd`: a contended run (all five coders racing for the dongles) reports `0 errors from 0 contexts`. `helgrind` reports a "pthread_cond broadcast: associated lock not held" warning pointing only at `wait_for_dongles`'s 1 ms `pthread_cond_timedwait`; this is a known helgrind false positive with glibc 2.34's adaptive condition variables under timed waits (no stack ever reaches our `compile_phase` broadcast, which holds `arbitration_lock`), and DRD, which shares no code with helgrind, reports nothing.

### Thread-safe communication between coders and the monitor

Coders and the monitor never send each other messages. They talk through protected shared state: coders update `state` and `compiles_done` under `state_lock`, and the monitor reads those same fields under the same lock to decide on burnout or completion. That handoff is thread-safe: no message passing, just shared memory guarded by mutexes.

## Resources

- POSIX Threads Programming, https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2 - was 99% of the way to this project, this playlist is really good
- Man pages: `pthread_create(3)`, `pthread_mutex_lock(3)`, `pthread_cond_timedwait(3)`, `gettimeofday(2)`
- Opencode

### How AI was used

AI was used as a study companion for this project. It helped review the concurrency ideas (mutexes, condition variables, deadlock avoidance, the FIFO/EDF policies), plan the priority-queue arbitration, and look for race conditions and edge cases. Every decision was reviewed, and the code was run and tested against the subject.