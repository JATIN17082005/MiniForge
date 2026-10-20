#include "optimizer/Optimizer.hpp"

namespace miniforge {

Optimizer::Optimizer() {
    // Standard optimization pipeline:
    // 1. Constant propagation
    // 2. Constant folding
    // 3. Dead temporary elimination
    passes_.push_back(std::make_unique<ConstantPropagator>());
    passes_.push_back(std::make_unique<ConstantFolder>());
    passes_.push_back(std::make_unique<DeadCodeEliminator>());
}

int Optimizer::optimize(IRProgram& program, int maxIterations) {
    int iterations = 0;
    bool changed = true;

    while (changed && iterations < maxIterations) {
        changed = false;
        for (auto& pass : passes_) {
            if (pass->run(program)) {
                changed = true;
            }
        }
        if (changed) {
            iterations++;
        }
    }

    return iterations;
}

} // namespace miniforge
