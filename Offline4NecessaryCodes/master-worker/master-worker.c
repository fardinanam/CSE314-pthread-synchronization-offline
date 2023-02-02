#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;

int *buffer;

pthread_mutex_t *m;
pthread_cond_t *empty, *fill;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
  {
    if(item_to_produce >= total_items) {
      break;
    }

    pthread_mutex_lock(m);

    while (curr_buf_size == max_buf_size){
      pthread_cond_wait(empty, m);
    }

    // it's required to check again if all the items have been produced
    // after lock has been acquired because, it might happen that 
    // the first if statement has been passed when there is only one item
    // to produce. But while waiting for mutex lock, a producer has already
    // produced it. So, without checking it again before producing might 
    // cause producing more items than expected.
    if (item_to_produce >= total_items) {
      pthread_mutex_unlock(m);
      return 0;
    }

    buffer[curr_buf_size++] = item_to_produce;
    print_produced(item_to_produce, thread_id);
    item_to_produce++;
    pthread_cond_signal(fill);

    pthread_mutex_unlock(m);
  }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *remove_requests_loop(void *data) 
{
  int thread_id = *((int *)data);
  
  while (1) {
    if (item_to_produce >= total_items && curr_buf_size == 0) 
      break;
    
    pthread_mutex_lock(m);

    while (curr_buf_size == 0) {
      // It is required to check if all the items have already been produced
      // before waiting for any producer. If we don't do this then the 
      // consumer will wait forever for a producer to produce something
      // while there is nothing left to produce. 
      if (item_to_produce >= total_items) {
        pthread_mutex_unlock(m);
        return 0;
      }

      pthread_cond_wait(fill, m);
    }
    
    int item_to_consume = buffer[--curr_buf_size];
    print_consumed(item_to_consume, thread_id);
    pthread_cond_signal(empty);

    pthread_mutex_unlock(m);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  int *master_thread_id, *worker_thread_id;
  pthread_t *master_thread, *worker_thread;
  item_to_produce = 0;
  curr_buf_size = 0;
  
  int i;
  
  if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    

  buffer = (int *)malloc (sizeof(int) * max_buf_size);

  // create mutex and csv instances
  m = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  empty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  fill = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));

  //create master producer threads
  master_thread_id = (int *)malloc(sizeof(int) * num_masters);
  master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++) 
    worker_thread_id[i] = i;
  
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, remove_requests_loop, (void *)&worker_thread_id[i]);
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
  {
    pthread_join(master_thread[i], NULL);
    printf("master %d joined\n", i);
  }

  for (i = 0; i < num_workers; i++) {
    pthread_join(worker_thread[i], NULL);
    printf("worker %d joined\n", i);
  }
  
  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(worker_thread);
  free(master_thread);
  free(worker_thread_id);
  free(master_thread_id);
  free(m);
  free(empty);
  free(fill);
  
  return 0;
}
