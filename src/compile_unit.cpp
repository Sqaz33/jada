#include "compile_unit.hpp"

namespace compile_unit {

CompileUnit::CompileUnit(std::shared_ptr<node::IDecl> unit, 
                         std::vector<std::shared_ptr<node::With>> imports,
                         std::vector<std::shared_ptr<node::IDecl>> useDecls) :
    unit_(unit)
    , imports_(imports)
    , useDecls_(useDecls)
{}

void CompileUnit::print(graphviz::GraphViz& gv) const {
    auto v = gv.addVertex("Program");

    for (auto imp : imports_) {
        imp->print(gv, v);
    }
    for (auto use : useDecls_) {
        use->print(gv, v);
    }
    unit_->print(gv, v);
}


} // namespace compile_unit
