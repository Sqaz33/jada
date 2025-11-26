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

    return JVMFieldDescriptor(types.at(type));
}

JVMFieldDescriptor
JVMFieldDescriptor::createObject(
    const attribute::QualifiedName& name)
{
    std::string descr("L");
    descr += name.toSring() + ";";
    return JVMFieldDescriptor(std::move(descr));
}

void JVMFieldDescriptor::addDimension() { 
    if (++dimens_ > 255) {
        throw std::logic_error("An array cannot have"
                               " more than 255 dimensions");
    }
    descr_ = "[" + descr_; 
}

const std::string& 
JVMFieldDescriptor::toString() const noexcept {
    return descr_;
}

JVMFieldDescriptor::JVMFieldDescriptor(std::string descr) :
    descr_(std::move(descr))
{}

namespace {

std::string desrcParams(
    const std::vector<JVMFieldDescriptor>& params)
{
    std::stringstream ss;
    ss << '(';
    for (auto&& p : params) {
        ss << p.toString();
    }
    ss << ')';
    return ss.str();
}

} // namespace

// JVMMethodDescriptor
JVMMethodDescriptor
JVMMethodDescriptor::createVoidRetun(
    const std::vector<JVMFieldDescriptor>& params)
{
    auto desc = desrcParams(params);
    desc += 'V';
    return JVMMethodDescriptor(std::move(desc));
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
JVMMethodDescriptor::create(        
    const std::vector<JVMFieldDescriptor>& params, 
    const JVMFieldDescriptor& ret)
{
    auto desc = desrcParams(params);
    desc += ret.toString();
    return JVMMethodDescriptor(std::move(desc));
}

JVMMethodDescriptor::
JVMMethodDescriptor(std::string descr) :
    descr_(descr)
{}

const std::string& 
JVMMethodDescriptor::toString() const noexcept {
    return descr_;
}


} //  namespace descriptor