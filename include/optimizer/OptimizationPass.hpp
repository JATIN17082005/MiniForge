#ifndef MINIFORGE_OPTIMIZER_OPTIMIZATIONPASS_HPP
#define MINIFORGE_OPTIMIZER_OPTIMIZATIONPASS_HPP

#include "ir/IR.hpp"
#include <string_view>

namespace miniforge {

class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;
    virtual std::string_view name() const = 0;

    // Runs the pass on the given IRProgram. Returns true if any changes were made.
    virtual bool run(IRProgram& program) = 0;
};

} // namespace miniforge

#endif // MINIFORGE_OPTIMIZER_OPTIMIZATIONPASS_HPP
