Resilient compiler
==================

The aim is to incorporate software fault tolerance techniques into
llvm and clang to build a resilient compiler.

Check *README.md* in the llvm source tree for some additional information.


Replication of the return address
---------------------------------

The repl-return-intrinsic branch implements a simple TMR-based protection of the
return address. On function entry the return address for the current function is
stored twice. On function exit the return address is retrieved a third time and
the two stored values are reloaded. If no two values are equal the program is
terminated, otherwise the correct value is restored, if neccessary, and the
program continues.

The existing llvm intrinsic *llvm.returnaddress* is used to get the current
return address. For the restoration of the return address a new intrinsic called
*llvm.setreturnaddress* is implemented in the llvm source tree. The C function
```
void __builtin_set_return_address(void *)
```
can be used to access this intrinsic directly in C code.

To enable generation of TMR-protected return addresses, the flag -frepl-return
has to be passed to clang. As all changes are inside of functions code generated
using this protection is binary compatible to other code.

*NOTE*: Currently the llvm.setreturnaddress intrinsic is only implemented for
 X86-based systems

*NOTE*: Currently there are 11 additional unexpected failures when running the
 llvm and clang tests. 6 of them are clang tests. However, none of these should
 pose a problem (check *failingTests.md* for more information).
