#include "descriptor.hpp"

#include <unordered_map>
#include <sstream>
#include <stdexcept>

namespace descriptor {

// JVMFieldDescriptor
JVMFieldDescriptor
JVMFieldDescriptor::createFundamental(
    codegen::FundamentalType type)
{   
    using namespace codegen;
    static const std::unordered_map<
        FundamentalType, std::string> types =
    { 
        {FundamentalType::INT,     "I"},
        {FundamentalType::FLOAT,   "F"},
        {FundamentalType::DOUBLE,  "D"},
        {FundamentalType::BOOLEAN, "Z"},
        {FundamentalType::LONG,    "J"}
    };

    int sz  = FundamentalType::LONG == type ||
                FundamentalType::DOUBLE == type 
                ? 2 : 1;

    return JVMFieldDescriptor(types.at(type), sz);
}

JVMFieldDescriptor
JVMFieldDescriptor::createObject(
    const attribute::QualifiedName& name)
{
    std::string descr("L");
    descr += name.toSring() + ";";
    return JVMFieldDescriptor(std::move(descr), 1);
}

void JVMFieldDescriptor::addDimension() { 
    if (++dimens_ > 255) {
        throw std::logic_error("An array cannot have"
                               " more than 255 dimensions");
    }
    descr_ = "[" + descr_; 
    sz_ = 1;
}

const std::string& 
JVMFieldDescriptor::toString() const noexcept {
    return descr_;
}

int JVMFieldDescriptor::size() const noexcept {
    return sz_;
}

JVMFieldDescriptor::JVMFieldDescriptor(std::string descr, int sz) :
    descr_(std::move(descr))
    , sz_(sz)
{}

namespace {

std::string desrcParams(
    const std::vector<
        std::pair<std::string,JVMFieldDescriptor>>& params)
{
    std::stringstream ss;
    ss << '(';
    for (auto&& [_, p] : params) {
        ss << p.toString();
    }
    ss << ')';
    return ss.str();
}

} // namespace

// JVMMethodDescriptor
JVMMethodDescriptor
JVMMethodDescriptor::createVoidRetun(
    const std::vector<
        std::pair<std::string,JVMFieldDescriptor>>& params)
{
    auto desc = desrcParams(params);
    desc += 'V';
    std::vector<std::pair<std::string, int>> p;
    for (auto [name, desc] : params) {
        p.emplace_back(name, desc.size());
    }
    return JVMMethodDescriptor(std::move(desc), std::move(p));
}

JVMMethodDescriptor
JVMMethodDescriptor::createVoidParams(
    const JVMFieldDescriptor& ret) 
{
    std::string desc("()");
    desc += ret.toString();
    return JVMMethodDescriptor(std::move(desc));
}

JVMMethodDescriptor
JVMMethodDescriptor::createVoidParamsVoidReturn() {
    std::string desc("()");
    desc += 'V';
    return JVMMethodDescriptor(std::move(desc));
}

JVMMethodDescriptor
JVMMethodDescriptor::create(        
    const std::vector<
            std::pair<std::string,JVMFieldDescriptor>>& params, 
    const JVMFieldDescriptor& ret)
{
    auto desc = desrcParams(params);
    desc += ret.toString();
    std::vector<std::pair<std::string, int>> p;
    for (auto [name, desc] : params) {
        p.emplace_back(name, desc.size());
    }
    return JVMMethodDescriptor(std::move(desc), std::move(p));
}

const std::string& 
JVMMethodDescriptor::toString() const noexcept {
    return descr_;
}

const std::vector<std::pair<std::string, int>>& 
JVMMethodDescriptor::params() const noexcept {
    return params_;
}

JVMMethodDescriptor::
JVMMethodDescriptor(std::string descr,
    std::vector<std::pair<std::string, int>> params) :
    descr_(descr)
    , params_(std::move(params))
{}

} //  namespace descriptor