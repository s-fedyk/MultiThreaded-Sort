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

    size_t minThread = 0; 
    size_t partitionSize = 0;

    sample_partition toMerge[p];

    for (size_t threadIndex = 0 ; threadIndex < p ; threadIndex++) {
      toMerge[threadIndex] = jobs[threadIndex].partitions[partitionIndex];

      partitionSize += toMerge[threadIndex].size;
    }

    size_t indexes[p]; // we are merging this many partitions
    memset(indexes, 0x0, p*sizeof(size_t));

    for (size_t i = 0 ; i < partitionSize ; i++) {
      minThread = 0;
      for (size_t threadIndex = 0 ; threadIndex < p ; threadIndex++) {
        // fully merged this sublist
        if (toMerge[threadIndex].size == indexes[threadIndex]) continue;

        // the value of the current minimum value
        int currentMin = toMerge[minThread].base[indexes[minThread]];
        int currentObservation = toMerge[threadIndex].base[indexes[threadIndex]];

        if (currentObservation < currentMin) {
          minThread = threadIndex;
        }
      }

      dest[resultIndex] = toMerge[minThread].base[indexes[minThread]];
      resultIndex++;
      indexes[minThread]++;
    }
  }
}

void* psrs(void* arg) {
  thread_job *job = (thread_job*)arg;
  phase1(&list[sampleSize * job->index], sampleSize, &gatheredSample[job->index * p]);

  // phase 1 finished
  pthread_barrier_await(&mbarrier);

  // phase 2, main processor sorting regular sample
  pthread_barrier_await(&mbarrier);

  job->partitions = phase3(&list[sampleSize * job->index], sampleSize, pivots, p);

  pthread_barrier_await(&mbarrier);

  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

  n = sizeof(list)/sizeof(int);
  p = std::stoi(argv[1]);
  w = n/(pow(p,2));
  sampleSize = n/p;

  dest = new int[n];

  // + 1, main thread
  pthread_barrier_init(&mbarrier, p+1);
  
  threads = new pthread_t[p];
  jobs = new thread_job[p];
  gatheredSample = new int[p * p];

  for (size_t i = 0; i < p ; i++) {
    jobs[i].index = i;
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
    std::cout << pivots[i] << " | ";
  }
  std::cout << std::endl;

  // phase 2 end
  pthread_barrier_await(&mbarrier);
  // phase 3, start
  pthread_barrier_await(&mbarrier);
  // phase 3 end, time to merge
  for (size_t i = 0 ; i < p ; i ++) {
    thread_job currentJob = jobs[i];
    std::cout << "Thread " << i << std::endl;
    for (size_t x = 0 ; x < p ; x++) {
      sample_partition currentPartition = currentJob.partitions[x];
      
      std::cout << "Partition " << x << std::endl;

      for (size_t z = 0 ; z < currentPartition.size ; z++) {
        std::cout << currentPartition.base[z] << " ";
      }
      
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;

  pthread_barrier_destroy(&mbarrier);

  phase4();

  for (size_t i = 0 ; i < n ; i++) {
    std::cout << dest[i] << " ";
  }
  std::cout << std::endl;

  free(gatheredSample);
  free(pivots);
  free(threads);
  free(jobs);

  return 1;
}

