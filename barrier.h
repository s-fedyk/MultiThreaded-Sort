#pragma once

#include <pthread.h>
#include <pthread_impl.h>
#include <sys/_pthread/_pthread_mutex_t.h>

struct pthread_barrier {
  int waitingCount;
  int conditionCount;
  pthread_mutex_t lock;
  pthread_cond_t cond;
};

int pthread_barrier_init(pthread_barrier * b, const int conditionCount);
int pthread_barrier_await(pthread_barrier * b);
int pthread_barrier_destroy(pthread_barrier * b);