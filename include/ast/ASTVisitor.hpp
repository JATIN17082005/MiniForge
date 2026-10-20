#ifndef MINIFORGE_AST_ASTVISITOR_HPP
#define MINIFORGE_AST_ASTVISITOR_HPP

namespace miniforge {

// Forward declarations of AST nodes
class IntegerLiteralExpr;
class VariableExpr;
class UnaryExpr;
class BinaryExpr;

class VarDeclStmt;
class AssignStmt;
class PrintStmt;
class BlockStmt;
class IfStmt;
class WhileStmt;
class Program;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // Expressions
    virtual void visit(IntegerLiteralExpr& node) = 0;
    virtual void visit(VariableExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;

    // Statements
    virtual void visit(VarDeclStmt& node) = 0;
    virtual void visit(AssignStmt& node) = 0;
    virtual void visit(PrintStmt& node) = 0;
    virtual void visit(BlockStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;

    // Program
    virtual void visit(Program& node) = 0;
};

} // namespace miniforge

#endif // MINIFORGE_AST_ASTVISITOR_HPP
