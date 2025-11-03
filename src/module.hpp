#pragma once

#include "node.hpp"
#include "graphviz.hpp"

namespace mdl {

class Module {
public:
    Module(std::shared_ptr<node::IDecl> unit, 
           std::vector<std::shared_ptr<node::With>> imports,
           std::vector<std::shared_ptr<node::IDecl>> useDecls,
           const std::string& name);

public:
    void print(graphviz::GraphViz& gv) const;

private:
    std::shared_ptr<node::IDecl> unit_;
    std::vector<std::shared_ptr<node::With>> imports_;
    std::vector<std::shared_ptr<node::IDecl>> useDecls_;
    std::string name_;
};

} // namespace mdl