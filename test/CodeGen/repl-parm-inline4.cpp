// RUN: %clang_cc1 -x c++ -emit-llvm  -frepl-parm -fno-repl-inline %s -o - | FileCheck -check-prefix INLINE %s

class TestClass
{
public:
  inline int method3(int* arg5, int arg6);
};

inline int TestClass::method3(int* arg5, int arg6) {
// CHECK-NOT: this.addrrepl1
// CHECK-NOT: this.addrrepl2

// CHECK: arg5.addrrepl1
// CHECK: arg5.addrrepl2
// INLINE-NOT: arg5.addrrepl1
// INLINE-NOT: arg5.addrrepl2

// CHECK-NOT: arg6.addrrepl1
// CHECK-NOT: arg6.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
// INLINE-NOT: replParm1
// INLINE-NOT: replParm2
  int* local = arg5;
  return 0;
}

void func()
{
  int a;
  int b;

  TestClass tc;
  tc.method3(&a, b);
}
