#include "optimizer/ConstantFolder.hpp"

namespace miniforge {

bool ConstantFolder::run(IRProgram& program) {
    bool modified = false;

    for (auto& instr : program.instructions()) {
        if (instr.opcode == IROpcode::Neg && instr.src1.isConstant()) {
            int64_t val = -instr.src1.constantValue();
            instr = IRInstruction::makeAssign(instr.dest, IROperand::makeConstant(val));
            modified = true;
            continue;
        }

        if (instr.src1.isConstant() && instr.src2.isConstant()) {
            int64_t c1 = instr.src1.constantValue();
            int64_t c2 = instr.src2.constantValue();
            int64_t result = 0;
            bool canFold = true;

            switch (instr.opcode) {
                case IROpcode::Add:
                    result = c1 + c2;
                    break;
                case IROpcode::Sub:
                    result = c1 - c2;
                    break;
                case IROpcode::Mul:
                    result = c1 * c2;
                    break;
                case IROpcode::Div:
                    if (c2 == 0) {
                        canFold = false; // Safe: preserve division by zero for runtime error
                    } else {
                        result = c1 / c2;
                    }
                    break;
                case IROpcode::Mod:
                    if (c2 == 0) {
                        canFold = false; // Safe: preserve modulo by zero for runtime error
                    } else {
                        result = c1 % c2;
                    }
                    break;
                case IROpcode::Equal:
                    result = (c1 == c2) ? 1 : 0;
                    break;
                case IROpcode::NotEqual:
                    result = (c1 != c2) ? 1 : 0;
                    break;
                case IROpcode::Less:
                    result = (c1 < c2) ? 1 : 0;
                    break;
                case IROpcode::LessEqual:
                    result = (c1 <= c2) ? 1 : 0;
                    break;
                case IROpcode::Greater:
                    result = (c1 > c2) ? 1 : 0;
                    break;
                case IROpcode::GreaterEqual:
                    result = (c1 >= c2) ? 1 : 0;
                    break;
                default:
                    canFold = false;
                    break;
            }

            if (canFold) {
                instr = IRInstruction::makeAssign(instr.dest, IROperand::makeConstant(result));
                modified = true;
            }
        }
    }

    return modified;
}

} // namespace miniforge
