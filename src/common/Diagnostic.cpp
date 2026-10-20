#include "common/Diagnostic.hpp"

namespace miniforge {

std::string Diagnostic::toString(const std::string& filename) const {
    std::string result;
    if (!filename.empty()) {
        result += filename + ":";
    }
    result += location.toString() + ": ";
    switch (severity) {
        case DiagnosticSeverity::Note:
            result += "note: ";
            break;
        case DiagnosticSeverity::Warning:
            result += "warning: ";
            break;
        case DiagnosticSeverity::Error:
            result += "error: ";
            break;
    }
    result += message;
    return result;
}

void DiagnosticEngine::printAll(std::ostream& os, const std::string& filename) const {
    for (const auto& diag : diagnostics_) {
        os << diag.toString(filename) << "\n";
    }
}

} // namespace miniforge
