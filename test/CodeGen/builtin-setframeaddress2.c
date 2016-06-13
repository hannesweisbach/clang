// RUN: %clang -o %t %s
// RUN: %t
// RUN: %clang_cc1 -emit-llvm < %s | FileCheck %s
void funcb2(int b)
{
	// equates to noop
// CHECK: llvm.frameaddress
	int* c = __builtin_frame_address(1);
// CHECK: llvm.setframeaddress
	__builtin_set_frame_address((void*)c);
}

int funcb1(int b)
{
	funcb2(b);
	return b;
}

void funca2(int a)
{
// CHECK: llvm.frameaddress
	int* c = __builtin_frame_address(2);
// CHECK: llvm.setframeaddress
	__builtin_set_frame_address((void*)c);
}

int funca1(int a)
{
	funca2(a);
	return a;
}


int main(int argc, char *argv[])
{
	int t[2] = {0};
	int a = 1;
	a = funca1(a);
	int b1 = a == 0;

	int b = 2;
	b = funcb1(b);
	int b2 = b == 2;

	return !(b1 && b2);
}
