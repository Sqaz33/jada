#include "descriptor.hpp"

#include <unordered_map>
#include <sstream>
#include <stdexcept>

namespace descriptor {

// JvmFieldDescriptor
JvmFieldDescriptor
JvmFieldDescriptor::createFundamental(
    codegen::FundamentalType type)
{   
    using namespace codegen;
    static const std::unordered_map<
        FundamentalType, std::string> types =
    { 
        {FundamentalType::INT, "I"},
        {FundamentalType::FLOAT, "F"},
        {FundamentalType::BOOLEAN, "Z"}
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
    if (++dimens_ > 255) {
        throw std::logic_error("An array cannot have"
                               " more than 255 dimensions");
    }
    descr_ = "[" + descr_; 
}

const std::string& 
JvmFieldDescriptor::toString() const noexcept {
    return descr_;
}

JvmFieldDescriptor::JvmFieldDescriptor(std::string descr) :
    descr_(std::move(descr))
{}

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

} // namespace

// JvmMethodDescriptor
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

JvmMethodDescriptor::
JvmMethodDescriptor(std::string descr) :
    descr_(descr)
{}

const std::string& 
JvmMethodDescriptor::toString() const noexcept {
    return descr_;
}


} //  namespace descriptor