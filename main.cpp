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
  size_t index;
  
  size_t* partitions;
};

pthread_barrier mbarrier;
pthread_t* threads;

// global job list
thread_job *jobs = nullptr;

size_t n,p,w;

// what each processor will pivot off of
int* pivots;

int list[] = {16, 2, 17, 24, 33, 28, 30, 1, 0, 27, 9, 25, 34, 23, 19, 18, 11, 7, 21, 13, 8, 35, 12, 29, 6, 3, 4, 14, 22, 15, 32, 10, 26, 31, 20, 5};

int compare(const void *l, const void *r) {
  return ( *(int*)l - *(int*)r );
}

int* phase1(int list[], const size_t size, int sample[]) {
  qsort(list, size, sizeof(int), compare);

  for (size_t i = 0 ; i < p; i++) {
    sample[i] = list[i*w];
  }

  return sample;
}

int* phase2(int gatheredSample[], size_t p) {
  qsort(gatheredSample, p*p, sizeof(int), compare);

  int* pivotList = new int[p-1];

  for (size_t i = 0; i < p-1 ; i++) {
    pivotList[i] = gatheredSample[i*p + p];
  }

  return pivotList;
}

void* psrs(void* arg) {
  thread_job *job = (thread_job*)arg;
  job->regularSample = phase1(job->list, job->size, job->regularSample);
  // phase 1 finished
  pthread_barrier_await(&mbarrier);

  // phase 2, main processor sorting regular sample
  pthread_barrier_await(&mbarrier);

  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  n = sizeof(list)/sizeof(int);
  p = std::stoi(argv[1]);
  w = n/(pow(p,2));

  std::cout << w << std::endl;

  // + 1, main thread
  pthread_barrier_init(&mbarrier, p+1);
  
  threads = new pthread_t[p];
  jobs = new thread_job[p];

  size_t sampleSize = n/p;

  int* gatheredSample = new int[p * p];

  for (size_t i = 0; i < p ; i++) {
    jobs[i].size = sampleSize;
    jobs[i].list = &list[sampleSize*i];
    jobs[i].index = i;
    jobs[i].regularSample = &gatheredSample[p*i];
    
    pthread_create(&threads[i], NULL, psrs, &jobs[i]);
  }

  // phase 1 end
  pthread_barrier_await(&mbarrier);
  // phase 2 start
  
  pivots = phase2(gatheredSample, p);

  std::cout << "SORTED GATHERED SAMPLE\n";
  for (size_t i = 0 ; i < p ; i++) {
    for(size_t x = 0 ; x < p ; x++) {
      std::cout << gatheredSample[i*p + x] << " ";
    }
  }

  std::cout << "\nPIVOTS AT" << std::endl;
  for (size_t i = 0 ; i < p-1 ;  i ++) {
    std::cout << pivots[i] << " ";
  }
  std::cout << std::endl;

  // phase 2 end
  pthread_barrier_await(&mbarrier);

  // phase 3, start
  // pthread_barrier_await(&mbarrier);
  // phase 3 end, time to merge
  
  for (size_t i = 0 ; i < n ; i ++) {
    std::cout << list[i] << " ";
  }

  pthread_barrier_destroy(&mbarrier);

  free(gatheredSample);
  free(pivots);
  free(threads);
  free(jobs);

  return 1;
}

