// RUN: %clang_cc1 -emit-llvm  -frepl-parm %s -o - | FileCheck %s

int func(int* arg1, int arg2) {
// CHECK: arg1.addrrepl1
// CHECK: arg1.addrrepl2

// CHECK-NOT: arg2.addrrepl2
// CHECK-NOT: arg2.addrrepl2

// check on usage
// CHECK: %comp13 = icmp eq i32* %0, %replParm1
// CHECK: %comp23 = icmp eq i32* %0, %replParm2
  int* local1 = arg1;

// CHECK-NOT: %comp13 = icmp eq i32* %0, %replParm1
// CHECK-NOT: %comp23 = icmp eq i32* %0, %replParm2
  int local2 = arg2;

// update on assign operator
// CHECK: store i32* %{{[0-9]}}, i32** %arg1.addrrepl1
// CHECK-NEXT: store i32* %{{[0-9]}}, i32** %arg1.addrrepl2
  arg1 = local1;

// CHECK-NOT: store i32* %{{[0-9]}}, i32** %arg2.addrrepl1
// CHECK-NOT: store i32* %{{[0-9]}}, i32** %arg2.addrrepl2
  arg2 = 42;

// update on compound assign operator
// CHECK: store i32* %add.ptr, i32** %arg1.addrrepl1
// CHECK-NEXT: store i32* %add.ptr, i32** %arg1.addrrepl2
  arg1 += 23;

// CHECK-NOT: store i32* %add.ptr, i32** %arg2.addrrepl1
// CHECK-NOT: store i32* %add.ptr, i32** %arg2.addrrepl2
  arg2 += 23;

// update on inc/dec
// CHECK: store i32* %incdec.ptr, i32** %arg1.addrrepl1
// CHECK-NEXT: store i32* %incdec.ptr, i32** %arg1.addrrepl2
  ++arg1;

// CHECK-NOT: store i32* %incdec.ptr, i32** %arg2.addrrepl1
// CHECK-NOT: store i32* %incdec.ptr, i32** %arg2.addrrepl2
  ++arg2;

  return 0;
}
