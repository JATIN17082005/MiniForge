#include "parser/Parser.hpp"

namespace miniforge {

struct ParseError {};

Parser::Parser(std::vector<Token> tokens, DiagnosticEngine* diags)
    : tokens_(std::move(tokens)), diags_(diags) {}

const Token& Parser::peek() const {
    if (current_ >= tokens_.size()) {
        return tokens_.back(); // EndOfFile token
    }
    return tokens_[current_];
}

const Token& Parser::previous() const {
    if (current_ == 0) {
        return tokens_.front();
    }
    return tokens_[current_ - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return type == TokenType::EndOfFile;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void Parser::reportError(const Token& token, const std::string& message) {
    hasInternalError_ = true;
    if (diags_) {
        diags_->error(token.location, message);
    }
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        return advance();
    }
    reportError(peek(), errorMessage);
    throw ParseError{};
}

void Parser::synchronize() {
    advance();

    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) {
            return;
        }

        switch (peek().type) {
            case TokenType::KwLet:
            case TokenType::KwIf:
            case TokenType::KwWhile:
            case TokenType::KwPrint:
            case TokenType::RBrace:
                return;
            default:
                advance();
                break;
        }
    }
}

std::unique_ptr<Program> Parser::parseProgram() {
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const ParseError&) {
            synchronize();
        }
    }

    return std::make_unique<Program>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    if (check(TokenType::KwLet)) {
        return parseVarDecl();
    }
    if (check(TokenType::KwIf)) {
        return parseIfStatement();
    }
    if (check(TokenType::KwWhile)) {
        return parseWhileStatement();
    }
    if (check(TokenType::KwPrint)) {
        return parsePrintStatement();
    }
    if (check(TokenType::LBrace)) {
        return parseBlockStatement();
    }
    if (check(TokenType::Identifier)) {
        return parseAssignment();
    }

    reportError(peek(), "Expected statement, found '" + peek().lexeme + "'");
    throw ParseError{};
}

std::unique_ptr<VarDeclStmt> Parser::parseVarDecl() {
    SourceLocation loc = peek().location;
    consume(TokenType::KwLet, "Expected 'let'");

    const Token& idTok = consume(TokenType::Identifier, "Expected variable name after 'let'");
    std::string name = idTok.lexeme;

    consume(TokenType::Equal, "Expected '=' in variable declaration");
    auto init = parseExpression();
    consume(TokenType::Semicolon, "Expected ';' after variable declaration");

    return std::make_unique<VarDeclStmt>(std::move(name), std::move(init), loc);
}

std::unique_ptr<AssignStmt> Parser::parseAssignment() {
    const Token& idTok = consume(TokenType::Identifier, "Expected variable name");
    std::string name = idTok.lexeme;
    SourceLocation loc = idTok.location;

    consume(TokenType::Equal, "Expected '=' in assignment");
    auto value = parseExpression();
    consume(TokenType::Semicolon, "Expected ';' after assignment");

    return std::make_unique<AssignStmt>(std::move(name), std::move(value), loc);
}

std::unique_ptr<IfStmt> Parser::parseIfStatement() {
    SourceLocation loc = peek().location;
    consume(TokenType::KwIf, "Expected 'if'");
    consume(TokenType::LParen, "Expected '(' after 'if'");
    auto cond = parseExpression();
    consume(TokenType::RParen, "Expected ')' after if condition");

    auto thenBranch = parseBlockStatement();
    std::unique_ptr<BlockStmt> elseBranch = nullptr;

    if (match(TokenType::KwElse)) {
        elseBranch = parseBlockStatement();
    }

    return std::make_unique<IfStmt>(std::move(cond), std::move(thenBranch), std::move(elseBranch), loc);
}

std::unique_ptr<WhileStmt> Parser::parseWhileStatement() {
    SourceLocation loc = peek().location;
    consume(TokenType::KwWhile, "Expected 'while'");
    consume(TokenType::LParen, "Expected '(' after 'while'");
    auto cond = parseExpression();
    consume(TokenType::RParen, "Expected ')' after while condition");

    auto body = parseBlockStatement();

    return std::make_unique<WhileStmt>(std::move(cond), std::move(body), loc);
}

std::unique_ptr<PrintStmt> Parser::parsePrintStatement() {
    SourceLocation loc = peek().location;
    consume(TokenType::KwPrint, "Expected 'print'");
    consume(TokenType::LParen, "Expected '(' after 'print'");
    auto expr = parseExpression();
    consume(TokenType::RParen, "Expected ')' after print argument");
    consume(TokenType::Semicolon, "Expected ';' after print statement");

    return std::make_unique<PrintStmt>(std::move(expr), loc);
}

std::unique_ptr<BlockStmt> Parser::parseBlockStatement() {
    SourceLocation loc = peek().location;
    consume(TokenType::LBrace, "Expected '{' to start block");

    std::vector<std::unique_ptr<Stmt>> statements;
    while (!check(TokenType::RBrace) && !isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const ParseError&) {
            synchronize();
        }
    }

    consume(TokenType::RBrace, "Expected '}' to end block");
    return std::make_unique<BlockStmt>(std::move(statements), loc);
}

std::unique_ptr<Expr> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<Expr> Parser::parseEquality() {
    auto expr = parseRelational();

    while (match({TokenType::EqualEqual, TokenType::BangEqual})) {
        TokenType op = previous().type;
        SourceLocation loc = previous().location;
        auto right = parseRelational();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), loc);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseRelational() {
    auto expr = parseAdditive();

    while (match({TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual})) {
        TokenType op = previous().type;
        SourceLocation loc = previous().location;
        auto right = parseAdditive();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), loc);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseAdditive() {
    auto expr = parseMultiplicative();

    while (match({TokenType::Plus, TokenType::Minus})) {
        TokenType op = previous().type;
        SourceLocation loc = previous().location;
        auto right = parseMultiplicative();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), loc);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseMultiplicative() {
    auto expr = parseUnary();

    while (match({TokenType::Star, TokenType::Slash, TokenType::Percent})) {
        TokenType op = previous().type;
        SourceLocation loc = previous().location;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), loc);
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::Minus)) {
        TokenType op = previous().type;
        SourceLocation loc = previous().location;
        auto operand = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(operand), loc);
    }

    return parsePrimary();
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::IntegerLiteral)) {
        return std::make_unique<IntegerLiteralExpr>(previous().intValue, previous().location);
    }

    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpr>(previous().lexeme, previous().location);
    }

    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        consume(TokenType::RParen, "Expected ')' after parenthesized expression");
        return expr;
    }

    reportError(peek(), "Expected expression, found '" + peek().lexeme + "'");
    throw ParseError{};
}

} // namespace miniforge
