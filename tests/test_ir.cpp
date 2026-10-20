#include "test_framework.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "ir/IRBuilder.hpp"

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

TEST_CASE(IRGenerationArithmeticAndTemporaries) {
    std::string src = "let x = 4;\n"
                      "let y = 3;\n"
                      "let result = x + y * 2;\n";
    auto ir = generateIR(src);
    const auto& instrs = ir.instructions();

    ASSERT_EQ(instrs.size(), static_cast<size_t>(5));

    // x = 4
    ASSERT_EQ(instrs[0].opcode, IROpcode::Assign);
    ASSERT_EQ(instrs[0].dest.variableName(), "x");
    ASSERT_EQ(instrs[0].src1.constantValue(), 4);

    // y = 3
    ASSERT_EQ(instrs[1].opcode, IROpcode::Assign);
    ASSERT_EQ(instrs[1].dest.variableName(), "y");
    ASSERT_EQ(instrs[1].src1.constantValue(), 3);

    // t0 = y * 2
    ASSERT_EQ(instrs[2].opcode, IROpcode::Mul);
    ASSERT_TRUE(instrs[2].dest.isTemporary());
    ASSERT_EQ(instrs[2].src1.variableName(), "y");
    ASSERT_EQ(instrs[2].src2.constantValue(), 2);

    // t1 = x + t0
    ASSERT_EQ(instrs[3].opcode, IROpcode::Add);
    ASSERT_TRUE(instrs[3].dest.isTemporary());
    ASSERT_EQ(instrs[3].src1.variableName(), "x");
    ASSERT_EQ(instrs[3].src2.tempId(), instrs[2].dest.tempId());

    // result = t1
    ASSERT_EQ(instrs[4].opcode, IROpcode::Assign);
    ASSERT_EQ(instrs[4].dest.variableName(), "result");
    ASSERT_EQ(instrs[4].src1.tempId(), instrs[3].dest.tempId());
}

TEST_CASE(IRGenerationIfElseBranching) {
    std::string src = "let x = 10;\n"
                      "if (x > 5) {\n"
                      "    print(1);\n"
                      "} else {\n"
                      "    print(0);\n"
                      "}\n";
    auto ir = generateIR(src);
    const auto& instrs = ir.instructions();

    // Check that we have a comparison, a jump_if_zero, print, jump, label, print, label
    bool hasComparison = false;
    bool hasJumpIfZero = false;
    bool hasJump = false;
    int labelCount = 0;

    for (const auto& in : instrs) {
        if (in.opcode == IROpcode::Greater) hasComparison = true;
        if (in.opcode == IROpcode::JumpIfZero) hasJumpIfZero = true;
        if (in.opcode == IROpcode::Jump) hasJump = true;
        if (in.opcode == IROpcode::Label) labelCount++;
    }

    ASSERT_TRUE(hasComparison);
    ASSERT_TRUE(hasJumpIfZero);
    ASSERT_TRUE(hasJump);
    ASSERT_EQ(labelCount, 2); // else label + end label
}

TEST_CASE(IRGenerationWhileLoop) {
    std::string src = "let i = 0;\n"
                      "while (i < 5) {\n"
                      "    i = i + 1;\n"
                      "}\n";
    auto ir = generateIR(src);
    const auto& instrs = ir.instructions();

    bool hasLoopStartLabel = false;
    bool hasLoopEndLabel = false;
    bool hasJumpIfZero = false;
    bool hasJumpBack = false;

    for (const auto& in : instrs) {
        if (in.opcode == IROpcode::Label) {
            if (in.labelName.find("loop_start") != std::string::npos) hasLoopStartLabel = true;
            if (in.labelName.find("loop_end") != std::string::npos) hasLoopEndLabel = true;
        }
        if (in.opcode == IROpcode::JumpIfZero) hasJumpIfZero = true;
        if (in.opcode == IROpcode::Jump) hasJumpBack = true;
    }

    ASSERT_TRUE(hasLoopStartLabel);
    ASSERT_TRUE(hasLoopEndLabel);
    ASSERT_TRUE(hasJumpIfZero);
    ASSERT_TRUE(hasJumpBack);
}
