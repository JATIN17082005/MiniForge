#include "test_framework.hpp"
#include "ir/IR.hpp"
#include "codegen/CodeGenerator.hpp"
#include "codegen/Disassembler.hpp"

using namespace miniforge;

TEST_CASE(CodegenBasicInstructionEmission) {
    IRProgram ir;
    // x = 10
    ir.addInstruction(IRInstruction::makeAssign(
        IROperand::makeVariable("x"),
        IROperand::makeConstant(10)
    ));
    // print x
    ir.addInstruction(IRInstruction::makePrint(
        IROperand::makeVariable("x")
    ));

    CodeGenerator codegen;
    auto bytecode = codegen.generate(ir);

    // Expect: PUSH_CONST 10, STORE_VAR x, LOAD_VAR x, PRINT, HALT
    ASSERT_EQ(bytecode.size(), static_cast<size_t>(5));
    ASSERT_EQ(static_cast<int>(bytecode[0].opcode), static_cast<int>(Opcode::PushConst));
    ASSERT_EQ(bytecode[0].intOperand, 10);
    ASSERT_EQ(static_cast<int>(bytecode[1].opcode), static_cast<int>(Opcode::StoreVar));
    ASSERT_EQ(bytecode[1].strOperand, "x");
    ASSERT_EQ(static_cast<int>(bytecode[2].opcode), static_cast<int>(Opcode::LoadVar));
    ASSERT_EQ(bytecode[2].strOperand, "x");
    ASSERT_EQ(static_cast<int>(bytecode[3].opcode), static_cast<int>(Opcode::Print));
    ASSERT_EQ(static_cast<int>(bytecode[4].opcode), static_cast<int>(Opcode::Halt));
}

TEST_CASE(CodegenLabelResolution) {
    IRProgram ir;
    // jump L_target
    ir.addInstruction(IRInstruction::makeJump("L_target"));
    // x = 1
    ir.addInstruction(IRInstruction::makeAssign(
        IROperand::makeVariable("x"),
        IROperand::makeConstant(1)
    ));
    // L_target:
    ir.addInstruction(IRInstruction::makeLabel("L_target"));
    // print 42
    ir.addInstruction(IRInstruction::makePrint(
        IROperand::makeConstant(42)
    ));

    CodeGenerator codegen;
    auto bytecode = codegen.generate(ir);

    // Jump should resolve to the instruction address of `print 42`
    // Instructions:
    // 0: JUMP (to 3)
    // 1: PUSH_CONST 1
    // 2: STORE_VAR x
    // 3: PUSH_CONST 42
    // 4: PRINT
    // 5: HALT
    ASSERT_EQ(static_cast<int>(bytecode[0].opcode), static_cast<int>(Opcode::Jump));
    ASSERT_EQ(bytecode[0].intOperand, 3);
}

TEST_CASE(CodegenDisassemblerFormatting) {
    BytecodeProgram bytecode;
    bytecode.addInstruction(VMInstruction(Opcode::PushConst, 42));
    bytecode.addInstruction(VMInstruction(Opcode::StoreVar, 0, "ans"));
    bytecode.addInstruction(VMInstruction(Opcode::Halt));

    std::string disasm = Disassembler::disassemble(bytecode);
    ASSERT_TRUE(disasm.find("PUSH_CONST") != std::string::npos);
    ASSERT_TRUE(disasm.find("42") != std::string::npos);
    ASSERT_TRUE(disasm.find("STORE_VAR") != std::string::npos);
    ASSERT_TRUE(disasm.find("ans") != std::string::npos);
    ASSERT_TRUE(disasm.find("HALT") != std::string::npos);
}
