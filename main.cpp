#include <iostream>
#include <mutex>
#include "barrier.h"
#include <cstdlib>
#include <pthread.h>
#include <pthread_impl.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <signal.h>
#include <sys/signal.h>

#define DEBUG

using namespace std;

struct thread_job {
  int* list;
  size_t size;
  //phase 1
  int* regularSample;
};

pthread_barrier mbarrier;
pthread_t* threads;

// global job list
thread_job **jobs;

// your job
thread_job *job;

size_t n;
size_t p;
size_t w;

int list[] = {73, 123, 13, 8, 9, 12, 7, 231, 39 ,2, 38, 39};

int compare(const void *l, const void *r) {
  return ( *(int*)l - *(int*)r );
}

void phase1(int list[], size_t size) {
  qsort(list, size, sizeof(int), compare);

  for (size_t i = 0 ; i < p ; i++) {

  }
}

void* psrs(void* arg) {

  thread_job *job = (thread_job*)arg;

  phase1(job->list, job->size);

  pthread_barrier_await(&mbarrier);

  return NULL;
}

int main(int argc, char* argv[]) {
  n = sizeof(list)/sizeof(int);
  p = std::stoi(argv[1]);
  w = n/p^2;

  // + 1, main thread
  pthread_barrier_init(&mbarrier, p+1);
  
  pthread_t threads[p];
  thread_job jobs[p];

  size_t sampleSize = n/p;

  for (size_t i = 0; i < p ; i++) {
    jobs[i].size = sampleSize;
    jobs[i].list = list + jobs[i].size * i;
    jobs[i].regularSample = new int[p];
    
    pthread_create(&threads[i], NULL, &psrs, &jobs[i]);
  }

  pthread_barrier_await(&mbarrier);

  for (size_t i = 0 ; i < n ; i ++) {
    std::cout << list[i] << " ";
  }


  for (size_t i = 0; i < p ; i++) {
    free(jobs[i].regularSample);
  }  

  pthread_barrier_destroy(&mbarrier);
  return 1;
}

