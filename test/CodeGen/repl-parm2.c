// RUN: %clang -frepl-parm -o %t %s
// RUN: %t
// RUN: %clang -o %t %s
// RUN: not %t

#include <stdlib.h>
#include <stdint.h>

int func1(int a, int* arg) {
  int* tmp = arg;
  int** q = &arg;

  *q = &a;
  if(arg == tmp)
    return 0;
  else
    return 1;
}

int main()
{
  int a;
  int b;
  return func1(a, &b);
}
