// RUN: %clang_cc1 -emit-llvm  -frepl-parm %s -o - | FileCheck %s

int func1(int* arg1, int arg2) {
// CHECK: arg1.addrrepl1
// CHECK: arg1.addrrepl2

// CHECK-NOT: arg2.addrrepl2
// CHECK-NOT: arg2.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
  int* local = arg1;
  return 0;
}

float func2(float* arg1, float arg2) {
// CHECK: arg1.addrrepl1
// CHECK: arg1.addrrepl2

// CHECK-NOT: arg2.addrrepl2
// CHECK-NOT: arg2.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
  float* local = arg1;
  return 0.0;
}

typedef int (*fncptr)(float);

int func3(fncptr arg1) {
// CHECK: arg1.addrrepl1
// CHECK: arg1.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
  fncptr local = arg1;
  return 0;
}
