#include <iostream>
#include <mutex>
#include <pthread.h>
#include <pthread_impl.h>
#include <sys/_pthread/_pthread_mutex_t.h>

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

struct thread_portion {
  int start;
  int end;
};

void* phase1(void* arg) {
  return NULL;
}

void* phase3(void* arg) {
  return NULL;
}

void* test1(void* arg) {

  cout << " Test1 thread dying" << "\n";
  pthread_barrier_await(&mbarrier);

  pthread_exit(NULL);
}

void* test2(void* arg) {
  cout << " Test2 thread dying" << "\n";
  pthread_barrier_await(&mbarrier);

  pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
  
  pthread_barrier_init(&mbarrier, 4);
  
  pthread_t thread1, thread2, thread3;
  pthread_create(&thread1, NULL, &test1, NULL);
  pthread_create(&thread2, NULL, &test2, NULL);
  pthread_create(&thread3, NULL, &test2, NULL);

  pthread_barrier_await(&mbarrier);

  cout << " main thread dying" << std::endl;
  pthread_barrier_destroy(&mbarrier);
  return 1;
}

