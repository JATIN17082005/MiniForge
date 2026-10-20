#include "ir/IRBuilder.hpp"
#include <stdexcept>

namespace miniforge {

IRBuilder::IRBuilder() {
    enterScope(); // Root scope
}

IRProgram IRBuilder::build(Program& program) {
    irProgram_.clear();
    nextTempId_ = 0;
    nextLabelId_ = 0;
    program.accept(*this);
    return std::move(irProgram_);
}

void IRBuilder::enterScope() {
    auto env = std::make_unique<ScopeEnv>();
    env->parent = currentScope_;
    env->scopeId = nextScopeId_++;
    currentScope_ = env.get();
    scopes_.push_back(std::move(env));
}

void IRBuilder::exitScope() {
    if (currentScope_ != nullptr) {
        currentScope_ = currentScope_->parent;
    }
}

std::string IRBuilder::declareVar(const std::string& name) {
    if (currentScope_ == nullptr) return name;

    // Check if name is already declared in an ancestor scope (shadowing)
    bool isShadowed = false;
    for (ScopeEnv* s = currentScope_->parent; s != nullptr; s = s->parent) {
        if (s->varMap.find(name) != s->varMap.end()) {
            isShadowed = true;
            break;
        }
    }

    std::string actualName = name;
    if (isShadowed) {
        actualName = name + "_" + std::to_string(currentScope_->scopeId);
    }
    currentScope_->varMap[name] = actualName;
    return actualName;
}

std::string IRBuilder::resolveVar(const std::string& name) {
    for (ScopeEnv* s = currentScope_; s != nullptr; s = s->parent) {
        auto it = s->varMap.find(name);
        if (it != s->varMap.end()) {
            return it->second;
        }
    }
    return name;
}

IROperand IRBuilder::newTemp() {
    return IROperand::makeTemporary(nextTempId_++);
}

std::string IRBuilder::newLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(nextLabelId_++);
}

IROpcode IRBuilder::tokenToBinaryOpcode(TokenType type) {
    switch (type) {
        case TokenType::Plus:         return IROpcode::Add;
        case TokenType::Minus:        return IROpcode::Sub;
        case TokenType::Star:         return IROpcode::Mul;
        case TokenType::Slash:        return IROpcode::Div;
        case TokenType::Percent:      return IROpcode::Mod;
        case TokenType::EqualEqual:   return IROpcode::Equal;
        case TokenType::BangEqual:    return IROpcode::NotEqual;
        case TokenType::Less:         return IROpcode::Less;
        case TokenType::LessEqual:    return IROpcode::LessEqual;
        case TokenType::Greater:      return IROpcode::Greater;
        case TokenType::GreaterEqual: return IROpcode::GreaterEqual;
        default:
            throw std::runtime_error("Unsupported binary operator token in IR builder");
    }
}

void IRBuilder::visit(IntegerLiteralExpr& node) {
    lastExprResult_ = IROperand::makeConstant(node.value());
}

void IRBuilder::visit(VariableExpr& node) {
    lastExprResult_ = IROperand::makeVariable(resolveVar(node.name()));
}

void IRBuilder::visit(UnaryExpr& node) {
    node.operand().accept(*this);
    IROperand operand = lastExprResult_;
    IROperand temp = newTemp();
    irProgram_.addInstruction(IRInstruction::makeUnary(IROpcode::Neg, temp, operand));
    lastExprResult_ = temp;
}

void IRBuilder::visit(BinaryExpr& node) {
    node.left().accept(*this);
    IROperand left = lastExprResult_;
    node.right().accept(*this);
    IROperand right = lastExprResult_;

    IROperand temp = newTemp();
    IROpcode op = tokenToBinaryOpcode(node.op());
    irProgram_.addInstruction(IRInstruction::makeBinary(op, temp, left, right));
    lastExprResult_ = temp;
}

void IRBuilder::visit(VarDeclStmt& node) {
    node.init().accept(*this);
    IROperand initVal = lastExprResult_;
    std::string varName = declareVar(node.name());
    irProgram_.addInstruction(IRInstruction::makeAssign(IROperand::makeVariable(varName), initVal));
}

void IRBuilder::visit(AssignStmt& node) {
    node.value().accept(*this);
    IROperand val = lastExprResult_;
    std::string varName = resolveVar(node.name());
    irProgram_.addInstruction(IRInstruction::makeAssign(IROperand::makeVariable(varName), val));
}

void IRBuilder::visit(PrintStmt& node) {
    node.expr().accept(*this);
    irProgram_.addInstruction(IRInstruction::makePrint(lastExprResult_));
}

void IRBuilder::visit(BlockStmt& node) {
    enterScope();
    for (auto& stmt : node.statements()) {
        stmt->accept(*this);
    }
    exitScope();
}

void IRBuilder::visit(IfStmt& node) {
    node.condition().accept(*this);
    IROperand cond = lastExprResult_;

    if (node.hasElse()) {
        std::string elseLabel = newLabel("L_else");
        std::string endLabel = newLabel("L_end");

        irProgram_.addInstruction(IRInstruction::makeJumpIfZero(cond, elseLabel));
        node.thenBranch().accept(*this);
        irProgram_.addInstruction(IRInstruction::makeJump(endLabel));

        irProgram_.addInstruction(IRInstruction::makeLabel(elseLabel));
        node.elseBranch()->accept(*this);
        irProgram_.addInstruction(IRInstruction::makeLabel(endLabel));
    } else {
        std::string endLabel = newLabel("L_end");
        irProgram_.addInstruction(IRInstruction::makeJumpIfZero(cond, endLabel));
        node.thenBranch().accept(*this);
        irProgram_.addInstruction(IRInstruction::makeLabel(endLabel));
    }
}

void IRBuilder::visit(WhileStmt& node) {
    std::string loopStart = newLabel("L_loop_start");
    std::string loopEnd = newLabel("L_loop_end");

    irProgram_.addInstruction(IRInstruction::makeLabel(loopStart));
    node.condition().accept(*this);
    IROperand cond = lastExprResult_;

    irProgram_.addInstruction(IRInstruction::makeJumpIfZero(cond, loopEnd));
    node.body().accept(*this);
    irProgram_.addInstruction(IRInstruction::makeJump(loopStart));
    irProgram_.addInstruction(IRInstruction::makeLabel(loopEnd));
}

void IRBuilder::visit(Program& node) {
    for (auto& stmt : node.statements()) {
        stmt->accept(*this);
    }
}

} // namespace miniforge
