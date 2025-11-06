#pragma once

#include "location.hh"

#include "graphviz.hpp"

namespace node {    

struct INode {
    virtual void print(graphviz::GraphViz& gv, 
                       graphviz::VertexType par) const = 0;
    virtual void* codegen() = 0; // TODO
    virtual ~INode() = default;

    void setLocation(const yy::location& loc);

protected:
    yy::location loc;
};

} // namespace node
