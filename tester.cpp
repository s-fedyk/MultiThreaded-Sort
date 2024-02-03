#include "tester.h"
#include "time.h"
#include <climits>
#include <cstdlib>
#include <iostream>
#include <map>

using namespace std;

int* generate(size_t n) {
  srandom(time(NULL));
  
  int* result = new int[n];

  for (int i = 0 ; i < n ; i++) {
    result[i] = random() % INT_MAX;
  }

  return result;
}

bool checkSorted(int* list, size_t n) {
  for (size_t i = 1 ; i < n ; i++) {
    if (list[i] < list[i-1]) {

      std::cout << list[i] << " " << list[i-1] << std::endl; 
      return false;
    }
  }

  return true;
}

bool checkElements(int* sorted, int* original, size_t n) {
  map<size_t, size_t> originalCount;
  map<size_t, size_t> newCount;

  for (size_t i = 0 ; i < n ; i++) {
    originalCount[original[i]] += 1;
    newCount[sorted[i]] += 1;
  };

  for (auto i = originalCount.begin() ; i != originalCount.end() ; i++) {
    if (originalCount[i->first] != newCount[i->first]) {
      return false;
    }
  }
     
  return true;
}

