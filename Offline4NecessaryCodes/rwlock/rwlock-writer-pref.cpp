#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw) {
  // printf("InitalizeReadWriteLock\n");
  rw->readerCount = 0;
  rw->writerCount = 0;
}

void ReaderLock(struct read_write_lock * rw) {
  // printf("ReaderLock: %d\n", rw->readerCount);

  pthread_mutex_lock(&rw->writerCountLock);
  
  while (rw->writerCount > 0) {
    // pthread_mutex_unlock(&rw->readerCountLock);
    pthread_cond_wait(&rw->finishWriter, &rw->writerCountLock);
    // printf("ReaderLock: waiting for writer to finish\n");
  } 

  pthread_mutex_unlock(&rw->writerCountLock);
  
  pthread_mutex_lock(&rw->readerCountLock);

  if (rw->readerCount == 0)
    pthread_mutex_lock(&rw->rwlock);
    
  rw->readerCount++;

  pthread_mutex_unlock(&rw->readerCountLock);
}

void ReaderUnlock(struct read_write_lock * rw) {
  // printf("ReaderUnlock: %d\n", rw->readerCount);

  pthread_mutex_lock(&rw->readerCountLock);
  
  if (rw->readerCount == 1) {
    pthread_mutex_unlock(&rw->rwlock);
  }

  rw->readerCount--;
  pthread_mutex_unlock(&rw->readerCountLock);
}

void WriterLock(struct read_write_lock * rw)  {
  // printf("WriterLock: %d\n", rw->writerCount);

  pthread_mutex_lock(&rw->writerCountLock);
  rw->writerCount++;
  pthread_mutex_unlock(&rw->writerCountLock);

  pthread_mutex_lock(&rw->rwlock);
}

void WriterUnlock(struct read_write_lock * rw) {
  // printf("WriterUnlock: %d\n", rw->writerCount);

  pthread_mutex_lock(&rw->writerCountLock);
  
  if (rw->writerCount == 1) {
    pthread_cond_broadcast(&rw->finishWriter);
  }
  rw->writerCount--;

  pthread_mutex_unlock(&rw->writerCountLock);

  pthread_mutex_unlock(&rw->rwlock);
}
