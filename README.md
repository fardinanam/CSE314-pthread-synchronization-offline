# CSE314 Operating Systems (Offline 4)

**[Problem Statement](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Specifications/314_Pthread_Lab_Assignment.pdf)**

## Solution to Part A : Master-Worker Thread Pool

The solution code is in [**`master-worker.c`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/master-worker/master-worker.c)

To run the code, use the following command:

```bash
gcc master-worker.c -o master-worker.o -lpthread
./master-worker.o <number-of-items> <max-buffer-size> <number-of-worker> <number-of-producer>
```

## Solution to Part B : Reader-Writer Problem
- [**`read-write-lock with reader preference`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/rwlock-reader-pref.cpp)
- [**`read-write-lock with writer preference`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/rwlock-writer-pref.cpp)

To test the codes, use [**`test-reader-pref.cpp`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/test-reader-pref.cpp) and [**`test-writer-pref.cpp`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/rwlock-writer-pref.cpp) respectively.

To compile and run the codes, use the following commands:

```bash
    g++ rwlock-reader-pref.cpp -o rwlock-reader-pref.o -lpthread
    ./rwlock-reader-pref.o <number-of-readers> <number-of-writers>
```

```bash
    g++ rwlock-writer-pref.cpp -o rwlock-writer-pref.o -lpthread
    ./rwlock-writer-pref.o <number-of-readers> <number-of-writers>
```