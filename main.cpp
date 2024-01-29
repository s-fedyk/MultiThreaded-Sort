#include <algorithm>
#include <iostream>
#include <mutex>
#include "barrier.h"
#include "BinarySearch.h"
#include <cstdlib>
#include <pthread.h>
#include <pthread_impl.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <signal.h>
#include <sys/signal.h>

#define DEBUG

#define MASTER if (job->index == p-1)

using namespace std;


struct sample_partition {
  int* base;
  size_t size;
};

struct thread_job {
  //phase 1
  size_t index;
  
  // each thread has p partitions which it builds from pivots
  sample_partition *partitions;
};

pthread_barrier mbarrier;
pthread_t* threads;

// global job list
thread_job *jobs = nullptr;

size_t n,p,w, sampleSize;

// what each processor will pivot off of
int* pivots;

// threads contribute to this
int* gatheredSample;

int list[] = {16, 2, 17, 24, 33, 28, 30, 1, 0, 27, 9, 25, 34, 23, 19, 18, 11, 7, 21, 13, 8, 35, 12, 29, 6, 3, 4, 14, 22, 15, 32, 10, 26, 31, 20, 5};

// where our result goes
int* dest;

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

sample_partition* phase3(int list[], const size_t size, int pivots[], size_t p) {
  sample_partition *partitions = new sample_partition[p];
  int offset = 0;
  for (size_t i = 0 ; i < p-1 ; i++) {
    partitions[i].base = &list[offset];
    partitions[i].size = binarySearch(list, size, pivots[i]) - offset;
    offset = offset + partitions[i].size;
  }

  partitions[p-1].base = &list[offset];
  partitions[p-1].size = size - offset;

  return partitions;
}

// use sample partitions to merge
void phase4() {
  size_t resultIndex = 0;

  for (size_t partitionIndex = 0 ; partitionIndex < p ; partitionIndex ++) {

    int minThread = -1; 
    size_t partitionSize = 0;

    sample_partition toMerge[p];

    std::cout << std::endl;
    std::cout << "printing partition " << partitionIndex << std::endl;
    for (size_t threadIndex = 0 ; threadIndex < p ; threadIndex++) {
      toMerge[threadIndex] = jobs[threadIndex].partitions[partitionIndex];
      for (size_t i = 0 ; i < toMerge[threadIndex].size ; i ++) {
        std::cout << toMerge[threadIndex].base[i] << " ";
      }
      partitionSize += toMerge[threadIndex].size;
    }

    size_t indexes[p]; // we are merging this many partitions
    memset(indexes, 0x0, p*sizeof(size_t));

    for (size_t i = 0 ; i < partitionSize ; i++) {
      std::cout << i << std::endl;
      minThread = -1;
      for (size_t threadIndex = 0 ; threadIndex < p ; threadIndex++) {
        // fully merged this sublist
        if (toMerge[threadIndex].size == indexes[threadIndex]) continue;
  
        int currentObservation = toMerge[threadIndex].base[indexes[threadIndex]];
        
        // no minimum, first thread we've seen
        if (minThread < 0) { 
          minThread = threadIndex;
        } else {
          int currentMin = toMerge[minThread].base[indexes[minThread]];
          if (currentObservation < currentMin) {
            minThread = threadIndex;
          }

        }
      }
  
      if (minThread >= 0) {
        dest[resultIndex] = toMerge[minThread].base[indexes[minThread]];
        resultIndex++;
        indexes[minThread]++;
      }
    }
  }
}

void* psrs(void* arg) {
  thread_job *job = (thread_job*)arg;
  phase1(&list[sampleSize * job->index], sampleSize, &gatheredSample[job->index * p]);

  // phase 1 finished
  pthread_barrier_await(&mbarrier);

  MASTER {
    pivots = phase2(gatheredSample, p);
  }

  pthread_barrier_await(&mbarrier);

  job->partitions = phase3(&list[sampleSize * job->index], sampleSize, pivots, p);

  pthread_barrier_await(&mbarrier);

  MASTER {
    phase4();


    for (size_t i = 0 ; i < n ; i++) {
      std::cout << dest[i] << " ";
    }
    std::cout << std::endl;

    free(dest);
    free(gatheredSample);
    free(pivots);
    free(threads);
    free(jobs);
  }

  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

  n = sizeof(list)/sizeof(int);
  p = std::stoi(argv[1]);
  w = n/(pow(p,2));
  sampleSize = n/p;

  dest = new int[n];

  // + 1, main thread
  pthread_barrier_init(&mbarrier, p);
  
  threads = new pthread_t[p];
  jobs = new thread_job[p];
  gatheredSample = new int[p * p];

  for (size_t i = 0; i < p-1 ; i++) {
    jobs[i].index = i;
    pthread_create(&threads[i], NULL, psrs, &jobs[i]);
  }

  jobs[p-1].index = p-1;
  psrs(&jobs[p-1]);

  return 1;
}
