#pragma once

#include "method.hpp"
#include "field.hpp"
#include "constant_pool.hpp"
#include "attribute.hpp"

namespace jvm_class {

class JVMClass : 
    std::enable_shared_from_this<JVMClass> 
{
public:
    JVMClass(
        const attribute::QualifiedName& name,
        std::uint16_t majorV,
        std::uint16_t minorV);

public:
    void printBytes(std::ostream& out) const;

public:
    constant_pool::SharedPtrJVMCP cp();
    const std::string& name() const noexcept;
    std::uint16_t nameIdx() const noexcept;
    auto slf() { 
        return shared_from_this(); 
    }
    void setParent(std::weak_ptr<JVMClass> par);
    void addAttr(std::shared_ptr<jvm_attribute::IAttribute> attr);
    void addAccesFlag(codegen::AccessFlag accf);

    const std::string& simpleName() const noexcept; 

public:
    class_member::SharedPtrField addField( 
        const std::string& name,
        descriptor::JVMFieldDescriptor type);
    class_member::SharedPtrMethod addMethod( 
        const std::string& name,
        descriptor::JVMMethodDescriptor type);

private:
    // class internal
    std::string name_;
    std::string simpleName_;
    std::weak_ptr<JVMClass> parent_;

    // bytes structure 
    std::uint16_t minorV_;
    std::uint16_t majorV_;
    constant_pool::SharedPtrJVMCP cp_;
    std::uint16_t accf_ = 0;
    std::uint16_t nameIdx_;
    std::uint16_t parentIdx_ = 0;
    std::vector<class_member::SharedPtrField> fields_;
    std::vector<class_member::SharedPtrMethod> methods_;
    std::vector<
        std::shared_ptr<jvm_attribute::IAttribute>> attrs_;
};

using SharedPtrJVMClass = std::shared_ptr<JVMClass>;

} // namespace jvm_class