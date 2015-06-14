// RUN: %clang -frepl-return -o %t %s
// RUN: not %t
// RUN: %clang -o %t %s
// RUN: %t
// RUN: %clang -c -emit-llvm %s -o %t -frepl-return | FileCheck %s

#include <stdlib.h>
#include <stdint.h>
void a() {
  exit(0);
}

float b() {
// CHECK: llvm.returnaddress
  uint64_t i = 1;
  __builtin_set_return_address(a);
// CHECK: llvm.setreturnaddress
  for(size_t j = 0; j < 99; ++j)
  {
    i += j;
  }
  return 42.23;
// CHECK: llvm.setreturnaddress
}

int main()
{
  return b();
}
