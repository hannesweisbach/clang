Resilient compiler
==================

The aim is to incorporate software fault tolerance techniques into
llvm and clang to build a resilient compiler.

Check *README.md* in the llvm source tree for some additional information.

based on:
llvm commit 68081f41fafc13e194dfa639a3219b21c176489f
clang commit 4c2ff0513c4753020510eaa887e2be1db944c9ab
compiler-rt commit 36ee62811293cb044305ee25224f6d4b7437d91c

!!Attention!!
Linux kernel >= 4.2?
	apply compiler-rt.patch to compiler-rt repo!
otherwise
	in llvm repo, checkout HEAD~1 (commit 4588b9872098a69c0baf68e1408bc6f3c74545c8)


Vptr
-----

My playground to incorporate software fault tolerance techniques into clang to
build a resilient compiler.

On the tmr-vptr branch dynamic dispatch protection is implemented. The Vptr of
virtual classes is protected using Triple Modular Redundancy (TMR). Upon each
dereference of the Vptr the replicas are checked and corrected if a mismatch is
detected. If no two Vptr replicas are equal, the program is terminated via a
trap.  Please note that, since Vptr TMR changes the layout of objects, it is
not ABI-compatible with object code compiled without Vptr replication.

To enable generation of TMR-protected Vptrs, the flag `-fprotect-vptr` has to
be passed. To be compatible to a non-TMR-protected standard library, the flag
`-fno-std-protection` can be used to disable generation of a replication Vptr
for everyting in namespace ::std and below.  Verbose output can be enabled with
`-fverbose-fault-tolerance`. This lists all virtual classes for which Vptr
replication is enabled and those for which it is disabled (if
`-fno-std-protection` is used).

To entirely protect an application, a standard library compiled with
TMR-protected Vptrs has to be used. libcxx and libcxxabi have been used for
this.

