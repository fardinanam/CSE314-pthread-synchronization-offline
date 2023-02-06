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

To test the code, use [**`test-reader-pref.cpp`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/test-reader-pref.cpp).

To compile and run the code, use the following commands:

```bash
    g++ rwlock-reader-pref.cpp -o rwlock-reader-pref.o -lpthread
    ./rwlock-reader-pref.o <number-of-readers> <number-of-writers>
```
- [**`read-write-lock with writer preference`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/rwlock-writer-pref.cpp)

To test the code, use  [**`test-writer-pref.cpp`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/rwlock/rwlock-writer-pref.cpp) 

To compile and run the code, use the following commands:

```bash
    g++ rwlock-writer-pref.cpp -o rwlock-writer-pref.o -lpthread
    ./rwlock-writer-pref.o <number-of-readers> <number-of-writers>
```

## Solution to Part C : Zemaphore
- [**`zemaphore.c`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/zemaphore/zemaphore.c)

To test the code, use [**`test-zem.c`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/zemaphore/zemaphore.c).

To compile and run the code, use the following commands:

```bash
    gcc zemaphore.c test-zem.c -o test-zem.o -lpthread
    ./test-zem.o
```

## Solution to Part D : Server-Client-Database
### Problem Statement:
There are multiple databases and one server. More than one client can connect to the server at a time and can request access to a database. The request will only be granted by the server if there is no other client accessing the requested database. 

### Solutions:
- [**`server-client-database with conditional variables`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/serverclient/cv-server-client-database.cpp)
```bash
    g++ cv-server-client-database.cpp -o cv-server-client-database.out -lpthread
    ./cv-server-client-database.out
```

- [**`server-client-database with zemaphores`**](https://github.com/fardinanam/CSE314-pthread-synchronization-offline/blob/master/Offline4NecessaryCodes/serverclient/zema-server-client-database.cpp)

```bash
    g++ zemaphore.c zema-server-client-database.cpp -o zema-server-client-database.out -lpthread
    ./zema-server-client-database.out
```