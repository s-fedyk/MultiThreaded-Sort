#include <iostream>
#include <queue>
#include <sys/time.h>
#include <cstring>
#include <math.h>

#ifdef __APPLE__
#include "util/Barrier.h"
#endif
#include "util/DataTypes.h"
#include "util/BinarySearch.h"
#include "util/Tester.h"
#include "util/Comparator.h"

//#define DEBUG

#define MASTER if (job->index == p-1)

using namespace std;

pthread_barrier_t mbarrier;
pthread_t* threads;

// global job list
thread_job *jobs = nullptr;

size_t p,w, leftOver;
unsigned long n, sampleSize;

timeval psrsStart, psrsEnd, p1Start, 
        p1End, p2Start, p2End, p3Start, 
        p3End, p4Start, p4End;

int *list, *gatheredSample, *pivots, *dest;

int* phase1(int list[], const unsigned long size, int sample[]) {
  qsort(list, size, sizeof(int), intCompare);

  for (unsigned long i = 0 ; i < p; i++) {
    sample[i] = list[i*w];
  }

  return sample;
}

int* phase2(int gatheredSample[], size_t p) {
  qsort(gatheredSample, p*p, sizeof(int), intCompare);

  int* pivotList = new int[p-1];

  for (unsigned long i = 0; i < p-1 ; i++) {
    pivotList[i] = gatheredSample[i*p + p];
  }

  return pivotList;
}

sample_partition* phase3(int list[], const size_t size, int pivots[], size_t p) {
  sample_partition *partitions = new sample_partition[p];
  int offset = 0;
  for (size_t i = 0 ; i < p-1 ; i++) {
    partitions[i].base = &list[offset];
    partitions[i].size = BinarySearch(list, size, pivots[i]) - offset;
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

  unsigned long resultIndex = 0;
  for (size_t threadIndex = 0 ; threadIndex < partitionIndex ; threadIndex++) {
    resultIndex += jobs[threadIndex].partitionSize;
  }

  size_t indexes[p]; // we are merging this many partitions
  memset(indexes, 0x0, p*sizeof(size_t));

  std::priority_queue<queue_entry, vector<queue_entry> , std::greater<queue_entry> > queue;

  for (size_t threadIndex = 0 ; threadIndex < p ; threadIndex++) {
    // fully merged this sublist
    if (toMerge[threadIndex].size == indexes[threadIndex]) continue;

    int currentObservation = toMerge[threadIndex].base[indexes[threadIndex]];

    queue_entry entry; 
    entry.value = currentObservation;
    entry.thread = threadIndex;

    queue.push(entry);
    indexes[threadIndex]++;
  }

  while (!queue.empty()) {
    dest[resultIndex] = queue.top().value;
    size_t threadIndex = queue.top().thread;
    queue.pop();

    if (toMerge[threadIndex].size > indexes[threadIndex]) {
      int currentObservation = toMerge[threadIndex].base[indexes[threadIndex]];

      queue_entry entry; 
      entry.value = currentObservation;
      entry.thread = threadIndex;

      queue.push(entry);
      indexes[threadIndex]++;
    }

    resultIndex++;
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
  unsigned long sampleAdjust = (job->index < leftOver) ? 1 : 0;

  // adjust bounds for n%p threds
  unsigned long startAdjust = (job->index < leftOver) ? job->index : leftOver;

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

#ifdef DEBUG
    std::cout << checkElements(dest, list, n) << std::endl;
#endif

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
