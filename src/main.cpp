#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "ir/IRBuilder.hpp"
#include "optimizer/Optimizer.hpp"
#include "codegen/CodeGenerator.hpp"
#include "codegen/Disassembler.hpp"
#include "vm/VM.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace miniforge;

static void printUsage(const char* exeName) {
    std::cout << "MiniForge Compiler & Virtual Machine\n\n"
              << "Usage:\n"
              << "  " << exeName << " run <file.mf> [--no-opt] [--trace]\n"
              << "  " << exeName << " tokens <file.mf>\n"
              << "  " << exeName << " ast <file.mf>\n"
              << "  " << exeName << " ir <file.mf> [--no-opt]\n"
              << "  " << exeName << " bytecode <file.mf> [--no-opt]\n"
              << "  " << exeName << " --help | -h\n\n"
              << "Commands:\n"
              << "  run       Compile and execute the program on the virtual machine\n"
              << "  tokens    Run lexical analysis and print tokens\n"
              << "  ast       Parse source and print abstract syntax tree\n"
              << "  ir        Generate and print Three-Address Code intermediate representation\n"
              << "  bytecode  Generate and disassemble virtual machine bytecode\n\n"
              << "Options:\n"
              << "  --no-opt  Disable compiler optimization passes\n"
              << "  --trace   Enable step-by-step virtual machine execution tracing\n"
              << "  --help    Display this help message\n";
}

static bool readFile(const std::string& path, std::string& content) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "miniforge: error: cannot open file '" << path << "'\n";
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    content = ss.str();
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string command = argv[1];
    if (command == "--help" || command == "-h" || command == "help") {
        printUsage(argv[0]);
        return 0;
    }

    if (argc < 3) {
        std::cerr << "miniforge: error: missing input file for command '" << command << "'\n";
        printUsage(argv[0]);
        return 1;
    }

    std::string filePath = argv[2];
    bool noOpt = false;
    bool trace = false;

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--no-opt") {
            noOpt = true;
        } else if (arg == "--trace") {
            trace = true;
        } else {
            std::cerr << "miniforge: error: unrecognized option '" << arg << "'\n";
            return 1;
        }
    }

    std::string source;
    if (!readFile(filePath, source)) {
        return 1;
    }

    DiagnosticEngine diags;

    // Stage 1: Lexical Analysis
    Lexer lexer(source, &diags);
    auto tokens = lexer.tokenize();

    if (command == "tokens") {
        for (const auto& tok : tokens) {
            std::cout << tok.toString() << "\n";
        }
        if (diags.hasErrors()) {
            diags.printAll(std::cerr, filePath);
            return 1;
        }
        return 0;
    }

    if (diags.hasErrors()) {
        diags.printAll(std::cerr, filePath);
        return 1;
    }

    // Stage 2: Parser & AST
    Parser parser(std::move(tokens), &diags);
    auto program = parser.parseProgram();

    if (diags.hasErrors() || !program) {
        diags.printAll(std::cerr, filePath);
        return 1;
    }

    if (command == "ast") {
        std::cout << program->toString();
        return 0;
    }

    // Stage 3: Semantic Analysis
    SemanticAnalyzer sema(diags);
    if (!sema.analyze(*program)) {
        diags.printAll(std::cerr, filePath);
        return 1;
    }

    // Stage 4: Three-Address IR Generation
    IRBuilder irBuilder;
    auto ir = irBuilder.build(*program);

    // Stage 5: Optimization
    if (!noOpt) {
        Optimizer optimizer;
        optimizer.optimize(ir);
    }

    if (command == "ir") {
        std::cout << ir.toString();
        return 0;
    }

    // Stage 6: Bytecode Generation
    CodeGenerator codegen(&diags);
    auto bytecode = codegen.generate(ir);

    if (diags.hasErrors()) {
        diags.printAll(std::cerr, filePath);
        return 1;
    }

    if (command == "bytecode") {
        std::cout << Disassembler::disassemble(bytecode);
        return 0;
    }

    // Stage 7: Virtual Machine Execution
    if (command == "run") {
        VirtualMachine vm(std::cout);
        return vm.run(bytecode, trace);
    }

    std::cerr << "miniforge: error: unrecognized command '" << command << "'\n";
    printUsage(argv[0]);
    return 1;
}
