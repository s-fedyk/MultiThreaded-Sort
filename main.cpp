#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <math.h>
#include <mutex>
#ifdef __APPLE__
#include "barrier.h"
#endif
#include "BinarySearch.h"
#include "tester.h"
#include <cstdlib>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/signal.h>

#define DEBUG

#define MASTER if (job->index == p-1)

using namespace std;

struct sample_partition {
  int* base;
  size_t size;
};

struct thread_job {
  size_t index;
  sample_partition *partitions;
  size_t partitionSize;
};

pthread_barrier_t mbarrier;
pthread_t* threads;

// global job list
thread_job *jobs = nullptr;

size_t p,w, sampleSize, leftOver;
long n;
chrono::time_point<chrono::steady_clock> start;

timeval psrsStart, psrsEnd, p1Start, 
        p1End, p2Start, p2End, p3Start, 
        p3End, p4Start, p4End;

int* pivots;

// threads contribute to this
int* gatheredSample;

int *list;

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
void phase4(size_t partitionIndex) {
  sample_partition toMerge[p];
  size_t partitionSize = 0;

  for (size_t threadIndex = 0 ; threadIndex < p ; threadIndex++) {
    toMerge[threadIndex] = jobs[threadIndex].partitions[partitionIndex];
    partitionSize += toMerge[threadIndex].size;
  }

  jobs[partitionIndex].partitionSize = partitionSize;
  pthread_barrier_wait(&mbarrier);

  size_t resultIndex = 0;
  for (size_t threadIndex = 0 ; threadIndex < partitionIndex ; threadIndex++) {
    resultIndex += jobs[threadIndex].partitionSize;
  }

  size_t indexes[p]; // we are merging this many partitions
  memset(indexes, 0x0, p*sizeof(size_t));

  int minThread = -1; 
  for (size_t i = 0 ; i < partitionSize ; i++) {
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

void* psrs(void* arg) {
  thread_job *job = (thread_job*)arg;

  //start timing p1 everyone reaches barrier
  pthread_barrier_wait(&mbarrier);
  MASTER {
    gettimeofday(&psrsStart, NULL);
    gettimeofday(&p1Start, NULL);
  }

  pthread_barrier_wait(&mbarrier);
  
  // first n%p threads will take 1 more 
  size_t sampleAdjust = (job->index < leftOver) ? 1 : 0;

  // adjust bounds for n%p threds
  size_t startAdjust = (job->index < leftOver) ? job->index : leftOver;

  phase1(&list[sampleSize * job->index + startAdjust], sampleSize + sampleAdjust, &gatheredSample[job->index * p]);

  // phase 1 finished
  pthread_barrier_wait(&mbarrier);
  MASTER {
    gettimeofday(&p1End, NULL);
  }
  pthread_barrier_wait(&mbarrier);

  MASTER {
    gettimeofday(&p2Start, NULL);
    pivots = phase2(gatheredSample, p);
    gettimeofday(&p2End, NULL);
  }

  pthread_barrier_wait(&mbarrier);
  MASTER {
    gettimeofday(&p3Start, NULL);
  }
  pthread_barrier_wait(&mbarrier);

  job->partitions = phase3(&list[sampleSize * job->index + startAdjust], sampleSize+sampleAdjust, pivots, p);

  pthread_barrier_wait(&mbarrier);
  MASTER {
    gettimeofday(&p3End, NULL);
    gettimeofday(&p4Start, NULL);
  }
  pthread_barrier_wait(&mbarrier);

  phase4(job->index);

  pthread_barrier_wait(&mbarrier);
  MASTER {
    gettimeofday(&p4End, NULL);
    gettimeofday(&psrsEnd, NULL);

    long psrsElapsed = (psrsEnd.tv_sec-psrsStart.tv_sec)*1000000 + psrsEnd.tv_usec-psrsStart.tv_usec;
    long p1Elapsed = (p1End.tv_sec-p1Start.tv_sec)*1000000 + p1End.tv_usec-p1Start.tv_usec;
    long p2Elapsed = (p2End.tv_sec-p2Start.tv_sec)*1000000 + p2End.tv_usec-p2Start.tv_usec;
    long p3Elapsed = (p3End.tv_sec-p3Start.tv_sec)*1000000 + p3End.tv_usec-p3Start.tv_usec;
    long p4Elapsed = (p4End.tv_sec-p4Start.tv_sec)*1000000 + p4End.tv_usec-p4Start.tv_usec;

    std::cout << psrsElapsed << "," << p1Elapsed << "," <<p2Elapsed << "," << p3Elapsed << "," << p4Elapsed << std::endl;
    /*
    std::cout << checkSorted(dest, n) << std::endl;
    std::cout << checkElements(dest, list, n) << std::endl;
    */

    free(dest);
    free(gatheredSample);
    free(pivots);
    free(threads);
    free(jobs);
  }

  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  n = std::stol(argv[1]);
  p = std::stoi(argv[2]);
  list = generate(n);
  pthread_setconcurrency(p);
  w = n/(pow(p,2));
  sampleSize = n/p;
  leftOver = n%p;
  
  dest = new int[n];

  pthread_barrier_init(&mbarrier,NULL, p);
  
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
