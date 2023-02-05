#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw) {
  rw->reader_count = 0;
  pthread_mutex_init(&rw->reader_count_lock, NULL);
  pthread_mutex_init(&rw->rwlock, NULL);
}

void ReaderLock(struct read_write_lock * rw) {
  pthread_mutex_lock(&rw->reader_count_lock);
  if (rw->reader_count == 0) {
    pthread_mutex_lock(&rw->rwlock);
  }

  rw->reader_count++;
  pthread_mutex_unlock(&rw->reader_count_lock);
}

void ReaderUnlock(struct read_write_lock * rw) {
  pthread_mutex_lock(&rw->reader_count_lock);
  if (rw->reader_count > 0) {
    rw->reader_count--;

    if (rw->reader_count == 0) {
      pthread_mutex_unlock(&rw->rwlock);
    }
  }
  pthread_mutex_unlock(&rw->reader_count_lock);
}

void WriterLock(struct read_write_lock * rw) {
  pthread_mutex_lock(&rw->rwlock);
}

void WriterUnlock(struct read_write_lock * rw) {
  pthread_mutex_unlock(&rw->rwlock);
}
