#include "ast/AST.hpp"
#include <sstream>

namespace miniforge {

static std::string makeIndent(int indent) {
    return std::string(indent * 2, ' ');
}

std::string IntegerLiteralExpr::toString(int indent) const {
    return makeIndent(indent) + "IntegerLiteral: " + std::to_string(value_) +
           " [" + location_.toString() + "]\n";
}

std::string VariableExpr::toString(int indent) const {
    return makeIndent(indent) + "VariableExpr: " + name_ +
           " [" + location_.toString() + "]\n";
}

std::string UnaryExpr::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "UnaryExpr: " << tokenTypeToString(op_)
        << " [" << location_.toString() << "]\n";
    oss << operand_->toString(indent + 1);
    return oss.str();
}

std::string BinaryExpr::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "BinaryExpr: " << tokenTypeToString(op_)
        << " [" << location_.toString() << "]\n";
    oss << left_->toString(indent + 1);
    oss << right_->toString(indent + 1);
    return oss.str();
}

std::string VarDeclStmt::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "VarDeclStmt: " << name_
        << " [" << location_.toString() << "]\n";
    oss << init_->toString(indent + 1);
    return oss.str();
}

std::string AssignStmt::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "AssignStmt: " << name_
        << " [" << location_.toString() << "]\n";
    oss << value_->toString(indent + 1);
    return oss.str();
}

std::string PrintStmt::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "PrintStmt [" << location_.toString() << "]\n";
    oss << expr_->toString(indent + 1);
    return oss.str();
}

std::string BlockStmt::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "BlockStmt [" << location_.toString() << "] ("
        << statements_.size() << " statements)\n";
    for (const auto& s : statements_) {
        oss << s->toString(indent + 1);
    }
    return oss.str();
}

std::string IfStmt::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "IfStmt [" << location_.toString() << "]\n";
    oss << makeIndent(indent + 1) << "Condition:\n";
    oss << condition_->toString(indent + 2);
    oss << makeIndent(indent + 1) << "Then:\n";
    oss << thenBranch_->toString(indent + 2);
    if (elseBranch_) {
        oss << makeIndent(indent + 1) << "Else:\n";
        oss << elseBranch_->toString(indent + 2);
    }
    return oss.str();
}

std::string WhileStmt::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "WhileStmt [" << location_.toString() << "]\n";
    oss << makeIndent(indent + 1) << "Condition:\n";
    oss << condition_->toString(indent + 2);
    oss << makeIndent(indent + 1) << "Body:\n";
    oss << body_->toString(indent + 2);
    return oss.str();
}

std::string Program::toString(int indent) const {
    std::ostringstream oss;
    oss << makeIndent(indent) << "Program (" << statements_.size() << " top-level statements)\n";
    for (const auto& s : statements_) {
        oss << s->toString(indent + 1);
    }
    return oss.str();
}

} // namespace miniforge
