#include "module.hpp"

namespace mdl {

Module::Module(std::shared_ptr<node::IDecl> unit, 
               std::vector<std::shared_ptr<node::With>> imports,
               std::vector<std::shared_ptr<node::IDecl>> useDecls,
               const std::string& name) :
    unit_(unit)
    , imports_(imports)
    , useDecls_(useDecls)
    , name_(name)
{}

 void Module::print(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Module", {name_ + ".adb"});
    gv.addEdge(par, v);

    for (auto imp : imports_) {
        imp->print(gv, v);
    }
    for (auto use : useDecls_) {
        use->print(gv, v);
    }
    unit_->print(gv, v);
}


} // namespace mdl
