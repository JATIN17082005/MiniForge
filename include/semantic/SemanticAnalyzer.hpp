#ifndef MINIFORGE_SEMANTIC_SEMANTICANALYZER_HPP
#define MINIFORGE_SEMANTIC_SEMANTICANALYZER_HPP

#include "ast/ASTVisitor.hpp"
#include "ast/AST.hpp"
#include "semantic/SymbolTable.hpp"
#include "common/Diagnostic.hpp"

namespace miniforge {

class SemanticAnalyzer : public ASTVisitor {
public:
    explicit SemanticAnalyzer(DiagnosticEngine& diags);

    bool analyze(Program& program);

    // ASTVisitor implementations
    void visit(IntegerLiteralExpr& node) override;
    void visit(VariableExpr& node) override;
    void visit(UnaryExpr& node) override;
    void visit(BinaryExpr& node) override;

    void visit(VarDeclStmt& node) override;
    void visit(AssignStmt& node) override;
    void visit(PrintStmt& node) override;
    void visit(BlockStmt& node) override;
    void visit(IfStmt& node) override;
    void visit(WhileStmt& node) override;

    void visit(Program& node) override;

    const SymbolTable& symbolTable() const { return symbolTable_; }

private:
    DiagnosticEngine& diags_;
    SymbolTable symbolTable_;
};

} // namespace miniforge

#endif // MINIFORGE_SEMANTIC_SEMANTICANALYZER_HPP
