Resilient compiler
==================

The aim is to incorporate software fault tolerance techniques into
llvm and clang to build a resilient compiler.

Currently available are:
- dynamic dispatch protection (tmr-vptr/extended-vptr-replication)
- replication of the return address (repl-return-address)
- replication of function parameters (repl-function-parameters)

Check the *README.md* files in each feature branch for more details.

*NOTE* Use the corresponding branches in the llvm and clang source trees.
If no branch with the same name exists, use the master branch.

Links to llvm and clang repos:
- llvm: <https://github.com/abred/llvm>
- clang: <https://github.com/hannesweisbach/clang>
- compiler-rt: <https://github.com/llvm-mirror/compiler-rt> (unmodified)

Dynamic dispatch protection
---------------------------
On the tmr-vptr branch dynamic dispatch protection is implemented. The Vptr of
virtual classes is protected using Triple Modular Redundancy (TMR). Upon each
dereference of the Vptr the replicas are checked and corrected if a mismatch is
detected. If no two Vptr replicas are equal, the program is terminated via a
trap. Please note that, since Vptr TMR changes the layout of objects, it is
not ABI-compatible with object code compiled without Vptr replication.

Compiler flag: **-fprotect-vptr**


Replication of the return address
---------------------------------

The repl-return-intrinsic branch implements a simple TMR-based protection of the
return address. On function entry the return address for the current function is
stored twice. On function exit the return address is retrieved a third time and
the two stored values are reloaded. If no two values are equal the program is
terminated, otherwise the correct value is restored, if neccessary, and the
program continues.

Compiler flag: **-repl-return**


Replication of function parameters
----------------------------------

The repl-func-parameters branch implements a simple TMR-based protection of
function parameters of pointer type. On function entry every pointer parameter
is replicated twice and the copies are stored on the stack. Every time it is
accessed the value is checked against the copies. If a fault occurred and the
correct value can be recovered the program can continue, otherwise it is
terminated.

Compiler flag: **-frepl-parm**
