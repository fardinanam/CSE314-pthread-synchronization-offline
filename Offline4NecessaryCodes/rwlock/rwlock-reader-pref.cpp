#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw) {
  rw->reader_count = 0;
}

void ReaderLock(struct read_write_lock * rw) {
  if (rw->reader_count == 0) {
    pthread_mutex_lock(&rw->rwlock);
  }

  rw->reader_count++;
}

void ReaderUnlock(struct read_write_lock * rw) {
  if (rw->reader_count > 0) {
    rw->reader_count--;

    if (rw->reader_count == 0) {
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
