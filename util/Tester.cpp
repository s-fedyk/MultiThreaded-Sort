#include "Tester.h"

#include <iostream>
#include <climits>

#include "Comparator.h"

using namespace std;

/*
 * generate random list of size n
*/
int* generate(size_t n) {
  srandom(time(NULL));
  
  int* result = new int[n];

  for (size_t i = 0 ; i < n ; i++) {
    result[i] = random() % INT_MAX;
  }

  return result;
}

/**
 * quicksort and check
 **/
bool checkElements(int* sorted, int* original, size_t n) {
  qsort(original, n, sizeof(int), intCompare);

  for (int i = 0 ; i < n ; i ++) {

    if (sorted[i] != original[i]) {
      std::cout << "bad sort at " << i << std::endl;
      return false;
    }

  }
  
  return true;
}

