#include "optimizer/ConstantPropagator.hpp"
#include <unordered_map>

namespace miniforge {

bool ConstantPropagator::run(IRProgram& program) {
    bool modified = false;
    std::unordered_map<std::string, int64_t> knownConstants;

    for (auto& instr : program.instructions()) {
        // Label marks the start of a block that might be reached via a jump.
        // Clear known constants to avoid unsafe propagation across control-flow boundaries.
        if (instr.opcode == IROpcode::Label) {
            knownConstants.clear();
            continue;
        }

        // Try propagating into src1
        if (!instr.src1.isNone() && !instr.src1.isConstant()) {
            auto it = knownConstants.find(instr.src1.toString());
            if (it != knownConstants.end()) {
                instr.src1 = IROperand::makeConstant(it->second);
                modified = true;
            }
        }

        // Try propagating into src2
        if (!instr.src2.isNone() && !instr.src2.isConstant()) {
            auto it = knownConstants.find(instr.src2.toString());
            if (it != knownConstants.end()) {
                instr.src2 = IROperand::makeConstant(it->second);
                modified = true;
            }
        }

        // Update definitions
        if (!instr.dest.isNone()) {
            if (instr.opcode == IROpcode::Assign && instr.src1.isConstant()) {
                knownConstants[instr.dest.toString()] = instr.src1.constantValue();
            } else {
                // Variable or temporary redefined with a non-constant value
                knownConstants.erase(instr.dest.toString());
            }
        }

        // Branching instructions terminate the current basic block.
        if (instr.opcode == IROpcode::Jump ||
            instr.opcode == IROpcode::JumpIfZero ||
            instr.opcode == IROpcode::JumpIfNotZero) {
            knownConstants.clear();
        }
    }

    return modified;
}

} // namespace miniforge
