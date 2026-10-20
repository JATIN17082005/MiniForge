#ifndef MINIFORGE_COMMON_DIAGNOSTIC_HPP
#define MINIFORGE_COMMON_DIAGNOSTIC_HPP

#include "common/SourceLocation.hpp"
#include <string>
#include <vector>
#include <iostream>

namespace miniforge {

enum class DiagnosticSeverity {
    Note,
    Warning,
    Error
};

struct Diagnostic {
    DiagnosticSeverity severity{DiagnosticSeverity::Error};
    SourceLocation location;
    std::string message;

    Diagnostic(DiagnosticSeverity sev, SourceLocation loc, std::string msg)
        : severity(sev), location(loc), message(std::move(msg)) {}

    std::string toString(const std::string& filename = "") const;
};

class DiagnosticEngine {
public:
    void report(DiagnosticSeverity severity, SourceLocation loc, const std::string& message) {
        diagnostics_.emplace_back(severity, loc, message);
    }

    void error(SourceLocation loc, const std::string& message) {
        report(DiagnosticSeverity::Error, loc, message);
    }

    void warning(SourceLocation loc, const std::string& message) {
        report(DiagnosticSeverity::Warning, loc, message);
    }

    void note(SourceLocation loc, const std::string& message) {
        report(DiagnosticSeverity::Note, loc, message);
    }

    bool hasErrors() const {
        for (const auto& diag : diagnostics_) {
            if (diag.severity == DiagnosticSeverity::Error) {
                return true;
            }
        }
        return false;
    }

    const std::vector<Diagnostic>& getDiagnostics() const {
        return diagnostics_;
    }

    void printAll(std::ostream& os = std::cerr, const std::string& filename = "") const;

    void clear() {
        diagnostics_.clear();
    }

private:
    std::vector<Diagnostic> diagnostics_;
};

} // namespace miniforge

#endif // MINIFORGE_COMMON_DIAGNOSTIC_HPP
