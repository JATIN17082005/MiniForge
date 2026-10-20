#ifndef MINIFORGE_IR_IRBUILDER_HPP
#define MINIFORGE_IR_IRBUILDER_HPP

#include "ast/AST.hpp"
#include "ast/ASTVisitor.hpp"
#include "ir/IR.hpp"
#include <unordered_map>
#include <vector>
#include <memory>

namespace miniforge {

class IRBuilder : public ASTVisitor {
public:
    IRBuilder();

    IRProgram build(Program& program);

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

private:
    struct ScopeEnv {
        std::unordered_map<std::string, std::string> varMap;
        ScopeEnv* parent{nullptr};
        int scopeId{0};
    };

    void enterScope();
    void exitScope();
    std::string declareVar(const std::string& name);
    std::string resolveVar(const std::string& name);

    IROperand newTemp();
    std::string newLabel(const std::string& prefix = "L");

    IROpcode tokenToBinaryOpcode(TokenType type);

    IRProgram irProgram_;
    int nextTempId_{0};
    int nextLabelId_{0};
    int nextScopeId_{0};

    std::vector<std::unique_ptr<ScopeEnv>> scopes_;
    ScopeEnv* currentScope_{nullptr};

    // Holds the operand result of the most recently visited expression
    IROperand lastExprResult_;
};

} // namespace miniforge

#endif // MINIFORGE_IR_IRBUILDER_HPP
