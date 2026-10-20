#include "lexer/Token.hpp"
#include <sstream>

namespace miniforge {

std::string_view tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KwLet:          return "LET";
        case TokenType::KwIf:           return "IF";
        case TokenType::KwElse:         return "ELSE";
        case TokenType::KwWhile:        return "WHILE";
        case TokenType::KwPrint:        return "PRINT";
        case TokenType::Identifier:     return "IDENTIFIER";
        case TokenType::IntegerLiteral: return "INTEGER_LITERAL";
        case TokenType::Plus:           return "+";
        case TokenType::Minus:          return "-";
        case TokenType::Star:           return "*";
        case TokenType::Slash:          return "/";
        case TokenType::Percent:        return "%";
        case TokenType::EqualEqual:     return "==";
        case TokenType::BangEqual:      return "!=";
        case TokenType::Less:           return "<";
        case TokenType::LessEqual:      return "<=";
        case TokenType::Greater:        return ">";
        case TokenType::GreaterEqual:   return ">=";
        case TokenType::Equal:          return "=";
        case TokenType::LParen:         return "(";
        case TokenType::RParen:         return ")";
        case TokenType::LBrace:         return "{";
        case TokenType::RBrace:         return "}";
        case TokenType::Semicolon:      return ";";
        case TokenType::Comma:          return ",";
        case TokenType::EndOfFile:      return "EOF";
        case TokenType::Invalid:        return "INVALID";
    }
    return "UNKNOWN";
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "[" << location.toString() << "] " << tokenTypeToString(type);
    if (!lexeme.empty()) {
        oss << " '" << lexeme << "'";
    }
    if (type == TokenType::IntegerLiteral) {
        oss << " (val=" << intValue << ")";
    }
    return oss.str();
}

} // namespace miniforge
