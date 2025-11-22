#pragma once 

#include <vector>
#include <memory>
#include <unordered_map>

#include "codegen_enums.hpp"
#include "jvm_attribute.hpp"
#include "constant_pool.hpp"

namespace class_member {

class JVMClassField;
class JVMClassMethod;

class IJVMClassMember {
public:
    using UniquePtrAttr = 
        std::unique_ptr<jvm_attribute::IAttribute>;

    using AttrPtr = jvm_attribute::IAttribute*;

public:
    friend class JVMClassField;
    friend class JVMClassMethod;

public:
    void printBytes(std::ostream& out) const;
    void addFlag(codegen::AccessFlag flag);
    void addAttr(UniquePtrAttr attr);

private:
    // utf8, utf8 - constant_pool idx 
    IJVMClassMember(
        std::uint16_t name, 
        std::uint16_t desc,
        bool isMethod,
        constant_pool::SharedPtrJVMCP cp
    ); 

private:
    // byte structure
    std::uint16_t accf_;
    std::uint16_t name_; 
    std::uint16_t desc_;
    std::vector<UniquePtrAttr> attrs_;
    
    // class internal
    std::map<std::uint16_t, AttrPtr> attrNames_;
    constant_pool::SharedPtrJVMCP cp_;
    bool isMethod_;
};

} // namespace class_member
