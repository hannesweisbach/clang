// RUN: %clang_cc1 -emit-llvm < %s | FileCheck %s
void a(void* x) {
  __builtin_set_return_address(x);
}
// CHECK: llvm.setreturnaddress
