// RUN: %clang -target x86_64-unknown-unknown -o %t %s
// RUN: %t
// RUN: %clang -target i386-unknown-unknown -o %t %s
// RUN: %t
// RUN: %clang -target i686-unknown-unknown -o %t %s
// RUN: %t
#include <stdlib.h>

void a() {
  exit(0);
}

void b() {
  __builtin_set_return_address(&a);
}

int main()
{
  b();
  return 1;
}
