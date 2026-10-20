#ifndef MINIFORGE_LEXER_LEXER_HPP
#define MINIFORGE_LEXER_LEXER_HPP

#include "lexer/Token.hpp"
#include "common/Diagnostic.hpp"
#include <string>
#include <vector>

namespace miniforge {

class Lexer {
public:
    explicit Lexer(std::string source, DiagnosticEngine* diags = nullptr);

    // Tokenizes the entire source and returns a vector of tokens ending with EndOfFile
    std::vector<Token> tokenize();

    // Scan the next single token
    Token nextToken();

    bool hasErrors() const {
        return diags_ ? diags_->hasErrors() : hasInternalError_;
    }

private:
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token scanIdentifierOrKeyword();
    Token scanNumber();

    std::string source_;
    size_t start_{0};
    size_t current_{0};
    size_t line_{1};
    size_t column_{1};
    size_t tokenStartColumn_{1};

    DiagnosticEngine* diags_{nullptr};
    bool hasInternalError_{false};

    void reportError(SourceLocation loc, const std::string& message);
};

} // namespace miniforge

#endif // MINIFORGE_LEXER_LEXER_HPP
