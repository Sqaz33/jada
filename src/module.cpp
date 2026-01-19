#include "module.hpp"

namespace mdl {

Module::Module(std::shared_ptr<node::IDecl> unit, 
               std::vector<std::shared_ptr<node::With>> with,
               std::vector<std::shared_ptr<node::Use>> use,
               const std::string& name,
               const std::string& fileName) :
    unit_(unit)
    , with_(with)
    , use_(use)
    , name_(name)
    , fileName_(fileName)
{}

 void Module::print(graphviz::GraphViz& gv, 
                    graphviz::VertexType par) const 
{
    auto v = gv.addVertex("Module", {name_ + ".adb"});
    gv.addEdge(par, v);

    for (auto imp : with_) {
        imp->print(gv, v);
    }
    for (auto use : use_) {
        use->print(gv, v);
    }
    unit_->print(gv, v);
}

std::weak_ptr<node::IDecl> Module::unit() {
    return unit_;
}

const std::vector<std::shared_ptr<node::With>>&
Module::with() const noexcept {
    return with_;
}

const std::vector<std::shared_ptr<node::Use>>&
Module::use() const noexcept {
    return use_;
}


const std::string& Module::fileName() const noexcept {
    return fileName_;
}

const std::string& Module::name() const noexcept {
    return name_;
}

void Module::resetUnit(std::shared_ptr<node::IDecl> unit) {
    unit_ = unit;
}

} // namespace mdl
