#include "vm/VM.hpp"
#include "codegen/Disassembler.hpp"
#include <iomanip>
#include <iostream>

namespace miniforge {

VirtualMachine::VirtualMachine(std::ostream& output)
    : output_(output) {}

void VirtualMachine::reset() {
    stack_.clear();
    memory_.clear();
    pc_ = 0;
}

void VirtualMachine::push(int64_t val) {
    stack_.push_back(val);
}

int64_t VirtualMachine::pop() {
    if (stack_.empty()) {
        throw VMRuntimeError("VM runtime error: Operand stack underflow");
    }
    int64_t val = stack_.back();
    stack_.pop_back();
    return val;
}

int64_t VirtualMachine::peek() const {
    if (stack_.empty()) {
        throw VMRuntimeError("VM runtime error: Operand stack underflow on peek");
    }
    return stack_.back();
}

int64_t VirtualMachine::getVariable(const std::string& name) const {
    auto it = memory_.find(name);
    if (it == memory_.end()) {
        throw VMRuntimeError("VM runtime error: Access to uninitialized variable '" + name + "'");
    }
    return it->second;
}

void VirtualMachine::printTrace(size_t currentPc, const VMInstruction& instr) const {
    std::cerr << "TRACE [" << std::setfill('0') << std::right << std::setw(4) << currentPc << "] "
              << std::setfill(' ') << std::left << std::setw(32)
              << Disassembler::disassembleInstruction(currentPc, instr)
              << " | Stack: [";
    for (size_t i = 0; i < stack_.size(); ++i) {
        if (i > 0) std::cerr << ", ";
        std::cerr << stack_[i];
    }
    std::cerr << "]\n";
}

int VirtualMachine::run(const BytecodeProgram& program, bool trace) {
    reset();

    try {
        while (pc_ < program.size()) {
            size_t currentPc = pc_;
            const auto& instr = program[pc_];

            if (trace) {
                printTrace(currentPc, instr);
            }

            pc_++; // Advance PC to next instruction by default

            switch (instr.opcode) {
                case Opcode::PushConst:
                    push(instr.intOperand);
                    break;
                case Opcode::LoadVar:
                    push(getVariable(instr.strOperand));
                    break;
                case Opcode::StoreVar:
                    memory_[instr.strOperand] = pop();
                    break;
                case Opcode::Add: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a + b);
                    break;
                }
                case Opcode::Sub: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a - b);
                    break;
                }
                case Opcode::Mul: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a * b);
                    break;
                }
                case Opcode::Div: {
                    int64_t b = pop();
                    int64_t a = pop();
                    if (b == 0) {
                        throw VMRuntimeError("VM runtime error: Division by zero");
                    }
                    push(a / b);
                    break;
                }
                case Opcode::Mod: {
                    int64_t b = pop();
                    int64_t a = pop();
                    if (b == 0) {
                        throw VMRuntimeError("VM runtime error: Modulo by zero");
                    }
                    push(a % b);
                    break;
                }
                case Opcode::Neg: {
                    int64_t a = pop();
                    push(-a);
                    break;
                }
                case Opcode::Equal: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a == b ? 1 : 0);
                    break;
                }
                case Opcode::NotEqual: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a != b ? 1 : 0);
                    break;
                }
                case Opcode::Less: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a < b ? 1 : 0);
                    break;
                }
                case Opcode::LessEqual: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a <= b ? 1 : 0);
                    break;
                }
                case Opcode::Greater: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a > b ? 1 : 0);
                    break;
                }
                case Opcode::GreaterEqual: {
                    int64_t b = pop();
                    int64_t a = pop();
                    push(a >= b ? 1 : 0);
                    break;
                }
                case Opcode::Jump:
                    if (instr.intOperand < 0 || static_cast<size_t>(instr.intOperand) >= program.size()) {
                        throw VMRuntimeError("VM runtime error: Invalid jump target address " + std::to_string(instr.intOperand));
                    }
                    pc_ = static_cast<size_t>(instr.intOperand);
                    break;
                case Opcode::JumpIfZero: {
                    int64_t cond = pop();
                    if (cond == 0) {
                        if (instr.intOperand < 0 || static_cast<size_t>(instr.intOperand) >= program.size()) {
                            throw VMRuntimeError("VM runtime error: Invalid jump target address " + std::to_string(instr.intOperand));
                        }
                        pc_ = static_cast<size_t>(instr.intOperand);
                    }
                    break;
                }
                case Opcode::JumpIfNotZero: {
                    int64_t cond = pop();
                    if (cond != 0) {
                        if (instr.intOperand < 0 || static_cast<size_t>(instr.intOperand) >= program.size()) {
                            throw VMRuntimeError("VM runtime error: Invalid jump target address " + std::to_string(instr.intOperand));
                        }
                        pc_ = static_cast<size_t>(instr.intOperand);
                    }
                    break;
                }
                case Opcode::Print: {
                    int64_t val = pop();
                    output_ << val << "\n";
                    break;
                }
                case Opcode::Halt:
                    return 0; // Successful execution
            }
        }
    } catch (const VMRuntimeError& err) {
        std::cerr << err.what() << "\n";
        return 1;
    }

    return 0;
}

} // namespace miniforge
