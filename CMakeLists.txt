cmake_minimum_required(VERSION 3.15)
project(MiniForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Core library sources
set(MINIFORGE_LIB_SOURCES
    src/common/Diagnostic.cpp
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
    src/ast/AST.cpp
    src/parser/Parser.cpp
    src/semantic/SemanticAnalyzer.cpp
    src/ir/IR.cpp
    src/ir/IRBuilder.cpp
    src/optimizer/ConstantFolder.cpp
    src/optimizer/ConstantPropagator.cpp
    src/optimizer/DeadCodeEliminator.cpp
    src/optimizer/Optimizer.cpp
    src/codegen/Bytecode.cpp
    src/codegen/CodeGenerator.cpp
    src/codegen/Disassembler.cpp
    src/vm/VM.cpp
)

add_library(miniforge_lib STATIC ${MINIFORGE_LIB_SOURCES})

# CLI executable
add_executable(miniforge src/main.cpp)
target_link_libraries(miniforge PRIVATE miniforge_lib)

# Test executable
set(MINIFORGE_TEST_SOURCES
    tests/test_main.cpp
    tests/test_lexer.cpp
    tests/test_parser.cpp
    tests/test_semantic.cpp
    tests/test_ir.cpp
    tests/test_optimizer.cpp
    tests/test_codegen.cpp
    tests/test_vm.cpp
    tests/test_e2e.cpp
)

add_executable(miniforge_tests ${MINIFORGE_TEST_SOURCES})
target_link_libraries(miniforge_tests PRIVATE miniforge_lib)
target_include_directories(miniforge_tests PRIVATE ${PROJECT_SOURCE_DIR}/tests)

enable_testing()
add_test(NAME MiniForgeAllTests COMMAND miniforge_tests)
