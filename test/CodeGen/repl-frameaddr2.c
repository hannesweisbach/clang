// RUN: %clang -frepl-frame-addr -o %t %s -O2
// RUN: %t
// RUN: %clang -o %t %s -O2
// RUN: not %t
// RUN: %clang_cc1 -emit-llvm %s -frepl-frame-addr -o - | FileCheck %s

#include <stdlib.h>
#include <stdio.h>

void func2(int a)
{
// CHECK: llvm.frameaddress
// CHECK: llvm.frameaddress

// CHECK: llvm.frameaddress
	/* int* c = __builtin_frame_address(2); */
// CHECK: llvm.setframeaddress
	__builtin_set_frame_address((void*)&a);

// CHECK: llvm.setframeaddress
}

int func1(int a)
{
	func2(a);
	int b = 3;
	int c = 2;
	int d = 2;
	int f = 12;
	int e = 42;
	int g = 21;
	int h = 35;
	int i = 32;
	int j = 342;
	int k =32;
	printf("%d %d %d %d %d %d %d %d %d %d\n", a,b,c,d,e,f,h,i,j,k);

	return a;
}


int main(int argc, char *argv[])
{
	int t[2] = {0};
	int a = 1;
	a = func1(a);
	int b = 3;
	int c = 2;
	int d = 2;
	int f = 12;
	int e = 42;
	int g = 21;
	int h = 35;
	int i = 32;
	int j = 342;
	int k =32;
	printf("test");
	int m = func1(b);
	printf("%d %d %d %d %d %d %d %d %d %d\n", a,b,c,d,e,f,h,i,j,k);
	return a == 0;
}
