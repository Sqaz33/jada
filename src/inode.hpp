#pragma once

#include "location.hh"


namespace node {

struct INode {
    virtual void print(int spc) const = 0;
    virtual INode* calc() /* = 0 */;
    virtual void* codegen() = 0; // TODO
    virtual ~INode() = default;

    void setLocation(const yy::location& loc);

protected:
    yy::location loc;
};

} // namespace node
