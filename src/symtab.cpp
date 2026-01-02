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

} // namespace symtab