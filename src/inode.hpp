#pragma once

namespace node {

struct INode {
    virtual void print() = 0;
};

struct IScope : INode {
    virtual void addBranch(INode* br) = 0;
    virtual IScope* push() = 0;
    virtual IScope* reset() = 0;
};

}
