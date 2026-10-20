#include "optimizer/DeadCodeEliminator.hpp"
#include <unordered_set>
#include <algorithm>

namespace miniforge {

bool DeadCodeEliminator::run(IRProgram& program) {
    // Collect all temporaries that are read as an operand anywhere in the program
    std::unordered_set<int> usedTemps;
    for (const auto& instr : program.instructions()) {
        if (instr.src1.isTemporary()) {
            usedTemps.insert(instr.src1.tempId());
        }
        if (instr.src2.isTemporary()) {
            usedTemps.insert(instr.src2.tempId());
        }
    }

    auto& instrs = program.instructions();
    auto it = std::remove_if(instrs.begin(), instrs.end(),
        [&usedTemps](const IRInstruction& instr) {
            if (instr.dest.isTemporary()) {
                return usedTemps.find(instr.dest.tempId()) == usedTemps.end();
            }
            return false;
        });

    if (it != instrs.end()) {
        instrs.erase(it, instrs.end());
        return true;
    }

    return false;
}

} // namespace miniforge
