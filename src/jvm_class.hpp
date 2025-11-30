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

    const std::string& name() const noexcept;
    std::uint16_t nameIdx() const noexcept;

    auto slf() { 
        return shared_from_this(); 
    }

    // has method ...

private:
    constant_pool::SharedPtrJVMCP jcp_;
    // TODO: fields 
    std::vector<class_member::JVMClassMethod> methods_;
    std::string name_;
    std::uint16_t nameIdx_;
};

} // namespace jvm_class