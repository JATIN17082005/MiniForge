#include "test_framework.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

using namespace miniforge;

static std::unique_ptr<Program> parseString(const std::string& src, DiagnosticEngine& diags) {
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();
    Parser parser(std::move(tokens), &diags);
    return parser.parseProgram();
}

TEST_CASE(ParserOperatorPrecedence) {
    DiagnosticEngine diags;
    auto program = parseString("let result = 2 + 3 * 4;", diags);

    ASSERT_FALSE(diags.hasErrors());
    ASSERT_EQ(program->statements().size(), static_cast<size_t>(1));

    auto* varDecl = dynamic_cast<VarDeclStmt*>(program->statements()[0].get());
    ASSERT_TRUE(varDecl != nullptr);
    ASSERT_EQ(varDecl->name(), "result");

    // The root of the expression should be BinaryExpr(+)
    auto* addExpr = dynamic_cast<BinaryExpr*>(&varDecl->init());
    ASSERT_TRUE(addExpr != nullptr);
    ASSERT_EQ(addExpr->op(), TokenType::Plus);

    // Left child should be literal 2
    auto* leftLit = dynamic_cast<IntegerLiteralExpr*>(&addExpr->left());
    ASSERT_TRUE(leftLit != nullptr);
    ASSERT_EQ(leftLit->value(), 2);

    // Right child should be BinaryExpr(*)
    auto* mulExpr = dynamic_cast<BinaryExpr*>(&addExpr->right());
    ASSERT_TRUE(mulExpr != nullptr);
    ASSERT_EQ(mulExpr->op(), TokenType::Star);
}

TEST_CASE(ParserParenthesesPrecedence) {
    DiagnosticEngine diags;
    auto program = parseString("let result = (2 + 3) * 4;", diags);

    ASSERT_FALSE(diags.hasErrors());
    auto* varDecl = dynamic_cast<VarDeclStmt*>(program->statements()[0].get());
    ASSERT_TRUE(varDecl != nullptr);

    // The root of expression should be BinaryExpr(*)
    auto* mulExpr = dynamic_cast<BinaryExpr*>(&varDecl->init());
    ASSERT_TRUE(mulExpr != nullptr);
    ASSERT_EQ(mulExpr->op(), TokenType::Star);

    // Left child should be BinaryExpr(+)
    auto* addExpr = dynamic_cast<BinaryExpr*>(&mulExpr->left());
    ASSERT_TRUE(addExpr != nullptr);
    ASSERT_EQ(addExpr->op(), TokenType::Plus);
}

TEST_CASE(ParserConditionalsAndLoops) {
    std::string src = "if (x > 0) {\n"
                      "    print(x);\n"
                      "} else {\n"
                      "    print(0);\n"
                      "}\n"
                      "while (x < 10) {\n"
                      "    x = x + 1;\n"
                      "}\n";

    DiagnosticEngine diags;
    auto program = parseString(src, diags);

    ASSERT_FALSE(diags.hasErrors());
    ASSERT_EQ(program->statements().size(), static_cast<size_t>(2));

    auto* ifStmt = dynamic_cast<IfStmt*>(program->statements()[0].get());
    ASSERT_TRUE(ifStmt != nullptr);
    ASSERT_TRUE(ifStmt->hasElse());
    ASSERT_EQ(ifStmt->thenBranch().statements().size(), static_cast<size_t>(1));
    ASSERT_EQ(ifStmt->elseBranch()->statements().size(), static_cast<size_t>(1));

    auto* whileStmt = dynamic_cast<WhileStmt*>(program->statements()[1].get());
    ASSERT_TRUE(whileStmt != nullptr);
    ASSERT_EQ(whileStmt->body().statements().size(), static_cast<size_t>(1));
}

TEST_CASE(ParserUnaryMinus) {
    DiagnosticEngine diags;
    auto program = parseString("let x = -5 + -(-a);", diags);

    ASSERT_FALSE(diags.hasErrors());
    auto* varDecl = dynamic_cast<VarDeclStmt*>(program->statements()[0].get());
    ASSERT_TRUE(varDecl != nullptr);

    auto* addExpr = dynamic_cast<BinaryExpr*>(&varDecl->init());
    ASSERT_TRUE(addExpr != nullptr);

    auto* unaryLeft = dynamic_cast<UnaryExpr*>(&addExpr->left());
    ASSERT_TRUE(unaryLeft != nullptr);
    ASSERT_EQ(unaryLeft->op(), TokenType::Minus);
}

TEST_CASE(ParserSyntaxErrorRecovery) {
    std::string src = "let = 4; \n"       // Missing identifier
                      "let y = 10; \n"     // Valid
                      "print(; \n"         // Missing expr/closing paren
                      "let z = 20; \n";    // Valid

    DiagnosticEngine diags;
    auto program = parseString(src, diags);

    ASSERT_TRUE(diags.hasErrors());
    // The parser should have recovered and parsed the two valid statements 'y' and 'z'
    ASSERT_TRUE(program->statements().size() >= static_cast<size_t>(2));
}
