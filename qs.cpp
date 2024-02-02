#include <cstdlib>
#include <iostream>
#include "tester.h"
#include <sys/time.h>
#include <sys/select.h>

using namespace std;

timeval qsStart;
timeval qsEnd;

int* list;

int compare(const void *l, const void *r) {
  return ( *(int*)l - *(int*)r );
}

int main(int argc, char* argv[]) {
  int n = std::stoi(argv[1]);
  list = generate(n);

  gettimeofday(&qsStart, NULL);
  qsort(list, n, sizeof(int), compare);
  gettimeofday(&qsEnd, NULL);

  long elapsed = (qsEnd.tv_sec-qsStart.tv_sec)*1000000 + qsEnd.tv_usec-qsStart.tv_usec;
  std::cout << elapsed<< "\n";
 
  return 1;
}
