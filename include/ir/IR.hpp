#ifndef MINIFORGE_IR_IR_HPP
#define MINIFORGE_IR_IR_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <utility>

namespace miniforge {

enum class IROpcode {
    Assign,         // dest = src1
    Add,            // dest = src1 + src2
    Sub,            // dest = src1 - src2
    Mul,            // dest = src1 * src2
    Div,            // dest = src1 / src2
    Mod,            // dest = src1 % src2
    Neg,            // dest = -src1
    Equal,          // dest = (src1 == src2)
    NotEqual,       // dest = (src1 != src2)
    Less,           // dest = (src1 < src2)
    LessEqual,      // dest = (src1 <= src2)
    Greater,        // dest = (src1 > src2)
    GreaterEqual,   // dest = (src1 >= src2)
    Label,          // label:
    Jump,           // jump label
    JumpIfZero,     // jump_if_zero cond, label
    JumpIfNotZero,  // jump_if_not_zero cond, label
    Print           // print src1
};

std::string_view irOpcodeToString(IROpcode op);

inline std::ostream& operator<<(std::ostream& os, IROpcode op) {
    os << irOpcodeToString(op);
    return os;
}

class IROperand {
public:
    enum class Kind {
        None,
        Constant,
        Variable,
        Temporary
    };

    IROperand() : kind_(Kind::None), intVal_(0) {}

    static IROperand makeNone() {
        return IROperand();
    }

    static IROperand makeConstant(int64_t val) {
        IROperand op;
        op.kind_ = Kind::Constant;
        op.intVal_ = val;
        return op;
    }

    static IROperand makeVariable(std::string name) {
        IROperand op;
        op.kind_ = Kind::Variable;
        op.name_ = std::move(name);
        return op;
    }

    static IROperand makeTemporary(int id) {
        IROperand op;
        op.kind_ = Kind::Temporary;
        op.intVal_ = id;
        return op;
    }

    Kind kind() const { return kind_; }
    bool isNone() const { return kind_ == Kind::None; }
    bool isConstant() const { return kind_ == Kind::Constant; }
    bool isVariable() const { return kind_ == Kind::Variable; }
    bool isTemporary() const { return kind_ == Kind::Temporary; }

    int64_t constantValue() const { return intVal_; }
    int tempId() const { return static_cast<int>(intVal_); }
    const std::string& variableName() const { return name_; }

    std::string toString() const {
        switch (kind_) {
            case Kind::None:
                return "<none>";
            case Kind::Constant:
                return std::to_string(intVal_);
            case Kind::Variable:
                return name_;
            case Kind::Temporary:
                return "t" + std::to_string(intVal_);
        }
        return "?";
    }

    bool operator==(const IROperand& other) const {
        if (kind_ != other.kind_) return false;
        if (kind_ == Kind::Constant || kind_ == Kind::Temporary) {
            return intVal_ == other.intVal_;
        }
        if (kind_ == Kind::Variable) {
            return name_ == other.name_;
        }
        return true;
    }

    bool operator!=(const IROperand& other) const {
        return !(*this == other);
    }

private:
    Kind kind_{Kind::None};
    int64_t intVal_{0};
    std::string name_;
};

struct IRInstruction {
    IROpcode opcode{IROpcode::Assign};
    IROperand dest;
    IROperand src1;
    IROperand src2;
    std::string labelName;

    IRInstruction() = default;

    static IRInstruction makeAssign(IROperand dest, IROperand src) {
        IRInstruction instr;
        instr.opcode = IROpcode::Assign;
        instr.dest = std::move(dest);
        instr.src1 = std::move(src);
        return instr;
    }

    static IRInstruction makeBinary(IROpcode op, IROperand dest, IROperand src1, IROperand src2) {
        IRInstruction instr;
        instr.opcode = op;
        instr.dest = std::move(dest);
        instr.src1 = std::move(src1);
        instr.src2 = std::move(src2);
        return instr;
    }

    static IRInstruction makeUnary(IROpcode op, IROperand dest, IROperand src) {
        IRInstruction instr;
        instr.opcode = op;
        instr.dest = std::move(dest);
        instr.src1 = std::move(src);
        return instr;
    }

    static IRInstruction makeLabel(std::string name) {
        IRInstruction instr;
        instr.opcode = IROpcode::Label;
        instr.labelName = std::move(name);
        return instr;
    }

    static IRInstruction makeJump(std::string target) {
        IRInstruction instr;
        instr.opcode = IROpcode::Jump;
        instr.labelName = std::move(target);
        return instr;
    }

    static IRInstruction makeJumpIfZero(IROperand cond, std::string target) {
        IRInstruction instr;
        instr.opcode = IROpcode::JumpIfZero;
        instr.src1 = std::move(cond);
        instr.labelName = std::move(target);
        return instr;
    }

    static IRInstruction makeJumpIfNotZero(IROperand cond, std::string target) {
        IRInstruction instr;
        instr.opcode = IROpcode::JumpIfNotZero;
        instr.src1 = std::move(cond);
        instr.labelName = std::move(target);
        return instr;
    }

    static IRInstruction makePrint(IROperand src) {
        IRInstruction instr;
        instr.opcode = IROpcode::Print;
        instr.src1 = std::move(src);
        return instr;
    }

    std::string toString() const;
};

class IRProgram {
public:
    IRProgram() = default;

    void addInstruction(IRInstruction instr) {
        instructions_.push_back(std::move(instr));
    }

    const std::vector<IRInstruction>& instructions() const { return instructions_; }
    std::vector<IRInstruction>& instructions() { return instructions_; }

    void clear() { instructions_.clear(); }

    std::string toString() const {
        std::ostringstream oss;
        for (const auto& instr : instructions_) {
            if (instr.opcode == IROpcode::Label) {
                oss << instr.toString() << "\n";
            } else {
                oss << "  " << instr.toString() << "\n";
            }
        }
        return oss.str();
    }

private:
    std::vector<IRInstruction> instructions_;
};

} // namespace miniforge

#endif // MINIFORGE_IR_IR_HPP
