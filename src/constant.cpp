#include "constant.hpp"

namespace constant {

//IConstant
IConstant::IConstant(ConstantType type) : type_(type) {}

void IConstant::printBytes(std::ostream& out) const {
    out << static_cast<std::uint8_t>(type_);
}

// Utf8
Utf8::Utf8(const std::string& text) : 
    IConstant(ConstantType::Utf8)
    , text_(text)
{}

void Utf8::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << static_cast<std::uint16_t>(text_.length());
    out << text_;
}

// Integer 
Integer::Integer(int numb) :
    IConstant(ConstantType::Integer)
    , numb_(numb)
{}

void Integer::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << numb_;
}

// Float
Float::Float(float numb) :
    IConstant(ConstantType::Float)
    , numb_(numb)
{}

void Float::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << numb_;
}

// String
String::String(std::uint16_t utf8) :
    IConstant(ConstantType::String)
    , utf8_(utf8)
{}

void String::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << utf8_;
}

// NameAndType
NameAndType::NameAndType(std::uint16_t name, 
                         std::uint16_t descr) :
    IConstant(ConstantType::NameAndType)
    , name_(name)
    , descr_(descr)
{}

void NameAndType::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << name_;
    out << descr_;
}

// Class
Class::Class(std::uint16_t name) :
    IConstant(ConstantType::Class)
    , name_(name)
{}

void Class::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << name_;
}

// Fieldref
Fieldref::Fieldref(std::uint16_t cls, 
                   std::uint16_t nameNType) :
    IConstant(ConstantType::Fieldref)
    , class_(cls)
    , nameNType_(nameNType)
{}

void Fieldref::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << class_;
    out << nameNType_;
}

// Methodref
Methodref::Methodref(std::uint16_t cls, 
                   std::uint16_t nameNType) :
    IConstant(ConstantType::Methodref)
    , class_(cls)
    , nameNType_(nameNType)
{}

void Methodref::printBytes(std::ostream& out) const {
    IConstant::printBytes(out);
    out << class_;
    out << nameNType_;
}

// Descriptor
Descriptor::Descriptor(std::unique_ptr<
    descriptor::JvmFieldDescriptor> fieldType) :
    IConstant(ConstantType::Utf8)
    , fieldType_(std::move(fieldType))
{}

Descriptor::Descriptor(std::unique_ptr<
    descriptor::JvmMethodDescriptor> methodType) :
    IConstant(ConstantType::Utf8)
    , methodType_(std::move(methodType))
{}

void Descriptor::printBytes(std::ostream& out) const {
    if (fieldType_) {
        fieldType_->printBytes(out);
    }
    if (methodType_) {
        methodType_->printBytes(out);
    }
}

} // namespace constant