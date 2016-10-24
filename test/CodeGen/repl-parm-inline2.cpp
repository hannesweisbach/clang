// RUN: %clang_cc1 -x c++ -emit-llvm  -frepl-parm -fno-repl-inline %s -o - | FileCheck -check-prefix INLINE %s

class TestClass
{
public:
  int method(int* arg1, int arg2)
  {
// CHECK-NOT: this.addrrepl1
// CHECK-NOT: this.addrrepl2

// CHECK: arg1.addrrepl1
// CHECK: arg1.addrrepl2
// INLINE-NOT: arg1.addrrepl1
// INLINE-NOT: arg1.addrrepl2

// CHECK-NOT: arg2.addrrepl1
// CHECK-NOT: arg2.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
// INLINE-NOT: replParm1
// INLINE-NOT: replParm2
    int* local = arg1;
    return 0;
  }
};

void func()
{
  int a;
  int b;

  TestClass tc;
  tc.method(&a, b);
}
