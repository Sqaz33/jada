#pragma once

#include "node.hpp"

namespace compile_unit {

class CompileUnit {
public:
    CompileUnit(std::shared_ptr<node::IDecl> unit, 
                std::vector<std::shared_ptr<node::With>> imports,
                std::vector<std::shared_ptr<node::IDecl>> useDecls);  // TODO: file name...

public:
    void print() const;

private:
    std::shared_ptr<node::IDecl> unit_;
    std::vector<std::shared_ptr<node::With>> imports_;
    std::vector<std::shared_ptr<node::IDecl>> useDecls_;
};

} // namespace compile_unit