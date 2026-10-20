#ifndef MINIFORGE_OPTIMIZER_OPTIMIZER_HPP
#define MINIFORGE_OPTIMIZER_OPTIMIZER_HPP

#include "optimizer/OptimizationPass.hpp"
#include "optimizer/ConstantFolder.hpp"
#include "optimizer/ConstantPropagator.hpp"
#include "optimizer/DeadCodeEliminator.hpp"
#include <vector>
#include <memory>

namespace miniforge {

class Optimizer {
public:
    Optimizer();

    void addPass(std::unique_ptr<OptimizationPass> pass) {
        passes_.push_back(std::move(pass));
    }

    // Runs optimization passes until no further modifications occur or maxIterations reached.
    // Returns total number of modification iterations performed.
    int optimize(IRProgram& program, int maxIterations = 10);

    const std::vector<std::unique_ptr<OptimizationPass>>& passes() const {
        return passes_;
    }

private:
    std::vector<std::unique_ptr<OptimizationPass>> passes_;
};

} // namespace miniforge

#endif // MINIFORGE_OPTIMIZER_OPTIMIZER_HPP
