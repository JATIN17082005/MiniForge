#ifndef MINIFORGE_LEXER_TOKEN_HPP
#define MINIFORGE_LEXER_TOKEN_HPP

#include "common/SourceLocation.hpp"
#include <string>
#include <cstdint>
#include <string_view>

namespace miniforge {

enum class TokenType {
    // Keywords
    KwLet,
    KwIf,
    KwElse,
    KwWhile,
    KwPrint,

    // Identifiers and Literals
    Identifier,
    IntegerLiteral,

    // Arithmetic Operators
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Percent,        // %

    // Comparison Operators
    EqualEqual,     // ==
    BangEqual,      // !=
    Less,           // <
    LessEqual,      // <=
    Greater,        // >
    GreaterEqual,   // >=

    // Assignment
    Equal,          // =

    // Delimiters / Punctuation
    LParen,         // (
    RParen,         // )
    LBrace,         // {
    RBrace,         // }
    Semicolon,      // ;
    Comma,          // ,

    // End-of-file & Invalid
    EndOfFile,
    Invalid
};

std::string_view tokenTypeToString(TokenType type);

inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << tokenTypeToString(type);
    return os;
}

struct Token {
    TokenType type{TokenType::Invalid};
    std::string lexeme;
    int64_t intValue{0};
    SourceLocation location;

    Token() = default;
    Token(TokenType t, std::string lex, SourceLocation loc, int64_t val = 0)
        : type(t), lexeme(std::move(lex)), intValue(val), location(loc) {}

    std::string toString() const;
};

} // namespace miniforge

#endif // MINIFORGE_LEXER_TOKEN_HPP
