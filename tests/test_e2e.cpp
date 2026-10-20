#include "test_framework.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "ir/IRBuilder.hpp"
#include "optimizer/Optimizer.hpp"
#include "codegen/CodeGenerator.hpp"
#include "vm/VM.hpp"
#include <sstream>

using namespace miniforge;

static std::pair<int, std::string> compileAndRun(const std::string& src, bool optimize = true) {
    DiagnosticEngine diags;
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();
    if (diags.hasErrors()) return {1, "Lexer error"};

    Parser parser(std::move(tokens), &diags);
    auto program = parser.parseProgram();
    if (diags.hasErrors() || !program) return {1, "Parser error"};

    SemanticAnalyzer sema(diags);
    if (!sema.analyze(*program)) return {1, "Semantic error"};

    IRBuilder builder;
    auto ir = builder.build(*program);

    if (optimize) {
        Optimizer opt;
        opt.optimize(ir);
    }

    CodeGenerator codegen(&diags);
    auto bytecode = codegen.generate(ir);
    if (diags.hasErrors()) return {1, "Codegen error"};

    std::ostringstream out;
    VirtualMachine vm(out);
    int code = vm.run(bytecode);
    return {code, out.str()};
}

TEST_CASE(E2EPromptBasicExample) {
    std::string src = "let x = 4;\n"
                      "let y = 3;\n"
                      "let result = x + y * 2;\n"
                      "\n"
                      "if (result > 8) {\n"
                      "    print(result);\n"
                      "} else {\n"
                      "    print(0);\n"
                      "}\n";

    // Test with optimization enabled
    auto [codeOpt, outOpt] = compileAndRun(src, true);
    ASSERT_EQ(codeOpt, 0);
    ASSERT_EQ(outOpt, "10\n");

    // Test with optimization disabled
    auto [codeNoOpt, outNoOpt] = compileAndRun(src, false);
    ASSERT_EQ(codeNoOpt, 0);
    ASSERT_EQ(outNoOpt, "10\n");
}

TEST_CASE(E2EWhileLoopSummation) {
    std::string src = "let sum = 0;\n"
                      "let i = 1;\n"
                      "while (i <= 5) {\n"
                      "    sum = sum + i;\n"
                      "    i = i + 1;\n"
                      "}\n"
                      "print(sum);\n";

    auto [code, out] = compileAndRun(src, true);
    ASSERT_EQ(code, 0);
    ASSERT_EQ(out, "15\n");
}

TEST_CASE(E2EFactorialCalculation) {
    std::string src = "let n = 5;\n"
                      "let fact = 1;\n"
                      "while (n > 1) {\n"
                      "    fact = fact * n;\n"
                      "    n = n - 1;\n"
                      "}\n"
                      "print(fact);\n";

    auto [code, out] = compileAndRun(src, true);
    ASSERT_EQ(code, 0);
    ASSERT_EQ(out, "120\n");
}

TEST_CASE(E2EScopeShadowing) {
    std::string src = "let a = 10;\n"
                      "{\n"
                      "    let a = 20;\n"
                      "    print(a);\n"
                      "}\n"
                      "print(a);\n";

    auto [code, out] = compileAndRun(src, true);
    ASSERT_EQ(code, 0);
    ASSERT_EQ(out, "20\n10\n");
}

TEST_CASE(E2EModuloAndUnaryMinus) {
    std::string src = "let rem = 17 % 5;\n"
                      "let neg = -10 + 4;\n"
                      "print(rem);\n"
                      "print(neg);\n";

    auto [code, out] = compileAndRun(src, true);
    ASSERT_EQ(code, 0);
    ASSERT_EQ(out, "2\n-6\n");
}
