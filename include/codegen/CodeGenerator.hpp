#ifndef MINIFORGE_CODEGEN_CODEGENERATOR_HPP
#define MINIFORGE_CODEGEN_CODEGENERATOR_HPP

#include "ir/IR.hpp"
#include "codegen/Bytecode.hpp"
#include "common/Diagnostic.hpp"
#include <unordered_map>

namespace miniforge {

class CodeGenerator {
public:
    explicit CodeGenerator(DiagnosticEngine* diags = nullptr);

    BytecodeProgram generate(const IRProgram& irProgram);

    bool hasErrors() const {
        return diags_ ? diags_->hasErrors() : hasInternalError_;
    }

private:
    void emitPush(const IROperand& op, BytecodeProgram& bytecode);
    void emitStore(const IROperand& op, BytecodeProgram& bytecode);

    DiagnosticEngine* diags_{nullptr};
    bool hasInternalError_{false};
};

} // namespace miniforge

#endif // MINIFORGE_CODEGEN_CODEGENERATOR_HPP
