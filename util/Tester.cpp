#include "Tester.h"
#include "Comparator.h"
#include <iostream>

using namespace std;

/*
 * generate random list of size n
*/
int* generate(size_t n) {
  srandom(time(NULL));
  
  int* result = new int[n];

  for (int i = 0 ; i < n ; i++) {
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
      return false;
    }
  }
  
  return true;
}

