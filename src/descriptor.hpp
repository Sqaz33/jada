#pragma once

#include <ostream>
#include <vector>

#include "codegen_enums.hpp"
#include "attribute.hpp"

namespace descriptor {

class JvmFieldDescriptor {
public:
    static JvmFieldDescriptor
    createFundamental(codegen::FundamentalType type);

    static JvmFieldDescriptor
    createObject(const attribute::QualifiedName& name);

public:
    // throw exception, if dimensions more then 255
    void addDimension(); 
    const std::string& toString() const noexcept;

private:
    JvmFieldDescriptor(std::string);

private:
    std::string descr_;
    int dimens_ = 0;
};

class JvmMethodDescriptor {
public:
    static JvmMethodDescriptor
    createVoidRetun(
        const std::vector<JvmFieldDescriptor>& params);
    
    static JvmMethodDescriptor
    createVoidParams(const JvmFieldDescriptor& ret);

    static JvmMethodDescriptor
    create(        
        const std::vector<JvmFieldDescriptor>& params, 
        const JvmFieldDescriptor& ret);

public:
    const std::string& toString() const noexcept;

private:
    JvmMethodDescriptor(std::string);

private:
    std::string descr_;
};

} // namespace descriptor