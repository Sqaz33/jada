#include "compile_unit.hpp"

namespace compile_unit {

CompileUnit::CompileUnit(std::shared_ptr<node::IDecl> unit, 
                         std::vector<std::shared_ptr<node::With>> imports,
                         std::vector<std::shared_ptr<node::IDecl>> useDecls) :
    unit_(unit)
    , imports_(imports)
    , useDecls_(useDecls)
{}

void CompileUnit::print() const {
    for (auto imp : imports_) {
        imp->print(0);
    }
    for (auto use : useDecls_) {
        use->print(0);
    }
    unit_->print(0);
}


} // namespace compile_unit
