#ifndef MINIFORGE_OPTIMIZER_CONSTANTPROPAGATOR_HPP
#define MINIFORGE_OPTIMIZER_CONSTANTPROPAGATOR_HPP

#include "optimizer/OptimizationPass.hpp"

namespace miniforge {

class ConstantPropagator : public OptimizationPass {
public:
    std::string_view name() const override { return "ConstantPropagator"; }

    bool run(IRProgram& program) override;
};

} // namespace miniforge

#endif // MINIFORGE_OPTIMIZER_CONSTANTPROPAGATOR_HPP
