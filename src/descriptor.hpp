#pragma once

#include <ostream>
#include <vector>

#include "codegen.hpp"
#include "attribute.hpp"

namespace descriptor {

class JvmFieldDescriptor {
public:
    static JvmFieldDescriptor
    createFundamental(codegen::FundamentalTypes type);

    static JvmFieldDescriptor
    createObject(const attribute::QualifiedName& name);

public:
    void addDimension();
    void printBytes(std::ostream& out) const;
    const std::string& toString() const noexcept;

private:
    JvmFieldDescriptor(std::string);

private:
    std::string descr_;
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
    void printBytes(std::ostream& out) const;

private:
    JvmMethodDescriptor(std::string);

private:
    std::string descr_;
};

} // namespace descriptor