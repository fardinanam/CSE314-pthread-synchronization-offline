#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw) {
  rw->reader_count = 0;
  rw->writer_count = 0;
}

void ReaderLock(struct read_write_lock * rw) {
  // wait for all writers to finish
  pthread_mutex_lock(&rw->writer_count_lock);
  while (rw->writer_count > 0) {
    pthread_cond_wait(&rw->finish_writer_cond, &rw->writer_count_lock);
  } 
  pthread_mutex_unlock(&rw->writer_count_lock);
  
  // acquire the lock if no other reader is reading
  // otherwise, just increment the reader count assuming
  // the lock is already acquired
  pthread_mutex_lock(&rw->reader_count_lock);
  if (rw->reader_count == 0)
    pthread_mutex_lock(&rw->rwlock);

  rw->reader_count++;
  pthread_mutex_unlock(&rw->reader_count_lock);
}

void ReaderUnlock(struct read_write_lock * rw) {
  // printf("ReaderUnlock: %d\n", rw->reader_count);

  // release the lock if this is the last reader
  pthread_mutex_lock(&rw->reader_count_lock);
  if (rw->reader_count == 1)
    pthread_mutex_unlock(&rw->rwlock);

  rw->reader_count--;
  pthread_mutex_unlock(&rw->reader_count_lock);
}

void WriterLock(struct read_write_lock * rw)  {
  // printf("WriterLock: %d\n", rw->writer_count);

  // increment the writer count
  pthread_mutex_lock(&rw->writer_count_lock);
  rw->writer_count++;
  pthread_mutex_unlock(&rw->writer_count_lock);

  // acquire the lock
  pthread_mutex_lock(&rw->rwlock);
}

void WriterUnlock(struct read_write_lock * rw) {
  // printf("WriterUnlock: %d\n", rw->writer_count);

  // if this is the last writer, broadcast to all readers
  // that they can now acquire the lock
  pthread_mutex_lock(&rw->writer_count_lock);
  if (rw->writer_count == 1) {
    pthread_cond_broadcast(&rw->finish_writer_cond);
  }
  rw->writer_count--;
  pthread_mutex_unlock(&rw->writer_count_lock);

  // release the lock
  pthread_mutex_unlock(&rw->rwlock);
}
