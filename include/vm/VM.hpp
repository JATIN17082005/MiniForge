#ifndef MINIFORGE_VM_VM_HPP
#define MINIFORGE_VM_VM_HPP

#include "codegen/Bytecode.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>

namespace miniforge {

class VMRuntimeError : public std::runtime_error {
public:
    explicit VMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class VirtualMachine {
public:
    explicit VirtualMachine(std::ostream& output = std::cout);

    // Executes bytecode program. Returns 0 on success, non-zero on error.
    int run(const BytecodeProgram& program, bool trace = false);

    // Getters for state inspection (useful in testing)
    const std::vector<int64_t>& stack() const { return stack_; }
    const std::unordered_map<std::string, int64_t>& memory() const { return memory_; }
    int64_t getVariable(const std::string& name) const;
    size_t pc() const { return pc_; }

    void reset();

private:
    void push(int64_t val);
    int64_t pop();
    int64_t peek() const;

    void printTrace(size_t currentPc, const VMInstruction& instr) const;

    std::vector<int64_t> stack_;
    std::unordered_map<std::string, int64_t> memory_;
    size_t pc_{0};
    std::ostream& output_;
};

} // namespace miniforge

#endif // MINIFORGE_VM_VM_HPP
