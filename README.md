Resilient compiler
==================

The aim is to incorporate software fault tolerance techniques into
llvm and clang to build a resilient compiler.

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
