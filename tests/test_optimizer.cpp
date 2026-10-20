#include "test_framework.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "ir/IRBuilder.hpp"
#include "optimizer/ConstantFolder.hpp"
#include "optimizer/ConstantPropagator.hpp"
#include "optimizer/DeadCodeEliminator.hpp"
#include "optimizer/Optimizer.hpp"

using namespace miniforge;

static IRProgram generateIR(const std::string& src) {
    DiagnosticEngine diags;
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens), &diags);
    auto program = parser.parseProgram();
    SemanticAnalyzer sema(diags);
    sema.analyze(*program);
    IRBuilder builder;
    return builder.build(*program);
}

TEST_CASE(OptimizerConstantFolding) {
    IRProgram ir;
    // t0 = 3 * 2
    ir.addInstruction(IRInstruction::makeBinary(
        IROpcode::Mul,
        IROperand::makeTemporary(0),
        IROperand::makeConstant(3),
        IROperand::makeConstant(2)
    ));
    // t1 = 10 / 0 (division by zero should NOT crash or fold)
    ir.addInstruction(IRInstruction::makeBinary(
        IROpcode::Div,
        IROperand::makeTemporary(1),
        IROperand::makeConstant(10),
        IROperand::makeConstant(0)
    ));

    ConstantFolder folder;
    bool modified = folder.run(ir);

    ASSERT_TRUE(modified);
    // t0 should be folded to Assign 6
    ASSERT_EQ(ir.instructions()[0].opcode, IROpcode::Assign);
    ASSERT_EQ(ir.instructions()[0].src1.constantValue(), 6);

    // t1 should remain Div
    ASSERT_EQ(ir.instructions()[1].opcode, IROpcode::Div);
}

TEST_CASE(OptimizerConstantPropagation) {
    IRProgram ir;
    // x = 4
    ir.addInstruction(IRInstruction::makeAssign(
        IROperand::makeVariable("x"),
        IROperand::makeConstant(4)
    ));
    // t0 = x + 3
    ir.addInstruction(IRInstruction::makeBinary(
        IROpcode::Add,
        IROperand::makeTemporary(0),
        IROperand::makeVariable("x"),
        IROperand::makeConstant(3)
    ));

    ConstantPropagator prop;
    bool modified = prop.run(ir);

    ASSERT_TRUE(modified);
    // In t0 = x + 3, x should be replaced with constant 4
    ASSERT_TRUE(ir.instructions()[1].src1.isConstant());
    ASSERT_EQ(ir.instructions()[1].src1.constantValue(), 4);
}

TEST_CASE(OptimizerDeadCodeElimination) {
    IRProgram ir;
    // t0 = 10 (used)
    ir.addInstruction(IRInstruction::makeAssign(
        IROperand::makeTemporary(0),
        IROperand::makeConstant(10)
    ));
    // t1 = 20 (never used, dead!)
    ir.addInstruction(IRInstruction::makeAssign(
        IROperand::makeTemporary(1),
        IROperand::makeConstant(20)
    ));
    // print t0
    ir.addInstruction(IRInstruction::makePrint(
        IROperand::makeTemporary(0)
    ));

    DeadCodeEliminator dce;
    bool modified = dce.run(ir);

    ASSERT_TRUE(modified);
    ASSERT_EQ(ir.instructions().size(), static_cast<size_t>(2));
    // Only t0 and print should remain
    ASSERT_EQ(ir.instructions()[0].dest.tempId(), 0);
    ASSERT_EQ(ir.instructions()[1].opcode, IROpcode::Print);
}

TEST_CASE(OptimizerFullPipelinePromptExample) {
    std::string src = "let x = 4;\n"
                      "let y = 3;\n"
                      "let result = x + y * 2;\n"
                      "print(result);\n";

    auto ir = generateIR(src);
    Optimizer opt;
    opt.optimize(ir);

    // After optimization:
    // result should be assigned constant 10!
    bool foundResultAssign10 = false;
    for (const auto& in : ir.instructions()) {
        if (in.opcode == IROpcode::Assign &&
            in.dest.isVariable() && in.dest.variableName() == "result" &&
            in.src1.isConstant() && in.src1.constantValue() == 10) {
            foundResultAssign10 = true;
        }
    }

    ASSERT_TRUE(foundResultAssign10);
}
