#pragma once 

#include <vector>
#include <memory>
#include <unordered_map>

#include "codegen_enums.hpp"
#include "jvm_attribute.hpp"
#include "constant_pool.hpp"

namespace class_member {

class IJVMClassMember {
    using SharedPtrAttr = 
        std::shared_ptr<jvm_attribute::IAttribute>;

public:
    void printBytes(std::ostream& out) const;

public: 
    void addFlag(codegen::AccessFlag flag);
    void addAttr(SharedPtrAttr attr);
    bool isStatic() const noexcept;

    std::uint16_t name() const noexcept;
    std::uint16_t type() const noexcept;

protected:
    // utf8, utf8 - constant_pool idx 
    IJVMClassMember(
        std::uint16_t name, 
        std::uint16_t desc,
        bool isMethod,
        constant_pool::SharedPtrJVMCP cp
    ); 

private:
    // byte structure
    std::uint16_t accf_ = 0;
    std::uint16_t name_; 
    std::uint16_t desc_;
    std::vector<SharedPtrAttr> attrs_;
    
    // class internal
    constant_pool::SharedPtrJVMCP cp_;
    bool isMethod_;
    bool isStatic_ = false;
};

} // namespace class_member
