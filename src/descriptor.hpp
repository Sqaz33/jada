#pragma once

#include <ostream>
#include <vector>

#include "codegen_enums.hpp"
#include "attribute.hpp"

namespace descriptor {

class JVMFieldDescriptor {
public:
    static JVMFieldDescriptor
    createFundamental(codegen::FundamentalType type);

    static JVMFieldDescriptor
    createObject(const attribute::QualifiedName& name);

public:
    // throw exception, if dimensions more then 255
    void addDimension(); 
    const std::string& toString() const noexcept;

private:
    JVMFieldDescriptor(std::string);

private:
    std::string descr_;
    int dimens_ = 0;
};

class JVMMethodDescriptor {
public:
    static JVMMethodDescriptor
    createVoidRetun(
        const std::vector<JVMFieldDescriptor>& params);
    
    static JVMMethodDescriptor
    createVoidParams(const JVMFieldDescriptor& ret);

    static JVMMethodDescriptor
    create(        
        const std::vector<JVMFieldDescriptor>& params, 
        const JVMFieldDescriptor& ret);

public:
    const std::string& toString() const noexcept;

private:
    JVMMethodDescriptor(std::string);

private:
    std::string descr_;
};

} // namespace descriptor