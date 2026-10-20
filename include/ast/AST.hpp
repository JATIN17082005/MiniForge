#ifndef MINIFORGE_AST_AST_HPP
#define MINIFORGE_AST_AST_HPP

#include "common/SourceLocation.hpp"
#include "lexer/Token.hpp"
#include "ast/ASTVisitor.hpp"
#include <memory>
#include <string>
#include <vector>

namespace miniforge {

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual SourceLocation location() const = 0;
    virtual void accept(ASTVisitor& visitor) = 0;
    virtual std::string toString(int indent = 0) const = 0;
};

// Base class for all Expressions
class Expr : public ASTNode {
public:
    ~Expr() override = default;
};

// Base class for all Statements
class Stmt : public ASTNode {
public:
    ~Stmt() override = default;
};

// Expression: Integer Literal
class IntegerLiteralExpr : public Expr {
public:
    IntegerLiteralExpr(int64_t val, SourceLocation loc)
        : value_(val), location_(loc) {}

    int64_t value() const { return value_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    int64_t value_;
    SourceLocation location_;
};

// Expression: Variable Reference
class VariableExpr : public Expr {
public:
    VariableExpr(std::string name, SourceLocation loc)
        : name_(std::move(name)), location_(loc) {}

    const std::string& name() const { return name_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::string name_;
    SourceLocation location_;
};

// Expression: Unary Expression (-x)
class UnaryExpr : public Expr {
public:
    UnaryExpr(TokenType op, std::unique_ptr<Expr> operand, SourceLocation loc)
        : op_(op), operand_(std::move(operand)), location_(loc) {}

    TokenType op() const { return op_; }
    const Expr& operand() const { return *operand_; }
    Expr& operand() { return *operand_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    TokenType op_;
    std::unique_ptr<Expr> operand_;
    SourceLocation location_;
};

// Expression: Binary Expression (left op right)
class BinaryExpr : public Expr {
public:
    BinaryExpr(TokenType op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, SourceLocation loc)
        : op_(op), left_(std::move(left)), right_(std::move(right)), location_(loc) {}

    TokenType op() const { return op_; }
    const Expr& left() const { return *left_; }
    Expr& left() { return *left_; }
    const Expr& right() const { return *right_; }
    Expr& right() { return *right_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    TokenType op_;
    std::unique_ptr<Expr> left_;
    std::unique_ptr<Expr> right_;
    SourceLocation location_;
};

// Statement: Variable Declaration (let x = expr;)
class VarDeclStmt : public Stmt {
public:
    VarDeclStmt(std::string name, std::unique_ptr<Expr> init, SourceLocation loc)
        : name_(std::move(name)), init_(std::move(init)), location_(loc) {}

    const std::string& name() const { return name_; }
    const Expr& init() const { return *init_; }
    Expr& init() { return *init_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::string name_;
    std::unique_ptr<Expr> init_;
    SourceLocation location_;
};

// Statement: Assignment (x = expr;)
class AssignStmt : public Stmt {
public:
    AssignStmt(std::string name, std::unique_ptr<Expr> value, SourceLocation loc)
        : name_(std::move(name)), value_(std::move(value)), location_(loc) {}

    const std::string& name() const { return name_; }
    const Expr& value() const { return *value_; }
    Expr& value() { return *value_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::string name_;
    std::unique_ptr<Expr> value_;
    SourceLocation location_;
};

// Statement: Print (print(expr);)
class PrintStmt : public Stmt {
public:
    PrintStmt(std::unique_ptr<Expr> expr, SourceLocation loc)
        : expr_(std::move(expr)), location_(loc) {}

    const Expr& expr() const { return *expr_; }
    Expr& expr() { return *expr_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::unique_ptr<Expr> expr_;
    SourceLocation location_;
};

// Statement: Block ({ stmt* })
class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements, SourceLocation loc)
        : statements_(std::move(statements)), location_(loc) {}

    const std::vector<std::unique_ptr<Stmt>>& statements() const { return statements_; }
    std::vector<std::unique_ptr<Stmt>>& statements() { return statements_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::vector<std::unique_ptr<Stmt>> statements_;
    SourceLocation location_;
};

// Statement: If-Else (if (cond) { ... } else { ... })
class IfStmt : public Stmt {
public:
    IfStmt(std::unique_ptr<Expr> condition,
           std::unique_ptr<BlockStmt> thenBranch,
           std::unique_ptr<BlockStmt> elseBranch,
           SourceLocation loc)
        : condition_(std::move(condition)),
          thenBranch_(std::move(thenBranch)),
          elseBranch_(std::move(elseBranch)),
          location_(loc) {}

    const Expr& condition() const { return *condition_; }
    Expr& condition() { return *condition_; }
    const BlockStmt& thenBranch() const { return *thenBranch_; }
    BlockStmt& thenBranch() { return *thenBranch_; }
    bool hasElse() const { return elseBranch_ != nullptr; }
    const BlockStmt* elseBranch() const { return elseBranch_.get(); }
    BlockStmt* elseBranch() { return elseBranch_.get(); }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::unique_ptr<Expr> condition_;
    std::unique_ptr<BlockStmt> thenBranch_;
    std::unique_ptr<BlockStmt> elseBranch_;
    SourceLocation location_;
};

// Statement: While Loop (while (cond) { ... })
class WhileStmt : public Stmt {
public:
    WhileStmt(std::unique_ptr<Expr> condition,
              std::unique_ptr<BlockStmt> body,
              SourceLocation loc)
        : condition_(std::move(condition)), body_(std::move(body)), location_(loc) {}

    const Expr& condition() const { return *condition_; }
    Expr& condition() { return *condition_; }
    const BlockStmt& body() const { return *body_; }
    BlockStmt& body() { return *body_; }
    SourceLocation location() const override { return location_; }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::unique_ptr<Expr> condition_;
    std::unique_ptr<BlockStmt> body_;
    SourceLocation location_;
};

// Program: Root AST container
class Program : public ASTNode {
public:
    explicit Program(std::vector<std::unique_ptr<Stmt>> statements)
        : statements_(std::move(statements)) {}

    const std::vector<std::unique_ptr<Stmt>>& statements() const { return statements_; }
    std::vector<std::unique_ptr<Stmt>>& statements() { return statements_; }
    SourceLocation location() const override {
        return statements_.empty() ? SourceLocation(1, 1) : statements_.front()->location();
    }
    void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
    std::string toString(int indent = 0) const override;

private:
    std::vector<std::unique_ptr<Stmt>> statements_;
};

} // namespace miniforge

#endif // MINIFORGE_AST_AST_HPP
