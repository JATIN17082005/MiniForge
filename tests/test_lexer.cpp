#include "test_framework.hpp"
#include "lexer/Lexer.hpp"

using namespace miniforge;

TEST_CASE(LexerKeywordsAndIdentifiers) {
    std::string src = "let x = 42; if while else print my_var123";
    DiagnosticEngine diags;
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();

    ASSERT_FALSE(diags.hasErrors());
    ASSERT_EQ(tokens.size(), static_cast<size_t>(11)); // 10 tokens + EOF

    ASSERT_EQ(tokens[0].type, TokenType::KwLet);
    ASSERT_EQ(tokens[1].type, TokenType::Identifier);
    ASSERT_EQ(tokens[1].lexeme, "x");
    ASSERT_EQ(tokens[2].type, TokenType::Equal);
    ASSERT_EQ(tokens[3].type, TokenType::IntegerLiteral);
    ASSERT_EQ(tokens[3].intValue, 42);
    ASSERT_EQ(tokens[4].type, TokenType::Semicolon);
    ASSERT_EQ(tokens[5].type, TokenType::KwIf);
    ASSERT_EQ(tokens[6].type, TokenType::KwWhile);
    ASSERT_EQ(tokens[7].type, TokenType::KwElse);
    ASSERT_EQ(tokens[8].type, TokenType::KwPrint);
    ASSERT_EQ(tokens[9].type, TokenType::Identifier);
    ASSERT_EQ(tokens[9].lexeme, "my_var123");
}

TEST_CASE(LexerOperatorsAndPunctuation) {
    std::string src = "+ - * / % == != < <= > >= ( ) { } , ;";
    DiagnosticEngine diags;
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();

    ASSERT_FALSE(diags.hasErrors());
    ASSERT_EQ(tokens.size(), static_cast<size_t>(18)); // 17 tokens + EOF

    ASSERT_EQ(tokens[0].type, TokenType::Plus);
    ASSERT_EQ(tokens[1].type, TokenType::Minus);
    ASSERT_EQ(tokens[2].type, TokenType::Star);
    ASSERT_EQ(tokens[3].type, TokenType::Slash);
    ASSERT_EQ(tokens[4].type, TokenType::Percent);
    ASSERT_EQ(tokens[5].type, TokenType::EqualEqual);
    ASSERT_EQ(tokens[6].type, TokenType::BangEqual);
    ASSERT_EQ(tokens[7].type, TokenType::Less);
    ASSERT_EQ(tokens[8].type, TokenType::LessEqual);
    ASSERT_EQ(tokens[9].type, TokenType::Greater);
    ASSERT_EQ(tokens[10].type, TokenType::GreaterEqual);
    ASSERT_EQ(tokens[11].type, TokenType::LParen);
    ASSERT_EQ(tokens[12].type, TokenType::RParen);
    ASSERT_EQ(tokens[13].type, TokenType::LBrace);
    ASSERT_EQ(tokens[14].type, TokenType::RBrace);
    ASSERT_EQ(tokens[15].type, TokenType::Comma);
    ASSERT_EQ(tokens[16].type, TokenType::Semicolon);
}

TEST_CASE(LexerCommentsAndLineColTracking) {
    std::string src = "// Initial comment\n"
                      "let a = 10; // inline comment\n"
                      "\n"
                      "print(a);\n";
    DiagnosticEngine diags;
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();

    ASSERT_FALSE(diags.hasErrors());

    // Line 2: let a = 10;
    ASSERT_EQ(tokens[0].type, TokenType::KwLet);
    ASSERT_EQ(tokens[0].location.line, static_cast<size_t>(2));
    ASSERT_EQ(tokens[0].location.column, static_cast<size_t>(1));

    ASSERT_EQ(tokens[1].type, TokenType::Identifier);
    ASSERT_EQ(tokens[1].location.line, static_cast<size_t>(2));
    ASSERT_EQ(tokens[1].location.column, static_cast<size_t>(5));

    // Line 4: print(a);
    ASSERT_EQ(tokens[5].type, TokenType::KwPrint);
    ASSERT_EQ(tokens[5].location.line, static_cast<size_t>(4));
    ASSERT_EQ(tokens[5].location.column, static_cast<size_t>(1));
}

TEST_CASE(LexerErrorHandling) {
    std::string src = "let @ = ! 5;";
    DiagnosticEngine diags;
    Lexer lexer(src, &diags);
    auto tokens = lexer.tokenize();

    ASSERT_TRUE(diags.hasErrors());
    ASSERT_EQ(diags.getDiagnostics().size(), static_cast<size_t>(2)); // '@' and '!'
}
