#ifndef MINIFORGE_COMMON_SOURCELOCATION_HPP
#define MINIFORGE_COMMON_SOURCELOCATION_HPP

#include <string>
#include <sstream>
#include <cstddef>
#include <cstdint>

namespace miniforge {

struct SourceLocation {
    size_t line{1};
    size_t column{1};

    SourceLocation() = default;
    SourceLocation(size_t l, size_t c) : line(l), column(c) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << line << ":" << column;
        return oss.str();
    }
};

} // namespace miniforge

#endif // MINIFORGE_COMMON_SOURCELOCATION_HPP
