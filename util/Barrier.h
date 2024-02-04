#pragma once

#include <pthread.h>

struct pthread_barrier_t {
  int waitingCount;
  int conditionCount;
  pthread_mutex_t lock;
  pthread_cond_t cond;
};

int pthread_barrier_init(pthread_barrier_t* b,void* null, const int conditionCount);
int pthread_barrier_wait(pthread_barrier_t* b);
int pthread_barrier_destroy(pthread_barrier_t* b);
