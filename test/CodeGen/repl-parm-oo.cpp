// RUN: %clang_cc1 -x c++ -emit-llvm  -frepl-parm %s -o - | FileCheck %s

class TestClass
{
public:
  int method(int* arg1, int arg2)
  {
// CHECK: this.addrrepl1
// CHECK: this.addrrepl2

// CHECK: arg1.addrrepl1
// CHECK: arg1.addrrepl2

// CHECK-NOT: arg2.addrrepl2
// CHECK-NOT: arg2.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
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
