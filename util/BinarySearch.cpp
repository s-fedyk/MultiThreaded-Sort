#include "BinarySearch.h"
#include "Comparator.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>

/*
 * test rig to quickly make sure I know what I'm doing
 */
bool testSearch() {
  srand(time(NULL));

  int test[1000];

  for (int i = 0; i < 1000 ; i++) {
    test[i] = rand() % 1000;
  }

  qsort(test, 1000, sizeof(int), intCompare);
  int test1[999];
  int removedIndex = rand() % 1000;
  
  size_t x = 0;
  for (int i = 0; i < 1000; i++) {
    if (i != removedIndex) {
      test1[x] = test[i];
      x++;
    }
  }

  int result = BinarySearch(test1, 1000, test[removedIndex]);
  bool good = true;

  for (int i = result; i < 1000 ; i++) {
    if (test1[i] < test[removedIndex]) {
      good = false;
    }
  }

  for (int i = 0; i < result; i++) {
    if (test1[i] > test[removedIndex]) {
      good = false;
    }
  }

  return good;
}

/**
 * Binary search to split array into partitions
 */
int BinarySearch(int list[], const unsigned long size, const int item) {
  unsigned long low = 0;
  unsigned long high = size-1;

  while (low <= high) {
    unsigned long index = low + (high - low) / 2;
    if (list[index] == item) {
        return index; // Item found at index
    } else if (list[index] > item) {
        high = index - 1;
    } else {
        low = index + 1;
    }  
  }

  return low;
}
