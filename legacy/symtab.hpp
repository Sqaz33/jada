#pragma once

#include <memory>
#include <map>
#include <utility>

#include "attribute.hpp"
#include "node.hpp"

namespace symtab {

enum class SymbolType {
    SUBPROG,
    CLASS,
    ALIAS,
    TYPE
};

} // namespace symtab

namespace symtab {

class SymTab {
public:
    void addSymbol(
        const attribute::QualifiedName& name, 
        SymbolType type, 
        std::weak_ptr<node::INode> symb);

    SymbolType 
    symbolType(const attribute::QualifiedName& name);

    std::weak_ptr<node::INode> 
    symbol(const attribute::QualifiedName& name);

    bool contains(const attribute::QualifiedName& name);

    void changeNode(const attribute::QualifiedName& name, 
                    std::weak_ptr<node::INode> symb);

    void changeType(const attribute::QualifiedName& name,
                    SymbolType type);

private:
    std::map<attribute::QualifiedName, 
            std::pair<
                SymbolType, std::weak_ptr<node::INode>>> 
    symbols_;
};

extern SymTab globalTab;

} // namespace symtab

// TODO delete
// 1. заполняется только структурами, доступными из глобала