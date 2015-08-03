// RUN: %clang -O0 -frepl-return -o %t %s
// RUN: not %t
// RUN: %clang -O0 -o %t %s
// RUN: %t

// RUN: %clang -O1 -frepl-return -o %t %s
// RUN: not %t
// RUN: %clang -O1 -o %t %s
// RUN: %t

// RUN: %clang -O2 -frepl-return -o %t %s
// RUN: not %t
// RUN: %clang -O2 -o %t %s
// RUN: %t

#include <stdlib.h>
#include <stdint.h>
void a() {
  exit(0);
}

float b() {
  uint64_t i = 1;
  __builtin_set_return_address(a);
  for(size_t j = 0; j < 99; ++j)
  {
    i += j;
  }
  return 42.23;
}

int main()
{
  return b();
}
