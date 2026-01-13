#include "symtab.hpp"

#include <stdexcept>

namespace symtab {

void SymTab::addSymbol(
    const attribute::QualifiedName& name, 
    SymbolType type, 
    std::weak_ptr<node::INode> symb)
{
#ifndef NDEBUG 
    if (contains(name)) {
        throw std::logic_error("Already contains " 
                               "a symbol with the given name");
    }
#endif
    symbols_[name] = std::make_pair(type, symb);
}

SymbolType 
SymTab::symbolType(
    const attribute::QualifiedName& name) 
{
    return symbols_[name].first;
}

std::weak_ptr<node::INode> 
SymTab::symbol(
    const attribute::QualifiedName& name) 
{
    return symbols_[name].second;    
}

bool SymTab::contains(
    const attribute::QualifiedName& name)
{
    return symbols_.contains(name);
}

void SymTab::changeNode(const attribute::QualifiedName& name, 
                        std::weak_ptr<node::INode> symb)
{
    auto&& p = symbols_.at(name);
    p.second.swap(symb);
}

void SymTab::changeType(const attribute::QualifiedName& name,
                        SymbolType type)
{
    auto&& p = symbols_.at(name);
    p.first = type;
}

SymTab globalTab;

} // namespace symtab