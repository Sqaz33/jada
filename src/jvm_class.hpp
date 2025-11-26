#pragma once

#include "method.hpp"
#include "field.hpp"
#include "constant_pool.hpp"


namespace jvm_class {

class JVMClass : std::enable_shared_from_this<JVMClass> {
public:

    void printBytes(std::ostream& out) const;

    constant_pool::SharedPtrJVMCP cp();

    // TODO: add field
    // TODO: add method

    auto slf() { 
        return shared_from_this(); 
    }

private:
    constant_pool::SharedPtrJVMCP jcp_;
    // TODO: fields 
    std::vector<class_member::JVMClassMethod> methods_;
    std::string name_;
};

} // namespace jvm_class