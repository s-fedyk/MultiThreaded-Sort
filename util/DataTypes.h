#include <cstdlib>

struct sample_partition {
  int* base;
  size_t size;
};

struct queue_entry {
  int value;
  size_t thread;

  bool operator>(const queue_entry& a) const {
    return value>a.value;
  }
};

struct thread_job {
  size_t index;
  sample_partition *partitions;
  size_t partitionSize;
};

