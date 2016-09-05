// RUN: %clang_cc1 -x c++ -emit-llvm  -frepl-return %s -o - | FileCheck %s

void a() {
}

class TestClass
{
public:
    float method()
    {
// CHECK: llvm.returnaddress
        int i = 1;
        __builtin_set_return_address((void*)a);
// CHECK: llvm.setreturnaddress
        for(int j = 0; j < 99; ++j)
        {
            i += j;
        }
        return 42.23;
// CHECK: llvm.setreturnaddress
    }

};
