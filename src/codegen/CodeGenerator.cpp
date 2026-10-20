#include "codegen/CodeGenerator.hpp"
#include <stdexcept>

namespace miniforge {

CodeGenerator::CodeGenerator(DiagnosticEngine* diags)
    : diags_(diags) {}

void CodeGenerator::emitPush(const IROperand& op, BytecodeProgram& bytecode) {
    if (op.isConstant()) {
        bytecode.addInstruction(VMInstruction(Opcode::PushConst, op.constantValue()));
    } else if (op.isVariable() || op.isTemporary()) {
        bytecode.addInstruction(VMInstruction(Opcode::LoadVar, 0, op.toString()));
    }
}

void CodeGenerator::emitStore(const IROperand& op, BytecodeProgram& bytecode) {
    if (op.isVariable() || op.isTemporary()) {
        bytecode.addInstruction(VMInstruction(Opcode::StoreVar, 0, op.toString()));
    }
}

BytecodeProgram CodeGenerator::generate(const IRProgram& irProgram) {
    BytecodeProgram bytecode;
    std::unordered_map<std::string, size_t> labelAddresses;

    // Pass 1: Translate IR instructions to VM instructions with symbolic labels
    for (const auto& instr : irProgram.instructions()) {
        if (instr.opcode == IROpcode::Label) {
            labelAddresses[instr.labelName] = bytecode.size();
            continue;
        }

        switch (instr.opcode) {
            case IROpcode::Assign:
                emitPush(instr.src1, bytecode);
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Neg:
                emitPush(instr.src1, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Neg));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Add:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Add));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Sub:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Sub));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Mul:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Mul));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Div:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Div));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Mod:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Mod));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Equal:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Equal));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::NotEqual:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::NotEqual));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Less:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Less));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::LessEqual:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::LessEqual));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Greater:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Greater));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::GreaterEqual:
                emitPush(instr.src1, bytecode);
                emitPush(instr.src2, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::GreaterEqual));
                emitStore(instr.dest, bytecode);
                break;
            case IROpcode::Jump:
                bytecode.addInstruction(VMInstruction(Opcode::Jump, 0, instr.labelName));
                break;
            case IROpcode::JumpIfZero:
                emitPush(instr.src1, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::JumpIfZero, 0, instr.labelName));
                break;
            case IROpcode::JumpIfNotZero:
                emitPush(instr.src1, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::JumpIfNotZero, 0, instr.labelName));
                break;
            case IROpcode::Print:
                emitPush(instr.src1, bytecode);
                bytecode.addInstruction(VMInstruction(Opcode::Print));
                break;
            case IROpcode::Label:
                break;
        }
    }

    // Always append HALT at the end of the program
    size_t haltAddress = bytecode.size();
    bytecode.addInstruction(VMInstruction(Opcode::Halt));

    // Pass 2: Label resolution
    for (size_t i = 0; i < bytecode.size(); ++i) {
        auto& instr = bytecode[i];
        if (instr.opcode == Opcode::Jump ||
            instr.opcode == Opcode::JumpIfZero ||
            instr.opcode == Opcode::JumpIfNotZero) {
            auto it = labelAddresses.find(instr.strOperand);
            if (it != labelAddresses.end()) {
                instr.intOperand = static_cast<int64_t>(it->second);
            } else {
                // If label was at end of IR with no subsequent instruction, point to HALT
                instr.intOperand = static_cast<int64_t>(haltAddress);
            }
        }
    }

    // Pass 3: Validation
    for (size_t i = 0; i < bytecode.size(); ++i) {
        const auto& instr = bytecode[i];
        if (instr.opcode == Opcode::Jump ||
            instr.opcode == Opcode::JumpIfZero ||
            instr.opcode == Opcode::JumpIfNotZero) {
            if (instr.intOperand < 0 || static_cast<size_t>(instr.intOperand) >= bytecode.size()) {
                hasInternalError_ = true;
                if (diags_) {
                    diags_->error(SourceLocation(1, 1),
                        "Codegen error: jump target address " + std::to_string(instr.intOperand) +
                        " out of range [0, " + std::to_string(bytecode.size()) + ")");
                }
            }
        }
    }

    return bytecode;
}

} // namespace miniforge
