#include "lexer/Lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace miniforge {

static const std::unordered_map<std::string_view, TokenType> kKeywords = {
    {"let",   TokenType::KwLet},
    {"if",    TokenType::KwIf},
    {"else",  TokenType::KwElse},
    {"while", TokenType::KwWhile},
    {"print", TokenType::KwPrint}
};

Lexer::Lexer(std::string source, DiagnosticEngine* diags)
    : source_(std::move(source)), diags_(diags) {}

bool Lexer::isAtEnd() const {
    return current_ >= source_.size();
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.size()) return '\0';
    return source_[current_ + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char c = source_[current_++];
    column_++;
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source_[current_] != expected) {
        return false;
    }
    current_++;
    column_++;
    return true;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            current_++;
            line_++;
            column_ = 1;
        } else if (c == '/' && peekNext() == '/') {
            // Line comment: skip until newline or EOF
            advance(); // '/'
            advance(); // '/'
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

void Lexer::reportError(SourceLocation loc, const std::string& message) {
    hasInternalError_ = true;
    if (diags_) {
        diags_->error(loc, message);
    }
}

Token Lexer::scanIdentifierOrKeyword() {
    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    SourceLocation loc(line_, tokenStartColumn_);

    auto it = kKeywords.find(text);
    if (it != kKeywords.end()) {
        return Token(it->second, std::move(text), loc);
    }

    return Token(TokenType::Identifier, std::move(text), loc);
}

Token Lexer::scanNumber() {
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    SourceLocation loc(line_, tokenStartColumn_);

    int64_t val = 0;
    try {
        val = std::stoll(text);
    } catch (const std::out_of_range&) {
        reportError(loc, "Integer literal '" + text + "' overflows 64-bit integer");
        return Token(TokenType::Invalid, std::move(text), loc);
    }

    return Token(TokenType::IntegerLiteral, std::move(text), loc, val);
}

Token Lexer::nextToken() {
    skipWhitespaceAndComments();

    start_ = current_;
    tokenStartColumn_ = column_;

    if (isAtEnd()) {
        return Token(TokenType::EndOfFile, "", SourceLocation(line_, column_));
    }

    char c = advance();

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        return scanIdentifierOrKeyword();
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        return scanNumber();
    }

    SourceLocation loc(line_, tokenStartColumn_);

    switch (c) {
        case '+': return Token(TokenType::Plus, "+", loc);
        case '-': return Token(TokenType::Minus, "-", loc);
        case '*': return Token(TokenType::Star, "*", loc);
        case '/': return Token(TokenType::Slash, "/", loc);
        case '%': return Token(TokenType::Percent, "%", loc);
        case '(': return Token(TokenType::LParen, "(", loc);
        case ')': return Token(TokenType::RParen, ")", loc);
        case '{': return Token(TokenType::LBrace, "{", loc);
        case '}': return Token(TokenType::RBrace, "}", loc);
        case ';': return Token(TokenType::Semicolon, ";", loc);
        case ',': return Token(TokenType::Comma, ",", loc);
        case '=':
            if (match('=')) return Token(TokenType::EqualEqual, "==", loc);
            return Token(TokenType::Equal, "=", loc);
        case '!':
            if (match('=')) return Token(TokenType::BangEqual, "!=", loc);
            reportError(loc, "Unexpected character '!' (did you mean '!='?)");
            return Token(TokenType::Invalid, "!", loc);
        case '<':
            if (match('=')) return Token(TokenType::LessEqual, "<=", loc);
            return Token(TokenType::Less, "<", loc);
        case '>':
            if (match('=')) return Token(TokenType::GreaterEqual, ">=", loc);
            return Token(TokenType::Greater, ">", loc);
        default:
            reportError(loc, std::string("Unexpected character '") + c + "'");
            return Token(TokenType::Invalid, std::string(1, c), loc);
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token tok = nextToken();
        tokens.push_back(tok);
        if (tok.type == TokenType::EndOfFile) {
            break;
        }
    }
    return tokens;
}

} // namespace miniforge
