#pragma once

#include "node.hpp"
#include "graphviz.hpp"

namespace mdl {

class Module {
public:
    Module(std::shared_ptr<node::IDecl> unit, 
           std::vector<std::shared_ptr<node::With>> with,
           std::vector<std::shared_ptr<node::Use>> use,
           const std::string& name,
           const std::string& fileName);

public:
    void print(graphviz::GraphViz& gv, 
               graphviz::VertexType par) const;

    std::weak_ptr<node::IDecl> unit();
    const std::vector<std::shared_ptr<node::With>>&
    with() const noexcept;

    const std::string& fileName() const noexcept;

    const std::string& name() const noexcept;

 private:
    std::shared_ptr<node::IDecl> unit_;
    std::vector<std::shared_ptr<node::With>> with_;
    std::vector<std::shared_ptr<node::Use>> use_;
    std::string name_;
    std::string fileName_;
};

} // namespace mdl