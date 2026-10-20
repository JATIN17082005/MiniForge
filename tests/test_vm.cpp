#include "test_framework.hpp"
#include "codegen/Bytecode.hpp"
#include "vm/VM.hpp"
#include <sstream>

using namespace miniforge;

TEST_CASE(VMArithmeticAndStack) {
    BytecodeProgram program;
    // 10 + 20 * 2
    program.addInstruction(VMInstruction(Opcode::PushConst, 10));
    program.addInstruction(VMInstruction(Opcode::PushConst, 20));
    program.addInstruction(VMInstruction(Opcode::PushConst, 2));
    program.addInstruction(VMInstruction(Opcode::Mul));
    program.addInstruction(VMInstruction(Opcode::Add));
    program.addInstruction(VMInstruction(Opcode::Print));
    program.addInstruction(VMInstruction(Opcode::Halt));

    std::ostringstream out;
    VirtualMachine vm(out);
    int exitCode = vm.run(program);

    ASSERT_EQ(exitCode, 0);
    ASSERT_EQ(out.str(), "50\n");
}

TEST_CASE(VMComparisonsAndConditionals) {
    BytecodeProgram program;
    // 5 > 3 ? print 1 : print 0
    // 0: PUSH 5
    // 1: PUSH 3
    // 2: GT
    // 3: JUMP_IF_ZERO -> 7
    // 4: PUSH 1
    // 5: PRINT
    // 6: JUMP -> 9
    // 7: PUSH 0
    // 8: PRINT
    // 9: HALT
    program.addInstruction(VMInstruction(Opcode::PushConst, 5));
    program.addInstruction(VMInstruction(Opcode::PushConst, 3));
    program.addInstruction(VMInstruction(Opcode::Greater));
    program.addInstruction(VMInstruction(Opcode::JumpIfZero, 7));
    program.addInstruction(VMInstruction(Opcode::PushConst, 1));
    program.addInstruction(VMInstruction(Opcode::Print));
    program.addInstruction(VMInstruction(Opcode::Jump, 9));
    program.addInstruction(VMInstruction(Opcode::PushConst, 0));
    program.addInstruction(VMInstruction(Opcode::Print));
    program.addInstruction(VMInstruction(Opcode::Halt));

    std::ostringstream out;
    VirtualMachine vm(out);
    int exitCode = vm.run(program);

    ASSERT_EQ(exitCode, 0);
    ASSERT_EQ(out.str(), "1\n");
}

TEST_CASE(VMDivisionByZeroRuntimeError) {
    BytecodeProgram program;
    program.addInstruction(VMInstruction(Opcode::PushConst, 10));
    program.addInstruction(VMInstruction(Opcode::PushConst, 0));
    program.addInstruction(VMInstruction(Opcode::Div));
    program.addInstruction(VMInstruction(Opcode::Halt));

    std::ostringstream out;
    VirtualMachine vm(out);
    int exitCode = vm.run(program);

    // Must return non-zero on error
    ASSERT_NE(exitCode, 0);
}

TEST_CASE(VMStackUnderflowRuntimeError) {
    BytecodeProgram program;
    // Try to ADD with empty stack
    program.addInstruction(VMInstruction(Opcode::Add));
    program.addInstruction(VMInstruction(Opcode::Halt));

    std::ostringstream out;
    VirtualMachine vm(out);
    int exitCode = vm.run(program);

    ASSERT_NE(exitCode, 0);
}

TEST_CASE(VMUninitializedVariableRuntimeError) {
    BytecodeProgram program;
    program.addInstruction(VMInstruction(Opcode::LoadVar, 0, "nonexistent"));
    program.addInstruction(VMInstruction(Opcode::Halt));

    std::ostringstream out;
    VirtualMachine vm(out);
    int exitCode = vm.run(program);

    ASSERT_NE(exitCode, 0);
}
