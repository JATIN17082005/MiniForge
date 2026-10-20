#include "test_framework.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticAnalyzer.hpp"

using namespace miniforge;

static bool analyzeString(const std::string& src, DiagnosticEngine& diags) {
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens), &diags);
    auto program = parser.parseProgram();
    if (diags.hasErrors() || !program) {
        return false;
    }
    SemanticAnalyzer sema(diags);
    return sema.analyze(*program);
}

TEST_CASE(SemanticValidDeclarationsAndScopes) {
    std::string src = "let x = 10;\n"
                      "let y = x + 5;\n"
                      "{\n"
                      "    let z = y * 2;\n"
                      "    let x = 20; // shadowing outer x\n"
                      "    print(x + z);\n"
                      "}\n"
                      "print(x);\n";

    DiagnosticEngine diags;
    bool success = analyzeString(src, diags);
    ASSERT_TRUE(success);
    ASSERT_FALSE(diags.hasErrors());
}

TEST_CASE(SemanticUndeclaredVariableInExpr) {
    std::string src = "let x = 10;\n"
                      "let y = x + undefined_var;\n";

    DiagnosticEngine diags;
    bool success = analyzeString(src, diags);
    ASSERT_FALSE(success);
    ASSERT_TRUE(diags.hasErrors());
}

TEST_CASE(SemanticUndeclaredVariableInAssignment) {
    std::string src = "unknown_var = 42;\n";

    DiagnosticEngine diags;
    bool success = analyzeString(src, diags);
    ASSERT_FALSE(success);
    ASSERT_TRUE(diags.hasErrors());
}

TEST_CASE(SemanticDuplicateDeclarationInSameScope) {
    std::string src = "let a = 1;\n"
                      "let a = 2;\n";

    DiagnosticEngine diags;
    bool success = analyzeString(src, diags);
    ASSERT_FALSE(success);
    ASSERT_TRUE(diags.hasErrors());
}

TEST_CASE(SemanticOutOfScopeVariableAccess) {
    std::string src = "{\n"
                      "    let inside = 100;\n"
                      "}\n"
                      "print(inside); // Error: inside went out of scope\n";

    DiagnosticEngine diags;
    bool success = analyzeString(src, diags);
    ASSERT_FALSE(success);
    ASSERT_TRUE(diags.hasErrors());
}
