#include <iostream>
#include <mutex>
#include <cstdlib>
#include <pthread.h>
#include <pthread_impl.h>
#include <sys/_pthread/_pthread_mutex_t.h>

#define DEBUG

using namespace std;

struct pthread_barrier {
  int waitingCount;
  int conditionCount;
  pthread_mutex_t lock;
  pthread_cond_t cond;
};

pthread_barrier mbarrier;

int pthread_barrier_init(pthread_barrier * b, const int conditionCount) {

  b->conditionCount = conditionCount;
  b->waitingCount = 0;

  pthread_mutex_init(&b->lock, 0);
  pthread_cond_init(&b->cond, 0);

  return 1;
};

int pthread_barrier_await(pthread_barrier * b) {
  pthread_mutex_lock(&b->lock);
  b->waitingCount += 1;

  if (b->waitingCount >= b->conditionCount) {
    pthread_cond_broadcast(&b->cond);
    pthread_mutex_unlock(&b->lock);
    return 0; // this was the thread that unlocked us
  } else {
    // give up lock and wait for condition
    pthread_cond_wait(&b->cond, &b->lock);
    pthread_mutex_unlock(&b->lock);
  }

  return 1;
}

int pthread_barrier_destroy(pthread_barrier * b) {
  pthread_cond_destroy(&b->cond);
  pthread_mutex_destroy(&b->lock);

  return 1;
}

struct thread_job {
  const unsigned int start;
  const unsigned int end;
};

thread_job **jobs;
unsigned int numJobs;
int list[] = {73, 123, 13, 8, 9, 12, 7, 231, 39 ,2, 38};

int compare(const void *l, const void *r) {
  return *((int*)l) < *((int*)r);
}

void phase1(int* list[], size_t size) {
  qsort(list, sizeof(int), size, &compare);
}

void* psrs(void* arg) {

#ifdef DEBUG
  std::cout << "Thread initialized! Running psrs..." << std::endl;
#endif // DEBUG

#ifdef DEBUG
  std::cout << "Job finished... syncing and dying..." << std::endl;
#endif // DEBUG
  pthread_barrier_await(&mbarrier);

  return NULL;
}

int main(int argc, char* argv[]) {

  numJobs = std::stoi(argv[1]);

#ifdef DEBUG
  std::cout << "Creating " <<  numJobs << " threads" << std::endl;
#endif // DEBUG

  // + 1, main thread
  pthread_barrier_init(&mbarrier, numJobs+1);
  
  pthread_t threads[numJobs];
  for (size_t i = 0; i < numJobs ; i++) {

#ifdef DEBUG
  std::cout << "Creating thread number " << i << std::endl;
#endif // DEBUG

    pthread_create(&threads[i], NULL, &psrs, NULL);
  }

  cout << "Main thread waiting..." << std::endl;
  pthread_barrier_await(&mbarrier);

  pthread_barrier_destroy(&mbarrier);
  return 1;
}

