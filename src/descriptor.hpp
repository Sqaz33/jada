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
    int size() const noexcept;

private:
    JVMFieldDescriptor(std::string, int);

private:
    std::string descr_;
    int sz_;
    int dimens_ = 0;
};

class JVMMethodDescriptor {
public:
    static JVMMethodDescriptor
    createVoidRetun(
        const std::vector<
            std::pair<std::string,JVMFieldDescriptor>>& params);
    
    static JVMMethodDescriptor
    createVoidParams(const JVMFieldDescriptor& ret);

    static JVMMethodDescriptor
    createVoidParamsVoidreturn();

    static JVMMethodDescriptor
    create(        
        const std::vector<
            std::pair<std::string,JVMFieldDescriptor>>& params, 
        const JVMFieldDescriptor& ret);

public:
    const std::string& toString() const noexcept;
    const std::vector<std::pair<std::string, int>>& 
    params() const noexcept;

private:
    JVMMethodDescriptor(std::string, 
        std::vector<std::pair<std::string, int>> params = {});

private:
    std::string descr_;
    std::vector<std::pair<std::string, int>> params_;
};

} // namespace descriptor