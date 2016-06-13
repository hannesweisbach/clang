// RUN: %clang_cc1 -emit-llvm  -frepl-frame-addr %s -o - | FileCheck %s

void a() {
// CHECK: llvm.frameaddress
  int a = 1337;
  return;
// CHECK: llvm.setframeaddress
}
