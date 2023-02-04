#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw) {
  rw->readerCount = 0;
}

void ReaderLock(struct read_write_lock * rw) {
  if (rw->readerCount == 0) {
    pthread_mutex_lock(&rw->rwlock);
  }

  rw->readerCount++;
}

void ReaderUnlock(struct read_write_lock * rw) {
  if (rw->readerCount > 0) {
    rw->readerCount--;

    if (rw->readerCount == 0) {
      pthread_mutex_unlock(&rw->rwlock);
    }
  }
}

void WriterLock(struct read_write_lock * rw) {
  pthread_mutex_lock(&rw->rwlock);
}

void WriterUnlock(struct read_write_lock * rw) {
  pthread_mutex_unlock(&rw->rwlock);
}
