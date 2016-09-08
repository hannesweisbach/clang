// RUN: %clang_cc1 -emit-llvm -fno-repl-inline  -frepl-parm %s -o - | FileCheck %s
static inline
int func1(int* arg1, int arg2) {
// CHECK-NOT: arg1.addrrepl1
// CHECK-NOT: arg1.addrrepl2

// CHECK-NOT: replParm1
// CHECK-NOT: replParm2
  int* local = arg1;
  return 0;
}

int main() {
  int a = 0;

  return func1(&a, 1);
}
