#ifndef MINIFORGE_OPTIMIZER_CONSTANTFOLDER_HPP
#define MINIFORGE_OPTIMIZER_CONSTANTFOLDER_HPP

#include "optimizer/OptimizationPass.hpp"

namespace miniforge {

class ConstantFolder : public OptimizationPass {
public:
    std::string_view name() const override { return "ConstantFolder"; }

    bool run(IRProgram& program) override;
};

} // namespace miniforge

#endif // MINIFORGE_OPTIMIZER_CONSTANTFOLDER_HPP
