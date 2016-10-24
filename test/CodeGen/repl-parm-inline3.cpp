// RUN: %clang_cc1 -x c++ -emit-llvm  -frepl-parm -fno-repl-inline %s -o - | FileCheck -check-prefix INLINE %s

class TestClass
{
public:
  int method2(int* arg3, int arg4);
};

int TestClass::method2(int* arg3, int arg4) {
// CHECK-NOT: this.addrrepl1
// CHECK-NOT: this.addrrepl2

// CHECK: arg3.addrrepl1
// CHECK: arg3.addrrepl2
// INLINE: arg3.addrrepl1
// INLINE: arg3.addrrepl2

// CHECK-NOT: arg4.addrrepl1
// CHECK-NOT: arg4.addrrepl2

// CHECK: replParm1
// CHECK: replParm2
// INLINE: replParm1
// INLINE: replParm2
  int* local = arg3;
  return 0;
}

void func()
{
  int a;
  int b;

  TestClass tc;
  tc.method2(&a, b);
}
