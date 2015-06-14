// RUN: %clang -frepl-return -o %t %s
// RUN: %t
// RUN: %clang -o %t %s
// RUN: not %t

#include <stdlib.h>
#include <stdint.h>
void a() {
  exit(1);
}

void b() {
  uint64_t i = 1;
  __builtin_set_return_address(a);
}

int main()
{
  b();
  return 0;
}
