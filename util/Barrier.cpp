#ifdef __APPLE__
#include "Barrier.h"
#include <iostream>

// var null to fit pthread impl, even though i don't fully do it
int pthread_barrier_init(pthread_barrier_t * b, void* _, const int conditionCount) {

  b->conditionCount = conditionCount;
  b->waitingCount = 0;
  return !pthread_mutex_init(&b->lock, 0) && !pthread_cond_init(&b->cond, 0);
};

int pthread_barrier_wait(pthread_barrier_t * b) {
  pthread_mutex_lock(&b->lock);
  b->waitingCount += 1;

  if (b->waitingCount >= b->conditionCount) {
    b->waitingCount = 0;
    pthread_cond_broadcast(&b->cond);
    pthread_mutex_unlock(&b->lock);
    return 0; // this was the thread that unlocked us
  } else {
    // give up lock and wait for condition
    pthread_cond_wait(&b->cond, &(b->lock));
    pthread_mutex_unlock(&b->lock);
  }

  return 1;
}

int pthread_barrier_destroy(pthread_barrier_t * b) {
  pthread_cond_destroy(&b->cond);
  pthread_mutex_destroy(&b->lock);

  return 1;
}
#endif