The 'normal' TMRed Vptrs are stored consecutively in memory. This has the
disadvantage that a single misplace (unaligned) write larger than one byte can
destroy two copies. To protect against unaligned writes, a word is reserved
between any two Vptr copies. This ensures, that even unaligned writes of up to
word size do not compromise more than one Vptr replica. This 'extended'
protection is selected with the `-fprotect-vptr-extened' flag.

How to build:

If you want a libcxx/libcxxabi with replicated Vptr PREFIX is a temporary
directory otherwise, it contains the final clang

```
export PREFIX=path/to/put/resilient/clang
```

Now build clang able to generate protected code. Use the -j flag for make as
desired. A Debug build also works. make check-all is optional.

```
cmake -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX \
-DCMAKE_C_COMPILER:STRING=clang \
-DCMAKE_CXX_COMPILER:STRING=clang++ \
-DCMAKE_BUILD_TYPE="Release" -DLLVM_TARGETS_TO_BUILD="X86" \
-DLLVM_BUILD_TOOLS=OFF -DLLVM_INCLUDE_EXAMPLES:BOOL=OFF \
-DLLVM_INCLUDE_TESTS:BOOL=ON .. && make \
&& make check-all && make install
```

If you don't want libcxx/libcxxabi with TMR'ed Vptrs you're done. Otherwise:
enable protection for libc++/libcxxabi in
llvm/projects/libcxxabi/CMakeLists.txt:199 and
llvm/projects/libcxx/CMakeLists.txt:137 by putting 
`set(LIBCXX_COMPILE_FLAGS "-fprotect-vptr")` there and build them:

Set Final install directory: 
`export PREFIX_STDLIB=path/to/resilient/clang-stdlib` and compile:

```
cmake -DCMAKE_INSTALL_PREFIX:PATH=PREFIX_STDLIB \
-DCMAKE_C_COMPILER:PATH=$PREFIX/bin/clang \
-DCMAKE_CXX_COMPILER:PATH=$PREFIX/bin/clang++ \
-DCMAKE_BUILD_TYPE="Release" -DLLVM_TARGETS_TO_BUILD="X86" \
-DLLVM_BUILD_TOOLS=OFF -DLLVM_INCLUDE_EXAMPLES:BOOL=OFF \
-DLLVM_INCLUDE_TESTS:BOOL=ON .. && make
```

You have now a resilient clang and a standard library. To have a self-hosting
resilient clang continue on. Use `export LD_LIBRARY_PATH=$PREFIX_STDLIB/lib/`
so that libc++/libcxxabi can be found and compile the self-hosting resilient
compiler:

```
cmake -DCMAKE_INSTALL_PREFIX:PATH=path/to/selfhosting/resilient/clang \
-DCMAKE_CXX_FLAGS="-fprotect-vptr" \
-DCMAKE_C_COMPILER:PATH=$PREFIX_STDLIB/bin/clang \
-DCMAKE_CXX_COMPILER:PATH=$PREFIX_STDLIB/bin/clang++ \
-DCMAKE_BUILD_TYPE="Release" -DLLVM_TARGETS_TO_BUILD="X86" \
-DLLVM_ENABLE_LIBCXXABI=ON -DLLVM_ENABLE_LIBCXX=ON \
-DLLVM_BUILD_TOOLS=OFF -DLLVM_INCLUDE_EXAMPLES:BOOL=OFF \
-DLLVM_INCLUDE_TESTS:BOOL=ON .. && make
```

Now you have to enable -fprotect-vptr for the tests where applicable. Some
(sanitizer) tests will fail, because their output now contains 24 Byte Vptrs
instead of 8 Byte ones, but with a bit of fixup all tests should succeed.


Replication of the return address
---------------------------------

The repl-return-intrinsic branch implements a simple TMR-based protection of the
return address. On function entry the return address for the current function is
stored twice. On function exit the return address is retrieved a third time and
the two stored values are reloaded. If no two values are equal the program is
terminated, otherwise the correct value is restored, if neccessary, and the
program continues.

### Implementation
The existing llvm intrinsic `llvm.returnaddress` is used to get the current
return address. For the restoration of the return address a new intrinsic called
`llvm.setreturnaddress` is implemented in the llvm source tree.

#### clang
`void CodeGenFunction::EmitReplicateReturnProlog()` is called at the start of
each function to create the copies.
`void CodeGenFunction::EmitReplicateReturnEpilog()` is called just before the
function returns and does the fault checking.

#### llvm
The intrinsic `llvm.setreturnaddress` is added to be able to change the return
address of the current function. During the generation of the DAG the matching
SETRETURNADDR-node (`SelectionDAGBuilder::visitIntrinsicCall`) is created  and
later during legalization lowered into the corresponding sequence of loads and
stores (`X86TargetLowering::LowerSETRETURNADDR`)


For testing purposes the function
```
void __builtin_set_return_address(void *)
```
can be used to access this intrinsic directly in C code.

### Usage:
To enable the generation of TMR-protected return addresses, the flag
**-frepl-return** has to be passed to clang. As all changes are inside of
functions, code generated using it is binary compatible to other code.
**-frepl-return-dbg** can be used for debugging purposes. In this case a trap
(int3) is inserted both if the program has to be terminated and if a correct
value could be restored.

*NOTE*: Currently the llvm.setreturnaddress intrinsic is only implemented for
 X86-based systems

*NOTE*: Currently there are 11 additional unexpected failures when running the
 llvm and clang tests. 6 of them are clang tests. However, none of these should
 pose a problem (check *failingTests.md* for more information).



#### Alternative implementation
In addition to the method using a modified clang, an alternative method using an
llvm FunctionPass is provided. The principle is the same but it does its work on
IR code.

How to use:

```
/path/to/clang -emit-llvm -o code.bc code.c
/path/to/opt -load /path/to/llvm/build/lib/LLVMReplicateReturn.so -repl-return <
code.bc > codeTransformed.bc
/path/to/clang -o prog codeTramsformed.bc
```

Replication of function parameters
----------------------------------

The repl-func-parameters branch implements a simple TMR-based protection of
function parameters of pointer type. On function entry every pointer parameter
is replicated twice and the copies are stored on the stack. Every time it is
accessed the value is checked against the copies. If a fault occurred and the
correct value can be recovered the program can continue, otherwise it is
terminated.

### Implementation

#### clang
On function entry clang emits an alloca for every parameter and stores the value
there. This is used as a hook to check the type and to create three instead of
one alloca for each pointer parameter (`CodeGenFunction::EmitParmDecl`).
Each time the parameter is accessed (`ScalarExprEmitter::VisitDeclRefExpr`) is
it checked for correctness.
If the value of the parameter is changed, the copies are updated as well
(`ScalarExprEmitter::EmitScalarPrePostIncDec`,
` ScalarExprEmitter::EmitCompoundAssignLValue`,
` ScalarExprEmitter::VisitBinAssign,
  CodeGenFunction::EmitBinaryOperatorLValue,
  CodeGenFunction::EmitCXXMemberOrOperatorCall,
  CodeGenFunction::EmitCXXMemberOrOperatorMemberCallExpr ).
After a function has been called, the copies of pointer parameters referenced in
the argument list are updated, they might have been modified by the function
(e.g. passed by ref))
After a lambda function has been called, the copies of pointer parameters
which have been captured by the lambda by Ref are updated.

The update and the check are done via
`CodeGenFunction::EmitPointerParmReplicaUpdate` and
`CodeGenFunction::EmitPointerParmReplicaCheck`.

TODO: Indirect accesses are not protected (e.g. via copies or pointer arithmetic)

#### llvm
The intrinsics `llvm.load.ptr` and `llvm.store.ptr` are added. They dereference
respectively assign a new value to a pointer. This is used to prevent unwanted
optimization occurring  on higher optimization levels as the compiler notices
the redundant copies. `llvm.load.ptr` is used to access and check the copies,
`llvm.store.ptr` is used to update the copies.

### Usage
To enable generation of TMR-protected function parameters, the flag
**-frepl-parm** has to be passed to clang. As all changes are inside
of functions code generated using this protection is binary
compatible to other code.
**-frepl-parm-dbg** can be used for debugging purposes. In this case a trap
(int3) is inserted both if the program has to be terminated and if a correct
value could be restored.
