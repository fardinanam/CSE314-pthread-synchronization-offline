# CSE314 Operating Systems (Offline 4)

**[Problem Statement](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Specifications/314_Pthread_Lab_Assignment.pdf)**

## Solution to Part A : Master-Worker Thread Pool

The solution code is in [**`master-worker.c`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/master-worker/master-worker.c)

To run the code, use the following command:

```bash
gcc master-worker.c -o master-worker.o -lpthread
./master-worker.o <number-of-items> <max-buffer-size> <number-of-worker> <number-of-producer>
```