#include "semantic/SemanticAnalyzer.hpp"

namespace miniforge {

SemanticAnalyzer::SemanticAnalyzer(DiagnosticEngine& diags)
    : diags_(diags) {}

bool SemanticAnalyzer::analyze(Program& program) {
    program.accept(*this);
    return !diags_.hasErrors();
}

void SemanticAnalyzer::visit(IntegerLiteralExpr& /*node*/) {
    // Integer literals are always semantically valid
}

void SemanticAnalyzer::visit(VariableExpr& node) {
    Symbol* sym = symbolTable_.resolve(node.name());
    if (sym == nullptr) {
        diags_.error(node.location(), "Use of undeclared variable '" + node.name() + "'");
    }
}

void SemanticAnalyzer::visit(UnaryExpr& node) {
    node.operand().accept(*this);
}

void SemanticAnalyzer::visit(BinaryExpr& node) {
    node.left().accept(*this);
    node.right().accept(*this);
}

void SemanticAnalyzer::visit(VarDeclStmt& node) {
    // First evaluate initializer
    node.init().accept(*this);

    // Then declare in current scope
    if (!symbolTable_.declare(node.name(), node.location())) {
        diags_.error(node.location(), "Redeclaration of variable '" + node.name() + "' in the same scope");
    }
}

void SemanticAnalyzer::visit(AssignStmt& node) {
    Symbol* sym = symbolTable_.resolve(node.name());
    if (sym == nullptr) {
        diags_.error(node.location(), "Assignment to undeclared variable '" + node.name() + "'");
    }

    node.value().accept(*this);
}

void SemanticAnalyzer::visit(PrintStmt& node) {
    node.expr().accept(*this);
}

void SemanticAnalyzer::visit(BlockStmt& node) {
    symbolTable_.enterScope();
    for (auto& stmt : node.statements()) {
        stmt->accept(*this);
    }
    symbolTable_.exitScope();
}

void SemanticAnalyzer::visit(IfStmt& node) {
    node.condition().accept(*this);
    node.thenBranch().accept(*this);
    if (node.hasElse()) {
        node.elseBranch()->accept(*this);
    }
}

void SemanticAnalyzer::visit(WhileStmt& node) {
    node.condition().accept(*this);
    node.body().accept(*this);
}

void SemanticAnalyzer::visit(Program& node) {
    for (auto& stmt : node.statements()) {
        stmt->accept(*this);
    }
}

} // namespace miniforge
