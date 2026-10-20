#include "test_framework.hpp"

int main() {
    return miniforge::test::TestRegistry::instance().runAll();
}
