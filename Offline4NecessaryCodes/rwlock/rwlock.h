#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

struct read_write_lock
{
    pthread_mutex_t rwlock;
    pthread_mutex_t readerCountLock;
    pthread_mutex_t writerCountLock;
    pthread_cond_t finishWriter;
    pthread_cond_t finishReader;
    int readerCount;
    int writerCount;
};

void InitalizeReadWriteLock(struct read_write_lock * rw);
void ReaderLock(struct read_write_lock * rw);
void ReaderUnlock(struct read_write_lock * rw);
void WriterLock(struct read_write_lock * rw);
void WriterUnlock(struct read_write_lock * rw);
