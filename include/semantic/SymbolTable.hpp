#ifndef MINIFORGE_SEMANTIC_SYMBOLTABLE_HPP
#define MINIFORGE_SEMANTIC_SYMBOLTABLE_HPP

#include "common/SourceLocation.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace miniforge {

struct Symbol {
    std::string name;
    SourceLocation location;
    bool isInitialized{false};

    Symbol() = default;
    Symbol(std::string n, SourceLocation loc, bool init = true)
        : name(std::move(n)), location(loc), isInitialized(init) {}
};

class Scope {
public:
    explicit Scope(Scope* parent = nullptr) : parent_(parent) {}

    Scope* parent() const { return parent_; }

    bool define(const Symbol& sym) {
        if (symbols_.find(sym.name) != symbols_.end()) {
            return false; // Duplicate declaration in the same scope
        }
        symbols_[sym.name] = sym;
        return true;
    }

    Symbol* resolve(const std::string& name) {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return &it->second;
        }
        if (parent_ != nullptr) {
            return parent_->resolve(name);
        }
        return nullptr;
    }

    const Symbol* resolve(const std::string& name) const {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return &it->second;
        }
        if (parent_ != nullptr) {
            return parent_->resolve(name);
        }
        return nullptr;
    }

    bool existsInCurrentScope(const std::string& name) const {
        return symbols_.find(name) != symbols_.end();
    }

private:
    std::unordered_map<std::string, Symbol> symbols_;
    Scope* parent_{nullptr};
};

class SymbolTable {
public:
    SymbolTable() {
        // Create the global / root scope
        enterScope();
    }

    void enterScope() {
        scopes_.push_back(std::make_unique<Scope>(currentScope_));
        currentScope_ = scopes_.back().get();
    }

    void exitScope() {
        if (currentScope_ != nullptr) {
            currentScope_ = currentScope_->parent();
        }
    }

    bool declare(const std::string& name, SourceLocation loc) {
        if (currentScope_ == nullptr) return false;
        return currentScope_->define(Symbol(name, loc, true));
    }

    Symbol* resolve(const std::string& name) {
        if (currentScope_ == nullptr) return nullptr;
        return currentScope_->resolve(name);
    }

    bool existsInCurrentScope(const std::string& name) const {
        if (currentScope_ == nullptr) return false;
        return currentScope_->existsInCurrentScope(name);
    }

private:
    std::vector<std::unique_ptr<Scope>> scopes_;
    Scope* currentScope_{nullptr};
};

} // namespace miniforge

#endif // MINIFORGE_SEMANTIC_SYMBOLTABLE_HPP
