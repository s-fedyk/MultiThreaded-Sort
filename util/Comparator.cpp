#include "Comparator.h"

int intCompare(const void *l, const void *r) {
  return ( *(int*)l - *(int*)r );
}
