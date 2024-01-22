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
  //
  int* regularSample;
};

pthread_barrier mbarrier;
pthread_t* threads;

// global job list
thread_job *jobs = nullptr;

// your job
thread_job *job = nullptr;

size_t n,p,w;

int list[] = {16, 2, 17, 24, 33, 28, 30, 1, 0, 27, 9, 25, 34, 23, 19, 18, 11, 7, 21, 13, 8, 35, 12, 29, 6, 3, 4, 14, 22, 15, 32, 10, 26, 31, 20, 5};

int compare(const void *l, const void *r) {
  return ( *(int*)l - *(int*)r );
}

int* phase1(int list[], size_t size) {
  qsort(list, size, sizeof(int), compare);
  
  int* sample = new int[p];

  for (size_t i = 0 ; i < p; i++) {
    sample[i] = list[i*w];
  }

  return sample;
}

void* psrs(void* arg) {
  thread_job *job = (thread_job*)arg;
  job->regularSample = phase1(job->list, job->size);
  pthread_barrier_await(&mbarrier);
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  n = sizeof(list)/sizeof(int);
  p = std::stoi(argv[1]);
  w = n/(pow(p,2));

  std::cout << w << std::endl;

  // + 1, main thread
  cout<< "Barrier status " << pthread_barrier_init(&mbarrier, p+1) << std::endl;
  
  threads = new pthread_t[p];
  jobs = new thread_job[p];

  size_t sampleSize = n/p;

  for (size_t i = 0; i < p ; i++) {
    jobs[i].size = sampleSize;
    jobs[i].list = &list[sampleSize*i];
    jobs[i].regularSample = new int[p];

    for (size_t x = 0 ; x < sampleSize ; x++) {
      std::cout << jobs[i].list[x] << " ";
    }
    std::cout << endl;
    
    pthread_create(&threads[i], NULL, psrs, &jobs[i]);
  }

  pthread_barrier_await(&mbarrier);
  std::cout << "printing" << std::endl;
  for (size_t i = 0; i < p ; i++) {
    for (size_t x = 0; x < p ; x++) {
      std::cout << jobs[i].regularSample[x] << " ";
      if (x+1 >= p) std::cout << "\n";
    }
    free(jobs[i].regularSample);
  }

  for (size_t i = 0 ; i < n ; i ++) {
    std::cout << list[i] << " ";
  }

  std::cout << "\n";

  pthread_barrier_destroy(&mbarrier);

  free(threads);
  free(jobs);

  return 1;
}

