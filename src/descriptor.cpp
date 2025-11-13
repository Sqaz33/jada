#include "descriptor.hpp"

#include <unordered_map>
#include <sstream>

namespace descriptor {

// JvmFieldDescriptor
JvmFieldDescriptor
JvmFieldDescriptor::createFundamental(
    codegen::FundamentalTypes type)
{   
    using namespace codegen;
    static const std::unordered_map<
        FundamentalTypes, std::string> types =
    { 
        {FundamentalTypes::INT, "I"},
        {FundamentalTypes::FLOAT, "F"},
        {FundamentalTypes::BOOLEAN, "Z"}
    };

    return JvmFieldDescriptor(types.at(type));
}

JvmFieldDescriptor
JvmFieldDescriptor::createObject(
    const attribute::QualifiedName& name)
{
    std::string descr("L");
    descr += name.toSring() + ";";
    return JvmFieldDescriptor(std::move(descr));
}

void JvmFieldDescriptor::addDimension() { 
    descr_ = "[" + descr_; 
}

void JvmFieldDescriptor::printBytes(
    std::ostream& out) const 
{ out << descr_; }

const std::string& 
JvmFieldDescriptor::toString() const noexcept {
    return descr_;
}

JvmFieldDescriptor::JvmFieldDescriptor(std::string descr) :
    descr_(std::move(descr))
{}

// JvmMethodDescriptor

namespace {

std::string desrcParams(
    const std::vector<JvmFieldDescriptor>& params)
{
    std::stringstream ss;
    ss << '(';
    for (auto&& p : params) {
        ss << p.toString();
    }
    ss << ')';
    return ss.str();
}

}

JvmMethodDescriptor
JvmMethodDescriptor::createVoidRetun(
    const std::vector<JvmFieldDescriptor>& params)
{
    auto desc = desrcParams(params);
    desc += 'V';
    return JvmMethodDescriptor(std::move(desc));
}

JvmMethodDescriptor
JvmMethodDescriptor::createVoidParams(
    const JvmFieldDescriptor& ret) 
{
    std::string desc("()");
    desc += ret.toString();
    return JvmMethodDescriptor(std::move(desc));
}

JvmMethodDescriptor
JvmMethodDescriptor::create(        
    const std::vector<JvmFieldDescriptor>& params, 
    const JvmFieldDescriptor& ret)
{
    auto desc = desrcParams(params);
    desc += ret.toString();
    return JvmMethodDescriptor(std::move(desc));
}

void JvmMethodDescriptor::printBytes(
    std::ostream& out) const 
{ out << descr_; }

JvmMethodDescriptor::
JvmMethodDescriptor(std::string descr) :
    descr_(descr)
{}

} //  namespace descriptor