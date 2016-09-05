// RUN: %clang_cc1 -emit-llvm  -frepl-return %s -o - | FileCheck %s

void a() {
  exit(0);
}

float b() {
// CHECK: llvm.returnaddress
  int i = 1;
  __builtin_set_return_address(a);
// CHECK: llvm.setreturnaddress
  for(int j = 0; j < 99; ++j)
  {
    i += j;
  }
  return 42.23;
// CHECK: llvm.setreturnaddress
}
