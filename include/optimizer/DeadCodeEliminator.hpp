#ifndef MINIFORGE_OPTIMIZER_DEADCODEELIMINATOR_HPP
#define MINIFORGE_OPTIMIZER_DEADCODEELIMINATOR_HPP

#include "optimizer/OptimizationPass.hpp"

namespace miniforge {

class DeadCodeEliminator : public OptimizationPass {
public:
    std::string_view name() const override { return "DeadCodeEliminator"; }

    bool run(IRProgram& program) override;
};

} // namespace miniforge

#endif // MINIFORGE_OPTIMIZER_DEADCODEELIMINATOR_HPP
