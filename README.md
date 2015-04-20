My playground to incorporate software fault tolerance techniques into clang to
build a resilient compiler.

On the tmr-vptr branch dynamic dispatch protection is implemented. The Vptr of
virtual classes is protected using Triple Modular Redundancy (TMR). Upon each
dereference of the Vptr the replicas are checked and corrected if a mismatch is
detected. If no two Vptr replicas are equal, the program is terminated via a
trap.  Please note that, since Vptr TMR changes the layout of objects, it is
not ABI-compatible with object code compiled without Vptr replication.

To enable generation of TMR-protected Vptrs, the flag -fprotect-vptr has to be
passed. To be compatible to a non-TMR-protected standard library, the flag
-fno-std-protection can be used to disable generation of a replication Vptr for
everyting in namespace ::std and below.
Verbose output can be enabled with -fverbose-fault-tolerance. This lists all
virtual classes for which Vptr replication is enabled and those for which it is
disabled (if -fno-std-protection is used).

To entirely protect an application, a standard library compiled with
TMR-protected Vptrs has to be used. libcxx and libcxxabi have been used for
this.

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
