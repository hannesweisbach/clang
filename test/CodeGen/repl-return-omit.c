// RUN: %clang_cc1 -emit-llvm  -frepl-return %s -o - | FileCheck %s

__attribute__ ((omit_repl_return))
float b() {
// CHECK-NOT: llvm.returnaddress
  int i = 1;
  __builtin_set_return_address(b);
// CHECK: llvm.setreturnaddress
  for(int j = 0; j < 99; ++j)
  {
    i += j;
  }
  return 42.23;
// CHECK-NOT: repl.ret.false.trap
}
