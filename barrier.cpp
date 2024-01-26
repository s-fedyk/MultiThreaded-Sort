#include "barrier.h"
#include <iostream>

int pthread_barrier_init(pthread_barrier * b, const int conditionCount) {

  b->conditionCount = conditionCount;
  b->waitingCount = 0;
  return !pthread_mutex_init(&b->lock, 0) && !pthread_cond_init(&b->cond, 0);
};

int pthread_barrier_await(pthread_barrier * b) {
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

int pthread_barrier_destroy(pthread_barrier * b) {
  pthread_cond_destroy(&b->cond);
  pthread_mutex_destroy(&b->lock);

  return 1;
}
