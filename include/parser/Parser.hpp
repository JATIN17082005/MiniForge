#ifndef MINIFORGE_PARSER_PARSER_HPP
#define MINIFORGE_PARSER_PARSER_HPP

#include "lexer/Token.hpp"
#include "ast/AST.hpp"
#include "common/Diagnostic.hpp"
#include <vector>
#include <memory>
#include <string>

namespace miniforge {

class Parser {
public:
    Parser(std::vector<Token> tokens, DiagnosticEngine* diags = nullptr);

    // Parses the full program
    std::unique_ptr<Program> parseProgram();

    // Expression parsing entry point (useful for isolated unit testing)
    std::unique_ptr<Expr> parseExpression();

    bool hasErrors() const {
        return diags_ ? diags_->hasErrors() : hasInternalError_;
    }

private:
    // Token navigation
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    const Token& consume(TokenType type, const std::string& errorMessage);

    // Parsing statements
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<VarDeclStmt> parseVarDecl();
    std::unique_ptr<AssignStmt> parseAssignment();
    std::unique_ptr<IfStmt> parseIfStatement();
    std::unique_ptr<WhileStmt> parseWhileStatement();
    std::unique_ptr<PrintStmt> parsePrintStatement();
    std::unique_ptr<BlockStmt> parseBlockStatement();

    // Parsing expressions with precedence climbing
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseRelational();
    std::unique_ptr<Expr> parseAdditive();
    std::unique_ptr<Expr> parseMultiplicative();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();

    // Error handling & synchronization
    void reportError(const Token& token, const std::string& message);
    void synchronize();

    std::vector<Token> tokens_;
    size_t current_{0};
    DiagnosticEngine* diags_{nullptr};
    bool hasInternalError_{false};
};

} // namespace miniforge

#endif // MINIFORGE_PARSER_PARSER_HPP
